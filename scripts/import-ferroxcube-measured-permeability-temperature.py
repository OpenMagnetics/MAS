#!/usr/bin/env python3
"""Replace the Ferroxcube mu_i(T) points of core_materials.ndjson with Ferroxcube's own
measurements, read from the measurement workbook Ferroxcube sent ("FXC Materials Power.xlsx").

WHAT IT READS
    Sheet "data", the block headed "mu_i 10 kHz 0,1 mT": one row per temperature (-40..180 C in
    10 C steps, column D) and one column per grade, named in row 2 (the same header row the
    workbook's own "ui(T)" chart takes its series names from).  The sample is the toroid named
    in cell E1.  The workbook is read with the standard library only (it is an Office Open XML
    zip); cached cell values are used, nothing is recomputed.

WHAT IT WRITES
    For every grade column whose name is a MAS material, each mu_i(T) point at a measured
    temperature (on the lowest-frequency, lowest-bias sweep, which is the one MKF evaluates)
    gets the workbook value, unrounded.  A measured temperature with no existing point is added
    with the sweep's frequency.  Points outside the measured range are left as they are (they
    come from other sources and carry their own provenance).  Nothing is interpolated,
    extrapolated, fitted or scaled.  One provenance entry per grade.

    Two workbook columns are not grades of their own: "3F46 long" and "3F46 short" are two
    samples of 3F46.  The MAS 3F46 sweep is the "long" sample (it matches it point for point),
    so that column is used and the other is not.  "3F46A short" has no MAS record and is
    skipped.  Any other column without a MAS record is reported and skipped.

Usage:
    python3 scripts/import-ferroxcube-measured-permeability-temperature.py WORKBOOK.xlsx [--patch data/core_materials.ndjson]
"""
import argparse
import datetime
import json
import os
import re
import sys
import zipfile
import xml.etree.ElementTree as ET

M = '{http://schemas.openxmlformats.org/spreadsheetml/2006/main}'
R = '{http://schemas.openxmlformats.org/officeDocument/2006/relationships}'
BLOCK = 'µi 10 kHz 0,1 mT'
COLUMN_TO_GRADE = {'3F46 long': '3F46'}
NOT_A_GRADE = {'3F46 short', '3F46A short'}
SOURCE_NAME = 'Ferroxcube measurement workbook "FXC Materials Power.xlsx" (private communication)'


def column_index(letters):
    n = 0
    for ch in letters:
        n = n * 26 + ord(ch) - 64
    return n - 1


def read_sheet(path, name):
    with zipfile.ZipFile(path) as z:
        strings = []
        if 'xl/sharedStrings.xml' in z.namelist():
            for si in ET.fromstring(z.read('xl/sharedStrings.xml')).findall(M + 'si'):
                strings.append(''.join(t.text or '' for t in si.iter(M + 't')))
        workbook = ET.fromstring(z.read('xl/workbook.xml'))
        rels = {r.get('Id'): r.get('Target') for r in ET.fromstring(z.read('xl/_rels/workbook.xml.rels'))}
        target = None
        for sheet in workbook.find(M + 'sheets'):
            if sheet.get('name') == name:
                target = rels[sheet.get(R + 'id')].lstrip('/')
        if target is None:
            raise RuntimeError(f'{path}: no sheet named {name!r}')
        target = target if target.startswith('xl/') else 'xl/' + target
        rows = {}
        for cell in ET.fromstring(z.read(target)).iter(M + 'c'):
            ref = re.fullmatch(r'([A-Z]+)(\d+)', cell.get('r'))
            kind, v = cell.get('t'), cell.find(M + 'v')
            if v is None:
                continue
            if kind == 's':
                value = strings[int(v.text)]
            elif kind in ('str', 'e'):
                value = v.text
            else:
                value = float(v.text)
            rows.setdefault(int(ref.group(2)), {})[column_index(ref.group(1))] = value
        return rows


def measured_sweeps(path):
    rows = read_sheet(path, 'data')
    sample = rows[1][4]
    header = {c: v for c, v in rows[2].items() if c >= 4}
    starts = [r for r in sorted(rows) if rows[r].get(0) == BLOCK and isinstance(rows[r].get(3), float)]
    # the workbook has two blocks with this heading: a single 25 C row, and the temperature sweep
    sweeps = [r for r in starts if isinstance(rows.get(r + 1, {}).get(3), float)]
    if len(sweeps) != 1:
        raise RuntimeError(f'{path}: expected exactly one "{BLOCK}" temperature sweep, found {len(sweeps)}')
    out = {name: [] for name in header.values()}
    r = sweeps[0]
    while r in rows and isinstance(rows[r].get(3), float):
        temperature = rows[r][3]
        for c, name in header.items():
            if isinstance(rows[r].get(c), float):
                out[name].append((temperature, rows[r][c]))
        r += 1
    return sample, out


def sweep_points(record):
    """The points MKF's temperature sweep uses: lowest frequency, lowest DC bias."""
    pts = record['permeability']['initial']
    freqs = [p['frequency'] for p in pts if p.get('frequency') is not None]
    biases = [p['magneticFieldDcBias'] for p in pts if p.get('magneticFieldDcBias') is not None]
    fmin = min(freqs) if freqs else None
    bmin = min(biases) if biases else None
    return [p for p in pts if p.get('temperature') is not None
            and not (p.get('frequency') is not None and p['frequency'] > fmin)
            and not (p.get('magneticFieldDcBias') is not None and p['magneticFieldDcBias'] > bmin)]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('workbook')
    ap.add_argument('--patch')
    args = ap.parse_args()
    today = datetime.date.today().isoformat()
    sample, sweeps = measured_sweeps(args.workbook)
    db = args.patch or os.path.join(os.path.dirname(__file__), '..', 'data', 'core_materials.ndjson')
    lines = open(db, encoding='utf-8').read().split('\n')
    index = {json.loads(line)['name']: i for i, line in enumerate(lines) if line.strip()}
    changed = 0
    for column, measured in sweeps.items():
        if column in NOT_A_GRADE:
            print(f'{column:12s} not a MAS grade, skipped')
            continue
        grade = COLUMN_TO_GRADE.get(column, column)
        if grade not in index:
            print(f'{column:12s} no MAS record, skipped')
            continue
        line = lines[index[grade]]
        record = json.loads(line)
        ascii_form = json.dumps(record) == line
        if not ascii_form and json.dumps(record, ensure_ascii=False) != line:
            raise RuntimeError(f'{grade}: record does not round-trip through json; refusing to rewrite it')
        pts = sweep_points(record)
        if not pts:
            raise RuntimeError(f'{grade}: has no mu_i(T) sweep to replace')
        by_t = {}
        for p in pts:
            if p['temperature'] in by_t:
                raise RuntimeError(f'{grade}: two sweep points at {p["temperature"]} C')
            by_t[p['temperature']] = p
        keys = {k: pts[0][k] for k in ('frequency', 'magneticFieldDcBias') if k in pts[0]}
        worst, added = 0.0, 0
        for temperature, value in measured:
            if temperature in by_t:
                old = by_t[temperature]['value']
                worst = max(worst, abs(value / old - 1))
                by_t[temperature]['value'] = value
            else:
                point = {'value': value, 'temperature': temperature}
                point.update(keys)
                record['permeability']['initial'].append(point)
                added += 1
        if added:
            record['permeability']['initial'].sort(key=lambda p: (p.get('temperature') is None, p.get('temperature') or 0))
        lo, hi = measured[0][0], measured[-1][0]
        print(f'{grade:6s} <- {column!r}: {len(measured)} points {lo:.0f}..{hi:.0f} C, {added} added, '
              f'largest change of an existing value {worst * 100:.2f} %')
        record.setdefault('provenance', []).append({
            'source': 'manufacturerDatabase',
            'sourceName': SOURCE_NAME,
            'sourceUrl': None,
            'retrievedDate': today,
            'verification': 'valuesReadFromSource',
            'verificationDate': today,
            'verificationMethod': f'Points {lo:.0f}..{hi:.0f} C: initial permeability measured by Ferroxcube at '
                                  f'10 kHz, 0.1 mT, on toroid {sample}',
            'fields': ['permeability.initial'],
        })
        lines[index[grade]] = json.dumps(record, ensure_ascii=ascii_form)
        changed += 1
    if args.patch and changed:
        with open(db, 'w', encoding='utf-8') as fh:
            fh.write('\n'.join(lines))
        print(f'patched {changed} records in {db}')


if __name__ == '__main__':
    sys.exit(main())

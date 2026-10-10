#!/usr/bin/env python3
"""Set the Curie temperature of a few ferrites to where their data sheet's mu_i(T) figure collapses.
ABT #1739.

WHY
    These records store a Curie temperature BELOW the temperature at which the curve their own
    data sheet draws collapses (or, for R7K, above it).  The data sheets print the Curie
    temperature only as a lower bound (">= 270 C", "> 280 C", "> 125 C", "> 140 C"), so the
    stored numbers were those bounds, while the drawn curve shows the material still strongly
    magnetic above them.  A consumer that treats the material as paramagnetic above Tc (MKF
    returns mu_i = 1 there) would then discard the top of the drawn curve.

DEFINITION OF THE FIGURE'S COLLAPSE
    The same figure the mu_i(T) sweep was read from (scripts/digitize-ferroxcube-permeability-
    temperature.py, scripts/digitize-dmegc-permeability-temperature.py: vector paths, axes
    calibrated on the figure's gridlines) is read again, and the collapse is the first
    temperature after the curve's peak at which the curve comes within one stroke width of the
    mu = 0 axis -- the same "no longer resolvable from the axis" criterion those scripts use to
    end a sweep -- linearly interpolated between evaluated path points and rounded to 1 C.  Every
    stored sweep point therefore lies below the stored Curie temperature.

    If the drawn curve ends -- at the right edge of the plot, or inside it -- before it gets
    within one stroke width of the axis, the collapse is not drawn, and the figure gives no
    Curie temperature.
      * A Ferroxcube grade then keeps the Curie temperature printed in its data sheet's
        specification table ("TC >= 220 C"), read from the sheet's text together with the
        sheet's revision date, and cited as such.  (3C90, 3C94: the curve flattens into a tail
        that ends at 240 C still 1.08 stroke widths above the axis.  3F4: the curve ends at
        248.6 C, 1.07 stroke widths above it.)
      * A DMEGC grade gets the temperature of the curve's last drawn point, stated in the
        provenance as a lower bound read off the figure, with the permeability the curve still
        has there.  (DN85H: the plot ends at 180 C, where the curve is at mu ~ 211, 20 % of its
        peak.)
    A record whose stored mu_i(T) sweep runs above the Curie temperature it would get is REFUSED
    (nothing is written, the script exits non-zero naming the grade) unless that Curie temperature's
    provenance states it is a guaranteed minimum ("TC >= 220 C" in the sheet's table): then the
    measured sweep is the material above that minimum (3C90's sweep runs to 240 C), and a consumer
    treats the material as paramagnetic only above both the Curie temperature and the sweep.

    The Ferroxcube grades are all those of the mu_i(T) digitiser (the same 15 sheets); 3C92A's
    sheet draws exactly the 3C92 curve, so both get the same temperature.  A record that already
    carries this script's Curie temperature (same value, a curieTemperature provenance entry
    citing the same sheet) is left as it is, so the script can be re-run.

    Nothing is extrapolated or fitted.

Usage:
    python3 scripts/set-curie-from-permeability-figure.py --cache DIR [--patch data/core_materials.ndjson]
"""
import argparse
import datetime
import importlib.util
import json
import os
import re
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
FERROXCUBE = None  # every grade of the Ferroxcube digitiser, see main()
DMEGC = ['DMR90', 'R7K', 'DN85H']
# The phrase a curieTemperature provenance entry carries when the stored value is the data sheet's
# guaranteed minimum ("TC >= 220 C"), not the Curie point itself.
GUARANTEED_MINIMUM = 'a GUARANTEED MINIMUM, not the Curie point itself'


def load(name, file):
    spec = importlib.util.spec_from_file_location(name, os.path.join(HERE, file))
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def ferroxcube_table_curie(pdf):
    """(Curie temperature, title, revision) printed in a Ferroxcube material data sheet.

    The specification table prints the Curie temperature as "TC / >= 220 / deg C"; the first page
    carries the sheet's revision date ("2008 Sep 01") and the "Supersedes data of ..." line.
    """
    import fitz
    document = fitz.open(pdf)
    first = document[0].get_text()
    revision = re.search(r'\n((?:19|20)\d\d [A-Z][a-z]{2} \d\d)\n', first)
    supersedes = re.search(r'(Supersedes data of [^\n]+)', first)
    grade = re.search(r'FERROXCUBE\s*\n(\S+)\nMaterial specification', first)
    if not (revision and supersedes and grade):
        raise RuntimeError(f'{pdf}: first page does not carry the Ferroxcube grade / revision lines')
    found = []
    for page in document:
        found += re.findall(r'\nTC\n(\u2265|>|\u2248|=)?\s*(\d+)\n\u00b0C\n', page.get_text())
    if len(found) != 1:
        raise RuntimeError(f'{pdf}: expected one "TC" row in the specification table, found {found}')
    relation, value = found[0]
    title = f'Ferroxcube DATA SHEET "{grade.group(1)} Material specification"'
    return float(value), relation, title, f'{revision.group(1)} ({supersedes.group(1)})'


def collapse(t, mu, resolution):
    """(temperature, reached_axis, mu at that temperature)."""
    peak = int(np.argmax(mu))
    below = np.where((np.arange(len(t)) > peak) & (mu <= resolution))[0]
    if len(below):
        i = below[0]
        return float(np.interp(resolution, [mu[i], mu[i - 1]], [t[i], t[i - 1]])), True, float(resolution)
    return float(t[-1]), False, float(mu[-1])


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--cache', required=True)
    ap.add_argument('--patch')
    ap.add_argument('grades', nargs='*', help='limit to these grades (default: all)')
    args = ap.parse_args()
    os.makedirs(args.cache, exist_ok=True)
    fx = load('fx', 'digitize-ferroxcube-permeability-temperature.py')
    dm = load('dm', 'digitize-dmegc-permeability-temperature.py')
    today = datetime.date.today().isoformat()
    db = args.patch or os.path.join(HERE, '..', 'data', 'core_materials.ndjson')
    lines = open(db, encoding='utf-8').read().split('\n')
    index = {json.loads(line)['name']: i for i, line in enumerate(lines) if line.strip()}
    figures = []
    wanted = set(args.grades) if args.grades else None
    for grade in fx.SOURCES:
        if wanted is not None and grade not in wanted:
            continue
        url = fx.MDS + fx.SOURCES[grade]
        pdf = fx.fetch(url, os.path.join(args.cache, fx.SOURCES[grade].replace('%20', '_')))
        t, mu, resolution, page, fig = fx.read_figure(pdf)
        figures.append((grade, f'Ferroxcube {grade} material data sheet', url,
                        f'{fig} (permeability as a function of temperature), page {page}', t, mu, resolution,
                        pdf))
    for grade in DMEGC:
        if wanted is not None and grade not in wanted:
            continue
        url, page, drawing, colour, region, _ = dm.FIGURES[grade]
        pdf = dm.fetch(url, os.path.join(args.cache, grade + '.pdf'))
        t, mu, resolution, _ = dm.read_figure(pdf, page, drawing, colour, region)
        figures.append((grade, f'DMEGC {grade} material data sheet', url,
                        f'initial permeability versus temperature figure, page {page + 1}', t, mu, resolution,
                        None))
    changed = 0
    refused = []
    for grade, source_name, url, where, t, mu, resolution, ferroxcube_pdf in figures:
        temperature, reached, mu_there = collapse(t, mu, resolution)
        value = float(round(temperature))
        from_table = not reached and ferroxcube_pdf is not None
        if from_table:
            value, relation, title, revision = ferroxcube_table_curie(ferroxcube_pdf)
            source_name = f'{title}, {revision}'
            relation = {'\u2265': '>= ', '>': '> ', '\u2248': '~ ', '=': '', '': ''}[relation]
        line = lines[index[grade]]
        record = json.loads(line)
        ascii_form = json.dumps(record) == line
        if not ascii_form and json.dumps(record, ensure_ascii=False) != line:
            raise RuntimeError(f'{grade}: record does not round-trip through json; refusing to rewrite it')
        last = max(p['temperature'] for p in record['permeability']['initial'] if p.get('temperature') is not None)
        minimum = from_table and relation in ('>= ', '> ')
        if from_table:
            method = (f'Curie temperature printed in the specification table of the data sheet, page 2, as '
                      f'"TC {relation}{value:.0f} C"' +
                      (f': a GUARANTEED MINIMUM, not the Curie point itself. ' if minimum else '. ') +
                      f'{where} does not draw the collapse: the curve ends at {temperature:.1f} C still at '
                      f'mu {mu_there:.0f} ({mu_there / resolution:.2f} stroke widths above the mu = 0 axis)')
        elif reached:
            method = (f'Curie temperature {value:.0f} C read off {where}: the first temperature after the '
                      f'curve\'s peak at which the drawn curve reaches the mu = 0 axis (within one stroke '
                      f'width, mu {resolution:.0f}); the sheet itself states only a lower bound')
        else:
            method = (f'Curie temperature {value:.0f} C is a LOWER BOUND read off {where}: the drawn curve '
                      f'ends at {temperature:.1f} C still at mu {mu_there:.0f} ({mu_there / resolution:.2f} '
                      f'stroke widths above the mu = 0 axis), so the collapse is not drawn; the sheet '
                      f'itself states only a lower bound')
        print(f"{grade:6s} stored Tc {record.get('curieTemperature')} C -> {value:.0f} C "
              f"({'axis reached' if reached else ('data sheet table, the drawn curve ends at mu %.0f' if from_table else 'LOWER BOUND, the drawn curve ends at mu %.0f') % mu_there}), "
              f"last stored mu_i(T) point {last:.0f} C")
        entry = {
            'source': 'manufacturerDatasheet',
            'sourceName': source_name,
            'sourceUrl': url,
            'retrievedDate': today,
            'verification': 'valuesReadFromSource',
            'verificationDate': today,
            'verificationMethod': method,
            'fields': ['curieTemperature'],
        }
        stored = [p for p in record.get('provenance', [])
                  if p.get('fields') == ['curieTemperature'] and p.get('sourceUrl') == url]
        # A sweep above the Curie temperature is accepted only when the record's own provenance says
        # that temperature is a guaranteed minimum (then the measured mu_i(T) is the material above it).
        if last > value and not any(GUARANTEED_MINIMUM in p.get('verificationMethod', '') for p in (stored or [entry])):
            refused.append(f'{grade}: stored mu_i(T) sweep reaches {last:.0f} C, above the Curie temperature '
                           f'{value:.0f} C it would get ({"data sheet table" if from_table else "figure"}), '
                           f'which is not stated as a guaranteed minimum')
            continue
        if record.get('curieTemperature') == value and stored:
            print(f'{grade:6s} already {value:.0f} C from this sheet; left as it is')
            continue
        if args.patch:
            record['curieTemperature'] = value
            record.setdefault('provenance', []).append(entry)
            lines[index[grade]] = json.dumps(record, ensure_ascii=ascii_form)
            changed += 1
    if refused:
        for line in refused:
            print('REFUSED ' + line, file=sys.stderr)
        print('nothing written', file=sys.stderr)
        return 1
    if args.patch and changed:
        with open(db, 'w', encoding='utf-8') as fh:
            fh.write('\n'.join(lines))
        print(f'patched {changed} records in {db}')


if __name__ == '__main__':
    sys.exit(main())

#!/usr/bin/env python3
"""Replace the mu_i(T) sweeps of DMEGC materials whose stored sweep contradicts the data sheet the
record cites, with the curve read from that data sheet's VECTOR figure.

WHY
    The stored sweeps of these records are not the curve their own data sheet draws:
      DMR90  stored peak 4643 at 90 C and collapse at 255..260 C; the sheet draws a 3230 peak at
             95 C and the collapse at ~305 C (it states Tc > 280 C).
      R7K    stored -40..130 C with 15447 at 130 C; the sheet draws -30..125 C, 12660 at 120 C,
             collapsed by 130 C (it states Tc > 125 C).
      DN85H  stored -55..145 C, 694 at 25 C and collapsed by 145 C; the sheet draws 25..180 C,
             854 at 25 C (it states 850 +-25 %), still 1046 at 150 C.

WHAT IT READS
    Each sheet draws "initial permeability versus temperature" as a stroked vector path.  The
    path is evaluated exactly (lines and cubic Beziers).  Each axis is calibrated on the
    figure's own gridlines: every tick label is assigned the nearest gridline (within 3 pt) and
    the gridline-position -> value fit must hold to 0.5 pt, or the material is refused.  The
    drawing that holds the curve is named per material below and checked by its colour.

WHAT IT WRITES
    The sweep MKF evaluates (the temperature points at the lowest frequency and lowest DC bias)
    is replaced by points on a 5 C grid from the first to the last grid temperature the curve
    covers, each read off the curve (linear interpolation between evaluated path points).  A
    reading within one stroke width of mu = 0 is not resolvable and ends the sweep.  New points
    carry the frequency / DC-bias keys of the sweep they replace.  Points at other DC biases or
    frequencies are left untouched.  Nothing is extrapolated, fitted or scaled.  The Curie
    temperature is not touched: every one of these sheets states it only as a lower bound.

Usage:
    python3 scripts/digitize-dmegc-permeability-temperature.py --cache DIR [--patch data/core_materials.ndjson]
"""
import argparse
import datetime
import json
import os
import re
import sys
import urllib.request

import numpy as np
import fitz  # PyMuPDF

UA = ('Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) '
      'Chrome/151.0.0.0 Safari/537.36')
RED = (0.93, 0.11, 0.14)
# material -> (data sheet URL the record cites, page index, index of the curve drawing, colour,
#              label region of the figure (x0, y0, x1, y1), sheet's mu_i conditions)
FIGURES = {
    'DMR90': ('https://ferrite.ru/upload/docs/pdf/DMR90%20Material%20Datasheet.pdf', 0, 337, RED,
              (90, 570, 270, 700), 'mu_i stated at 10 kHz, B < 0.25 mT'),
    'R7K': ('https://irp.cdn-website.com/eb58f5fe/files/uploaded/47-r7k_high_ui_material_sl.pdf', 0, 159, RED,
            (100, 480, 250, 610), 'mu_i stated at 10 kHz, B < 0.25 mT'),
    'DN85H': ('https://irp.cdn-website.com/eb58f5fe/files/uploaded/DN85H_Material_Characteristics.pdf', 1, 72,
              (1.0, 0.0, 0.0), (60, 110, 290, 292), 'mu_i stated at 1 kHz, B < 0.25 mT'),
}
GRID_STEP = 5.0
SNAP = 3.0
MAX_RESIDUAL = 0.5


def fetch(url, path):
    if not os.path.exists(path):
        req = urllib.request.Request(url, headers={'User-Agent': UA})
        with urllib.request.urlopen(req, timeout=90) as fh:
            raw = fh.read()
        if not raw.startswith(b'%PDF'):
            raise RuntimeError(f'{url} did not return a PDF')
        with open(path, 'wb') as fh:
            fh.write(raw)
    return path


def number(s):
    s = s.replace('−', '-').replace('–', '-')
    return float(s) if re.fullmatch(r'-?\d+(\.\d+)?', s) else None


def path_points(drawing):
    out = []
    for it in drawing['items']:
        if it[0] == 'l':
            seg = np.array([[it[1].x, it[1].y], [it[2].x, it[2].y]])
        elif it[0] == 'c':
            t = np.linspace(0, 1, 200)[:, None]
            p = [np.array([q.x, q.y]) for q in it[1:5]]
            seg = (1 - t) ** 3 * p[0] + 3 * (1 - t) ** 2 * t * p[1] + 3 * (1 - t) * t * t * p[2] + t ** 3 * p[3]
        else:
            raise RuntimeError(f'unexpected path item {it[0]!r} in the curve')
        out.append(seg)
    return np.vstack(out)


def straight_lines(page):
    """Every straight stroke on the page, with a chain of collinear segments merged into one line."""
    lines = []
    for d in page.get_drawings():
        items = d['items']
        if items and all(it[0] == 'l' for it in items):
            xs = [q for it in items for q in (it[1].x, it[2].x)]
            ys = [q for it in items for q in (it[1].y, it[2].y)]
            if max(xs) - min(xs) < 0.05 or max(ys) - min(ys) < 0.05:
                lines.append((min(xs), min(ys), max(xs), max(ys)))
                continue
        for it in items:
            if it[0] == 'l':
                lines.append((min(it[1].x, it[2].x), min(it[1].y, it[2].y), max(it[1].x, it[2].x), max(it[1].y, it[2].y)))
    return lines


def axis_fit(positions, values, gridlines, name):
    pairs = []
    for p, v in zip(positions, values):
        g = min(gridlines, key=lambda q: abs(q - p)) if gridlines else None
        if g is None or abs(g - p) > SNAP:
            raise RuntimeError(f'{name}: tick label {v} has no gridline within {SNAP} pt')
        pairs.append((g, v))
    if len({g for g, _ in pairs}) != len(pairs):
        raise RuntimeError(f'{name}: two tick labels snapped to one gridline')
    g = np.array([q for q, _ in pairs])
    v = np.array([q for _, q in pairs])
    a = np.polyfit(g, v, 1)
    residual = float(np.max(np.abs(np.polyval(a, g) - v)) / abs(a[0]))
    if residual > MAX_RESIDUAL:
        raise RuntimeError(f'{name}: gridline fit residual {residual:.2f} pt > {MAX_RESIDUAL} pt')
    return a, residual


def read_figure(pdf, page_index, drawing_index, colour, region):
    page = fitz.open(pdf)[page_index]
    drawing = page.get_drawings()[drawing_index]
    if drawing.get('color') is None or max(abs(a - b) for a, b in zip(drawing['color'], colour)) > 0.02:
        raise RuntimeError(f'{pdf}: drawing {drawing_index} is not the {colour} curve')
    curve = drawing['rect']
    box = fitz.Rect(*region)
    words = [w for w in page.get_text('words') if box.contains(fitz.Rect(w[:4])) and number(w[4]) is not None]
    rows = {}
    for w in words:
        if w[1] >= curve.y1 - 2:
            rows.setdefault(round(w[3]), []).append(w)
    xrow = max(rows.values(), key=len)
    ycol = [w for w in words if w[2] <= curve.x0 + 2 and w not in xrow]
    xs = [(w[0] + w[2]) / 2 for w in xrow]
    ys = [(w[1] + w[3]) / 2 for w in ycol]
    frame = fitz.Rect(min(min(xs), curve.x0) - 14, min(min(ys), curve.y0) - 14,
                      max(max(xs), curve.x1) + 14, max(max(ys), curve.y1) + 14)
    vertical, horizontal = [], []
    for x0, y0, x1, y1 in straight_lines(page):
        if not (frame.x0 < x0 and x1 < frame.x1 and frame.y0 < y0 and y1 < frame.y1):
            continue
        if x1 - x0 < 0.05 and y1 - y0 > 0.5 * (max(ys) - min(ys)):
            vertical.append(x0)
        if y1 - y0 < 0.05 and x1 - x0 > 0.5 * (max(xs) - min(xs)):
            horizontal.append(y0)
    ax, rx = axis_fit(xs, [number(w[4]) for w in xrow], vertical, 'temperature axis')
    ay, ry = axis_fit(ys, [number(w[4]) for w in ycol], horizontal, 'permeability axis')
    pts = path_points(drawing)
    t = np.polyval(ax, pts[:, 0])
    mu = np.polyval(ay, pts[:, 1])
    if t[0] > t[-1]:
        t, mu = t[::-1], mu[::-1]
    return np.maximum.accumulate(t), mu, abs(ay[0]) * drawing['width'], (rx, ry)


def sweep(record):
    """The temperature sweep MKF evaluates: points at the lowest frequency and lowest DC bias.  A
    record that also stores mu(H) curves at a few temperatures has their H = 0 points in that set
    too; the sweep proper is the run of consecutive entries with strictly rising temperature, and
    only that run is returned (the H = 0 points of the mu(H) curves stay where they are)."""
    pts = record['permeability']['initial']
    freqs = [p['frequency'] for p in pts if p.get('frequency') is not None]
    biases = [p['magneticFieldDcBias'] for p in pts if p.get('magneticFieldDcBias') is not None]
    fmin = min(freqs) if freqs else None
    bmin = min(biases) if biases else None
    member = [p.get('temperature') is not None
              and not (p.get('frequency') is not None and p['frequency'] > fmin)
              and not (p.get('magneticFieldDcBias') is not None and p['magneticFieldDcBias'] > bmin) for p in pts]
    runs, current = [], []
    for i, p in enumerate(pts):
        if member[i] and (not current or p['temperature'] > pts[current[-1]]['temperature']):
            current.append(i)
        else:
            if current:
                runs.append(current)
            current = [i] if member[i] else []
    if current:
        runs.append(current)
    best = max(runs, key=len)
    if sum(member) != len(best):
        print(f"  {record['name']}: {sum(member) - len(best)} lowest-bias points outside the sweep run are kept")
    return [pts[i] for i in best]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--cache', required=True)
    ap.add_argument('--patch')
    args = ap.parse_args()
    os.makedirs(args.cache, exist_ok=True)
    today = datetime.date.today().isoformat()
    db = args.patch or os.path.join(os.path.dirname(__file__), '..', 'data', 'core_materials.ndjson')
    lines = open(db, encoding='utf-8').read().split('\n')
    index = {json.loads(line)['name']: i for i, line in enumerate(lines) if line.strip()}
    changed = 0
    for name, (url, page, drawing, colour, region, conditions) in FIGURES.items():
        pdf = fetch(url, os.path.join(args.cache, name + '.pdf'))
        t, mu, resolution, (rx, ry) = read_figure(pdf, page, drawing, colour, region)
        line = lines[index[name]]
        record = json.loads(line)
        ascii_form = json.dumps(record) == line
        if not ascii_form and json.dumps(record, ensure_ascii=False) != line:
            raise RuntimeError(f'{name}: record does not round-trip through json; refusing to rewrite it')
        old = sweep(record)
        keys = {k: old[0][k] for k in ('frequency', 'magneticFieldDcBias') if k in old[0]}
        new = []
        temp = np.ceil(t[0] / GRID_STEP - 1e-9) * GRID_STEP
        while temp <= t[-1] + 1e-9:
            value = float(np.interp(temp, t, mu))
            if value < resolution:
                break
            point = {'value': float(round(value)), 'temperature': float(temp)}
            point.update(keys)
            new.append(point)
            temp += GRID_STEP
        olds = sorted(old, key=lambda p: p['temperature'])
        print(f"{name:6s} stored {olds[0]['temperature']:.1f}..{olds[-1]['temperature']:.1f} C ({len(old)} points) -> "
              f"figure {new[0]['temperature']:.0f}..{new[-1]['temperature']:.0f} C ({len(new)} points), "
              f"axis fit residual {rx:.2f} / {ry:.2f} pt, mu resolution {resolution:.0f}, Curie {record.get('curieTemperature')} C")
        if args.patch:
            drop = {id(p) for p in old}
            record['permeability']['initial'] = new + [p for p in record['permeability']['initial'] if id(p) not in drop]
            record.setdefault('provenance', []).append({
                'source': 'manufacturerDatasheet',
                'sourceName': f'DMEGC {name} material data sheet',
                'sourceUrl': url,
                'retrievedDate': today,
                'verification': 'valuesReadFromSource',
                'verificationDate': today,
                'verificationMethod': f"Points {new[0]['temperature']:.0f}..{new[-1]['temperature']:.0f} C: initial "
                                      f"permeability versus temperature figure, page {page + 1}; {conditions}",
                'fields': ['permeability.initial'],
            })
            lines[index[name]] = json.dumps(record, ensure_ascii=ascii_form)
            changed += 1
    if args.patch and changed:
        with open(db, 'w', encoding='utf-8') as fh:
            fh.write('\n'.join(lines))
        print(f'patched {changed} records in {db}')


if __name__ == '__main__':
    sys.exit(main())

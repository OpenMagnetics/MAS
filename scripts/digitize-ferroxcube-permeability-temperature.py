#!/usr/bin/env python3
"""Extend the mu_i(T) sweeps of Ferroxcube ferrites up to their Curie collapse, read from the
VECTOR curve of each grade's own material data sheet.  ABT #1739.

WHY
    Seventeen Ferroxcube mu_i(T) tables in core_materials.ndjson stop at 180 C (-40..180 C in
    10 C steps, at 10 kHz), although every grade's data sheet draws "Initial permeability as a
    function of temperature" up to and through its Curie collapse (230..290 C).  Above the last
    tabulated point a consumer has no data: MKF used to extrapolate its spline there.  This
    script appends the part of the published curve that the tables lack.

WHAT IT READS
    Ferroxcube's MDS PDFs draw that figure as vector paths, so nothing here is traced from
    pixels: the curve is the stroked path itself (lines and cubic Beziers, evaluated exactly),
    and the axes are calibrated on the figure's own GRIDLINES -- every gridline is assigned the
    nearest multiple of the tick-label step and the position->value fit must hold to 0.25 pt on
    both axes, or the grade is refused.  Tick labels only seed that assignment.  The part of a
    path lying outside the plot box is clipped away on the page and is dropped here too.

WHAT IT WRITES
    For a grade it accepts, points on a 5 C grid strictly above the last tabulated temperature,
    up to the end of the drawn curve, each read off the curve (linear interpolation between the
    evaluated path points).  A point whose reading lies within one stroke width of the
    mu = 0 axis is not resolvable from the axis line and is not written; the sweep stops there.
    Nothing is extrapolated, nothing fitted, nothing scaled.  New points carry the same
    frequency as the tabulated temperature points (the data sheets state mu_i at <= 10 kHz).
    One provenance entry per grade names the document and figure.

THE JOIN WITH THE MEASURED SWEEP
    The -40..180 C points are Ferroxcube's own measurements (scripts/import-ferroxcube-measured-
    permeability-temperature.py), which override the data sheet wherever they exist.  The data
    sheet curve is therefore used only above the last measured temperature, and only where it
    meets the measurement: the two sources are different samples, and a grade whose figure
    differs from the measurement by more than SEAM_TOLERANCE at the last measured temperature
    is REFUSED -- appending it would put a step into the sweep, and its table stays at the
    measured range.  Nothing is scaled to make the two meet.  The figure-vs-measurement
    difference at the join is printed for every grade.
    With the 2026-10-10 data: 3C92A -8.4 %, 3C94 +14.4 %, 3C95 -20.1 %, 3C98 +9.1 %, 3F4 -10.8 %
    are refused (they stop at 180 C); the other ten join within 5 % (largest: 3C97 -4.6 %,
    3C92 -4.2 %).

Usage:
    python3 scripts/digitize-ferroxcube-permeability-temperature.py --cache DIR [--patch data/core_materials.ndjson] [GRADE ...]
"""
import argparse
import collections
import datetime
import json
import os
import re
import sys
import urllib.request

import numpy as np
import fitz  # PyMuPDF

MDS = 'https://www.ferroxcube.com/upload/media/product/file/MDS/'
# grade -> data-sheet file name on Ferroxcube's site (the URL the MAS record already cites)
SOURCES = {
    '3C90': '3c90.pdf', '3C91': '3c91.pdf', '3C92': '3C92%202022.pdf', '3C92A': '3c92a.pdf',
    '3C94': '3c94.pdf', '3C95': '3c95.pdf', '3C95A': '3c95a.pdf', '3C95F': '3c95f.pdf',
    '3C96': '3c96.pdf', '3C97': '3c97.pdf', '3C98': '3c98.pdf', '3F36': '3f36.pdf',
    '3F37': '2025%20MDS%203F37.pdf', '3F4': '3f4.pdf', '3F46': '3f46.pdf',
}
# the mu_i measurement conditions each sheet prints in its specification table
OLD, NEW = 'mu_i stated at <=10 kHz, 0.25 mT', 'mu_i stated at 10 kHz, 0.25 mT'
CONDITIONS = {'3C90': OLD, '3C91': OLD, '3C92': NEW, '3C92A': NEW, '3C94': OLD, '3C95': OLD, '3C95A': NEW,
              '3C95F': NEW, '3C96': OLD, '3C97': NEW, '3C98': OLD, '3F36': OLD, '3F37': NEW, '3F4': OLD,
              '3F46': NEW}
SEAM_TOLERANCE = 0.05
GRID_STEP = 5.0
UA = ('Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) '
      'Chrome/151.0.0.0 Safari/537.36')


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
    s = s.strip().replace('−', '-').replace('–', '-')
    return float(s) if re.fullmatch(r'-?\d+(\.\d+)?', s) else None


def pieces(drawing):
    out = []
    for it in drawing['items']:
        if it[0] == 'l':
            out.append(np.array([[it[1].x, it[1].y], [it[2].x, it[2].y]]))
        elif it[0] == 'c':
            t = np.linspace(0, 1, 80)[:, None]
            p = [np.array([q.x, q.y]) for q in it[1:5]]
            out.append((1 - t) ** 3 * p[0] + 3 * (1 - t) ** 2 * t * p[1] + 3 * (1 - t) * t * t * p[2] + t ** 3 * p[3])
        elif it[0] == 're':
            r = it[1]
            out += [np.array([[r.x0, r.y0], [r.x1, r.y0]]), np.array([[r.x0, r.y1], [r.x1, r.y1]]),
                    np.array([[r.x0, r.y0], [r.x0, r.y1]]), np.array([[r.x1, r.y0], [r.x1, r.y1]])]
    return out


def figure_captions(page):
    """(Fig word, 'temperature' word) of every 'permeability ... function of temperature' caption."""
    words = page.get_text('words')
    out = []
    for i, w in enumerate(words):
        if w[4].lower().startswith('temperature'):
            back = words[max(0, i - 12):i]
            text = ' '.join(x[4] for x in back)
            if 'ermeab' in text and 'oss' not in text.split('ermeab')[-1]:
                fig = [x for x in back if x[4].startswith('Fig')]
                if fig:
                    out.append((fig[-1], w))
    return out


def label_fit(pos, val):
    """Tick labels sorted by position must increase; a label whose minus sign is drawn as a path
    (not text) reads positive and is restored here."""
    order = np.argsort(pos)
    pos = np.array(pos)[order]
    val = np.array(val, float)[order]
    for i in range(len(val) - 2, -1, -1):
        if val[i] >= val[i + 1] and val[i] > 0:
            val[i] = -val[i]
    return np.polyfit(pos, val, 1), val


def grid_fit(lines, seed, label_step):
    lines = sorted(set(round(g, 2) for g in lines))
    if len(lines) < 3:
        raise RuntimeError('fewer than 3 gridlines')
    v = np.polyval(seed, lines)
    for k in (1, 2, 4, 5, 10):
        step = label_step / k
        snapped = np.round(v / step) * step
        if np.max(np.abs(v - snapped)) < 0.3 * step:
            break
    else:
        raise RuntimeError('gridlines do not sit on multiples of the label step')
    lines = np.array(lines)
    fit = np.polyfit(lines, snapped, 1)
    err = np.abs(np.polyval(fit, lines) - snapped) / abs(fit[0])
    if np.max(err) > 0.25 and len(lines) >= 6:
        # one misplaced gridline is tolerated (3F36 draws its 2000 line 1.5 pt off the other
        # ten, which are evenly spaced and agree with the tick labels); it is left out of the fit
        keep = np.arange(len(lines)) != np.argmax(err)
        fit = np.polyfit(lines[keep], snapped[keep], 1)
        err = np.abs(np.polyval(fit, lines[keep]) - snapped[keep]) / abs(fit[0])
    return fit, float(np.max(err))


def read_figure(pdf):
    """Return (T, mu, mu_resolution, page_number, figure_label) for the grade's mu_i(T) figure."""
    found = []
    for pno, page in enumerate(fitz.open(pdf)):
        drawings = page.get_drawings()
        words = page.get_text('words')
        for fig, tw in figure_captions(page):
            cap_x = (fig[0] + tw[2]) / 2
            cap_y = fig[1]
            vertical = []
            for d in drawings:
                for s in pieces(d):
                    if len(s) == 2 and abs(s[0, 0] - s[1, 0]) < 0.3 and abs(s[1, 1] - s[0, 1]) > 80 \
                            and max(s[:, 1]) < cap_y - 2 and min(s[:, 1]) > cap_y - 360:
                        vertical.append((s[0, 0], min(s[:, 1]), max(s[:, 1])))
            by_bottom = collections.defaultdict(list)
            for v in vertical:
                by_bottom[round(v[2])].append(v)
            clusters = []
            for vs in by_bottom.values():
                vs = sorted(vs)
                cur = [vs[0]]
                for v in vs[1:]:
                    if v[0] - cur[-1][0] > 60:
                        clusters.append(cur)
                        cur = [v]
                    else:
                        cur.append(v)
                clusters.append(cur)
            best = None
            for vs in clusters:
                x0, x1 = min(v[0] for v in vs), max(v[0] for v in vs)
                yt, yb = min(v[1] for v in vs), max(v[2] for v in vs)
                if x1 - x0 < 80 or not (x0 - 30 < cap_x < x1 + 30):
                    continue
                xl = [w for w in words if yb - 1 < w[1] < yb + 16 and x0 - 14 < (w[0] + w[2]) / 2 < x1 + 14
                      and number(w[4]) is not None]
                if len(xl) < 3:
                    continue
                row = collections.Counter(round(w[3]) for w in xl).most_common(1)[0][0]
                xl = [w for w in xl if abs(w[3] - row) < 1.5]
                yl = [w for w in words if x0 - 40 < w[2] < x0 + 1 and yt - 6 < (w[1] + w[3]) / 2 < yb + 6
                      and number(w[4]) is not None]
                if len(xl) >= 3 and len(yl) >= 3:
                    dist = abs((x0 + x1) / 2 - cap_x)
                    if best is None or dist < best[0]:
                        best = (dist, x0, x1, yt, yb, xl, yl)
            if best is None:
                continue
            _, x0, x1, yt, yb, xl, yl = best
            ax, xv = label_fit([(w[0] + w[2]) / 2 for w in xl], [number(w[4]) for w in xl])
            ay, yv = label_fit([-(w[1] + w[3]) / 2 for w in yl], [number(w[4]) for w in yl])
            ay = np.array([-ay[0], ay[1]])
            gv, gh = [], []
            for d in drawings:
                for s in pieces(d):
                    if len(s) != 2:
                        continue
                    (a1, b1), (a2, b2) = s
                    if abs(a1 - a2) < 0.05 and abs(b2 - b1) > 0.9 * (yb - yt) and x0 - 0.6 < a1 < x1 + 0.6 \
                            and min(b1, b2) > yt - 0.8 and max(b1, b2) < yb + 0.8:
                        gv.append(a1)
                    if abs(b1 - b2) < 0.05 and abs(a2 - a1) > 0.9 * (x1 - x0) and yt - 0.6 < b1 < yb + 0.6 \
                            and min(a1, a2) > x0 - 0.8 and max(a1, a2) < x1 + 0.8:
                        gh.append(b1)
            ax, rx = grid_fit(gv, ax, np.median(np.diff(sorted(xv))))
            ay, ry = grid_fit(gh, ay, np.median(np.diff(sorted(yv))))
            if rx > 0.25 or ry > 0.25:
                raise RuntimeError(f'{pdf}: gridline calibration residual {rx:.2f} / {ry:.2f} pt')
            # the curve: the stroke style with the longest non-axis-aligned path inside the box
            styles = collections.defaultdict(list)
            for d in drawings:
                r = d['rect']
                if r.x1 < x0 or r.x0 > x1 or r.y1 < yt or r.y0 > yb or d['type'] != 's':
                    continue
                ps = pieces(d)
                if not ps:
                    continue
                allp = np.vstack(ps)
                inside = (allp[:, 0] >= x0 - 0.5) & (allp[:, 0] <= x1 + 0.5) & (allp[:, 1] >= yt - 0.5) & (allp[:, 1] <= yb + 0.5)
                if inside.mean() < 0.5:
                    continue
                for s in ps:
                    k = (s[:, 0] >= x0 - 0.5) & (s[:, 0] <= x1 + 0.5) & (s[:, 1] >= yt - 0.5) & (s[:, 1] <= yb + 0.5)
                    if k.sum() < 2:
                        continue
                    s = s[k]
                    horizontal = np.ptp(s[:, 1]) < 0.05
                    vert = np.ptp(s[:, 0]) < 0.05
                    if horizontal and (abs(s[0, 1] - yb) < 0.6 or abs(s[0, 1] - yt) < 0.6 or np.ptp(s[:, 0]) > 0.95 * (x1 - x0)):
                        continue
                    if vert and (abs(s[0, 0] - x0) < 0.6 or abs(s[0, 0] - x1) < 0.6 or np.ptp(s[:, 1]) > 0.95 * (yb - yt)):
                        continue
                    styles[(round(d.get('width') or 0, 2), d.get('color'))].append(s)

            def oblique_length(ss):
                total = 0.0
                for s in ss:
                    dd = np.diff(s, axis=0)
                    m = (np.abs(dd[:, 0]) > 0.05) & (np.abs(dd[:, 1]) > 0.05)
                    total += float(np.sum(np.hypot(dd[m, 0], dd[m, 1])))
                return total
            ranked = sorted(styles.items(), key=lambda kv: -oblique_length(kv[1]))
            if not ranked or oblique_length(ranked[0][1]) == 0:
                raise RuntimeError(f'{pdf}: no curve inside the plot box')
            if len(ranked) > 1 and oblique_length(ranked[1][1]) > 0.2 * oblique_length(ranked[0][1]):
                raise RuntimeError(f'{pdf}: more than one curve-like stroke style in the plot box')
            (width, _), segs = ranked[0]
            chain = [segs[0]]
            for s in segs[1:]:
                e = chain[-1][-1]
                if np.hypot(*(s[-1] - e)) < np.hypot(*(s[0] - e)):
                    s = s[::-1]
                if np.hypot(*(s[0] - e)) > 1.0:
                    raise RuntimeError(f'{pdf}: curve pieces do not join ({np.hypot(*(s[0] - e)):.2f} pt gap)')
                chain.append(s)
            p = np.vstack(chain)
            if p[-1, 0] < p[0, 0]:
                p = p[::-1]
            t = np.polyval(ax, p[:, 0])
            mu = np.polyval(ay, p[:, 1])
            if np.min(np.diff(t)) < -0.2:
                raise RuntimeError(f'{pdf}: curve runs backwards in T by {-np.min(np.diff(t)):.2f} C')
            resolution = abs(ay[0]) * width
            found.append((np.maximum.accumulate(t), mu, resolution, pno + 1, fig[4]))
    if len(found) != 1:
        raise RuntimeError(f'{pdf}: found {len(found)} mu_i(T) figures, expected exactly one')
    return found[0]


def temperature_points(record):
    """The points MKF's temperature sweep uses: lowest frequency, lowest DC bias."""
    pts = record['permeability']['initial']
    freqs = [p['frequency'] for p in pts if p.get('frequency') is not None]
    biases = [p['magneticFieldDcBias'] for p in pts if p.get('magneticFieldDcBias') is not None]
    fmin = min(freqs) if freqs else None
    bmin = min(biases) if biases else None
    return [p for p in pts if p.get('temperature') is not None
            and not (p.get('frequency') is not None and p['frequency'] > fmin)
            and not (p.get('magneticFieldDcBias') is not None and p['magneticFieldDcBias'] > bmin)]


def extension(record, curve):
    t, mu, resolution, page, fig = curve
    tab = sorted(temperature_points(record), key=lambda p: p['temperature'])
    last = tab[-1]
    seam = float(np.interp(last['temperature'], t, mu)) / last['value'] - 1
    if abs(seam) > SEAM_TOLERANCE:
        return None, seam
    keys = {k: last[k] for k in ('frequency', 'magneticFieldDcBias', 'magneticFluxDensityPeak') if k in last}
    new = []
    temp = (np.floor(last['temperature'] / GRID_STEP) + 1) * GRID_STEP
    while temp <= t[-1]:
        value = float(np.interp(temp, t, mu))
        if value < resolution:
            break
        point = {'value': float(round(value)), 'temperature': float(temp)}
        point.update(keys)
        new.append(point)
        temp += GRID_STEP
    return new, seam


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--cache', required=True)
    ap.add_argument('--patch')
    ap.add_argument('grades', nargs='*')
    args = ap.parse_args()
    os.makedirs(args.cache, exist_ok=True)
    grades = args.grades or list(SOURCES)
    today = datetime.date.today().isoformat()
    db = args.patch or os.path.join(os.path.dirname(__file__), '..', 'data', 'core_materials.ndjson')
    lines = open(db, encoding='utf-8').read().split('\n')
    index = {}
    for i, line in enumerate(lines):
        if line.strip():
            index[json.loads(line)['name']] = i
    changed = 0
    for grade in grades:
        url = MDS + SOURCES[grade]
        pdf = fetch(url, os.path.join(args.cache, SOURCES[grade].replace('%20', '_')))
        curve = read_figure(pdf)
        line = lines[index[grade]]
        record = json.loads(line)
        ascii_form = json.dumps(record) == line
        if not ascii_form and json.dumps(record, ensure_ascii=False) != line:
            raise RuntimeError(f'{grade}: record does not round-trip through json; refusing to rewrite it')
        tab = sorted(temperature_points(record), key=lambda p: p['temperature'])
        new, seam = extension(record, curve)
        head = f"{grade:6s} measured {tab[0]['temperature']:.0f}..{tab[-1]['temperature']:.0f} C, figure to {curve[0][-1]:.1f} C, " \
               f"Curie {record.get('curieTemperature')} C, figure vs measurement at {tab[-1]['temperature']:.0f} C: {seam * 100:+.1f} %"
        if new is None:
            print(head + '  -> REFUSED (figure does not meet the measurement)')
            continue
        if not new:
            print(head + '  -> nothing above the table')
            continue
        print(head + f"  -> +{len(new)} points {new[0]['temperature']:.0f}..{new[-1]['temperature']:.0f} C")
        if args.patch:
            record['permeability']['initial'].extend(new)
            record.setdefault('provenance', []).append({
                'source': 'manufacturerDatasheet',
                'sourceName': f'Ferroxcube {grade} material data sheet',
                'sourceUrl': url,
                'retrievedDate': today,
                'verification': 'valuesReadFromSource',
                'verificationDate': today,
                'verificationMethod': f"Points {new[0]['temperature']:.0f}..{new[-1]['temperature']:.0f} C: {curve[4]} "
                                      f"(permeability as a function of temperature), page {curve[3]}; {CONDITIONS[grade]}",
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

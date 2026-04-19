#!/usr/bin/env python3
"""
Populate the `application` field for all core materials in core_materials.ndjson.
Also fixes known broken/missing datasheetUrl entries.

Run from repo root:
    python3 MAS/scripts/populate_application.py
"""

import json
import os

DB_PATH = os.path.join(os.path.dirname(__file__), "..", "data", "core_materials.ndjson")

# ── Application lookup tables by (manufacturer, material_name) ────────────────
# Values: "Power" | "Signal Processing" | "Interference Suppression"

TDK_APP = {
    "C350":  "Interference Suppression",  # NiZn ferrite polymer composite, EMI
    "C351":  "Interference Suppression",
    "K1":    "Signal Processing",         # NiZn, filter coils/CAN-bus chokes
    "K10":   "Signal Processing",         # NiZn, LAN/data-line chokes
    "M33":   "Signal Processing",         # MnZn, broadband filter above 1 MHz
    "N22":   "Power",
    "N27":   "Power",
    "N30":   "Interference Suppression",  # MnZn µi=4300, common-mode chokes
    "N41":   "Power",
    "N45":   "Signal Processing",         # Stable µ over temp, sensor coils
    "N48":   "Signal Processing",
    "N49":   "Power",
    "N51":   "Power",
    "N72":   "Signal Processing",         # Low remanence, signal transformers
    "N87":   "Power",
    "N88":   "Power",
    "N92":   "Power",
    "N95":   "Power",
    "N96":   "Power",
    "N97":   "Power",
    "PC47":  "Power",
    "PC95":  "Power",
    "PC200": "Power",
    "T35":   "Power",                     # NiZn, power-line chokes / SMPS
    "T36":   "Signal Processing",         # NiZn, current-compensated chokes
    "T37":   "Signal Processing",
    "T38":   "Interference Suppression",  # NiZn, EMI filters / common-mode
    "T46":   "Power",                     # NiZn, power-line chokes
    "T57":   "Signal Processing",         # VDSL/broadband
    "T65":   "Power",                     # NiZn, high-current power-line chokes
    "T66":   "Signal Processing",         # NiZn µi=13000, filter applications
}

FERROXCUBE_APP = {
    "3C90":  "Power",
    "3C91":  "Power",
    "3C92":  "Power",
    "3C92A": "Power",
    "3C94":  "Power",
    "3C95":  "Power",
    "3C95A": "Power",
    "3C95F": "Power",
    "3C96":  "Power",
    "3C97":  "Power",
    "3C98":  "Power",
    "3C99":  "Power",
    "3F36":  "Power",
    "3F4":   "Power",
    "3F46":  "Power",
    "4F1":   "Power",
    "3F3":   "Power",
    "3E6":   "Signal Processing",         # High-µ MnZn, broadband transformers
}

FAIR_RITE_APP = {
    "61": "Signal Processing",         # NiZn, high-Q broadband (RF inductors)
    "67": "Power",                     # NiZn, high-freq power (>4 MHz SMPS)
    "77": "Power",                     # MnZn, Power Conversion
    "78": "Power",
    "79": "Power",
    "80": "Power",
    "95": "Power",
    "97": "Power",
    "98": "Power",
    "15": "Signal Processing",         # MnZn µi=2000, broadband transformers
    "20": "Signal Processing",         # MnZn µi=1500, broadband transformers
    "31": "Interference Suppression",
    "43": "Interference Suppression",  # NiZn EMI
    "44": "Interference Suppression",
    "46": "Interference Suppression",
    "51": "Signal Processing",         # NiZn, broadband RF
    "52": "Signal Processing",
    "68": "Signal Processing",         # MnZn, broadband transformers
    "73": "Interference Suppression",
    "75": "Interference Suppression",  # MnZn, power-line EMI filters
    "76": "Interference Suppression",
    "96": "Power",                     # MnZn, high-temp power
}

# ACME (all MnZn ferrites, classified by family)
ACME_APP = {}
# P-series: Power
for g in ["P4", "P41", "P42", "P45", "P47", "P48", "P49", "P491", "P492",
          "P5", "P51", "P52", "P53", "P61", "P63", "P451", "P452"]:
    ACME_APP[g] = "Power"
# D-series: Power
for g in ["D1C", "D25", "D28", "D30", "D35", "D37", "D40", "ACME D27"]:
    ACME_APP[g] = "Power"
# N-series: Power
for g in ["N4", "N5", "N07", "N10", "N42", "N43"]:
    ACME_APP[g] = "Power"
# A-series: Signal Processing (high-permeability MnZn for broadband)
for g in ["A043", "A044", "A05", "A06", "A061", "A062", "A064",
          "A07", "A071", "A072", "A10", "A102", "A103", "A104",
          "A121", "A13", "A151"]:
    ACME_APP[g] = "Signal Processing"
# K-series: Signal Processing (medium-permeability MnZn for broadband)
for g in ["K081", "K12", "K13", "K15", "K151", "K20", "K25", "ACME K10"]:
    ACME_APP[g] = "Signal Processing"

# DMEGC
DMEGC_APP = {}
# DMR-series (MnZn): Power
for g in ["DMR24", "DMR25", "DMR28", "DMR40", "DMR40B", "DMR44", "DMR47",
          "DMR50", "DMR50B", "DMR51", "DMR51W", "DMR52", "DMR52W", "DMR53",
          "DMR55", "DMR70", "DMR71", "DMR73", "DMR90", "DMR91", "DMR95",
          "DMR95B", "DMR96", "DMR96A"]:
    DMEGC_APP[g] = "Power"
# DN-series (NiZn): Interference Suppression
for g in ["DN2S", "DN15P", "DN20F", "DN30B", "DN33L", "DN40B", "DN50B",
          "DN85H", "DN100H", "DN150H", "DN200L"]:
    DMEGC_APP[g] = "Interference Suppression"
# R-series (very-high-µ MnZn): Interference Suppression (common-mode chokes)
for g in ["R5K", "R5KC", "R5KZ", "R7K", "R7KC", "R10K", "R10KC", "R10KZ",
          "R12K", "R12KZ", "R15K", "R15KZ"]:
    DMEGC_APP[g] = "Interference Suppression"

# Magnetics — all powder cores → Power
MAGNETICS_APP = {n: "Power" for n in [
    "Kool Mµ 14", "Kool Mµ 26", "Kool Mµ 40", "Kool Mµ 60", "Kool Mµ 75",
    "Kool Mµ 90", "Kool Mµ 125",
    "Kool Mµ MAX 14", "Kool Mµ MAX 19", "Kool Mµ MAX 26", "Kool Mµ MAX 40",
    "Kool Mµ MAX 60", "Kool Mµ MAX 75", "Kool Mµ MAX 90",
    "Kool Mµ Hƒ 26", "Kool Mµ Hƒ 40", "Kool Mµ Hƒ 60", "Kool Mµ Hƒ 75",
    "Kool Mµ Hƒ 125",
    "Kool Mµ Ultra 26", "Kool Mµ Ultra 40", "Kool Mµ Ultra 60",
    "75-Series 26", "75-Series 40", "75-Series 60",
    "XFlux 19", "XFlux 26", "XFlux 40", "XFlux 60", "XFlux 75", "XFlux 90",
    "XFlux 125",
    "High DC Bias XFlux 26", "High DC Bias XFlux 40", "High DC Bias XFlux 60",
    "XFlux Ultra 26", "XFlux Ultra 60",
    "High Flux 14", "High Flux 26", "High Flux 40", "High Flux 60",
    "High Flux 75", "High Flux 125", "High Flux 147", "High Flux 160",
    "Edge 14", "Edge 19", "Edge 26", "Edge 40", "Edge 60", "Edge 75",
    "Edge 90", "Edge 125",
    "High DC Bias Edge 26", "High DC Bias Edge 60",
    "MPP 14", "MPP 19", "MPP 26", "MPP 40", "MPP 60", "MPP 75", "MPP 90",
    "MPP 125", "MPP 147", "MPP 160", "MPP 173", "MPP 200", "MPP 300",
    "MPP 550",
]}

# Micrometals — all powder cores → Power
MICROMETALS_APP = {n: "Power" for n in [
    "Mix 1", "Mix 2", "Mix 3", "Mix 5", "Mix 6", "Mix 7", "Mix 8",
    "Mix 10", "Mix 12", "Mix 14", "Mix 15", "Mix 17", "Mix 18", "Mix 19",
    "Mix 26", "Mix 30", "Mix 34", "Mix 35", "Mix 38", "Mix 40", "Mix 45",
    "Mix 52", "Mix 60", "Mix 61", "Mix 63", "Mix 65", "Mix 66", "Mix 70",
    "Mix 125",
    "FS 14", "FS 26", "FS 40", "FS 60", "FS 75", "FS 90",
    "GX 60", "GX 125",
    "HF 14", "HF 26", "HF 40", "HF 60", "HF 75", "HF 125", "HF 147",
    "HF 160",
    "MP 14", "MP 26", "MP 60", "MP 125", "MP 147", "MP 160", "MP 173",
    "MP 205",
    "MS 14", "MS 26", "MS 40", "MS 60", "MS 75", "MS 90", "MS 125",
    "MS 147", "MS 160",
    "OC 26", "OC 40", "OC 60", "OC 90", "OC 125",
    "OD 26", "OD 40", "OD 60", "OD 90",
    "OE 26", "OE 40", "OE 60", "OE 75", "OE 90",
    "OP 14", "OP 26", "OP 40", "OP 60", "OP 75", "OP 90", "OP 125",
    "SH 10", "SH 26", "SH 60", "SH 125",
    "SM 26", "SM 40", "SM 60",
    "SP 26", "SP 60", "SP 90",
]}

SINOMAG_APP = {
    "SMP44": "Interference Suppression",
    "SMP47": "Interference Suppression",
    "SMP50": "Signal Processing",
    "SMP51": "Signal Processing",
    "SMP53": "Signal Processing",
    "SMP95": "Power",
    "SMP96": "Power",
    "SMP97": "Power",
}

SHANDONG_JIANUO_APP = {
    "JNP44":  "Power",
    "JNP95":  "Power",
    "JNP96":  "Power",
    "JNP96A": "Power",
}

MAGNETEC_APP = {n: "Interference Suppression" for n in [
    "Nanoperm 90000", "Nanoperm 80000", "Nanoperm 30000", "Nanoperm 8000",
    "Nanoperm 4000", "Nanoperm 2000", "Nanoperm 1000",
]}

GAOTUNE_APP = {
    "AF": "Power",                    # amorphous, power transformers/inductors
    "AN": "Interference Suppression", # nanocrystalline, EMI common-mode chokes
}

PROTERIAL_APP = {
    "Finemet": "Interference Suppression",  # nanocrystalline FeSi, EMI chokes
    "Metglas": "Power",                     # amorphous FeSi, power transformers
}

HOEGANAES_APP = {
    "X-Indmix A": "Power",  # FeSi powder core, inductive energy storage
}

POCO_APP = {n: "Power" for n in
    ["NPF 26", "NPF 40", "NPF 60", "NPF 75", "NPF 90"]}

# TDG powder cores all → Power; TP = MnZn Power; TS = MnZn Signal Processing
TDG_APP = {}
for g in ["TMSA 19", "TMSA 26", "TMSA 40", "TMSA 60", "TMSA 75", "TMSA 90",
          "TMSA 125", "TMSC 26", "TMSC 60",
          "TMF 26", "TMF 40", "TMF 60", "TMF 75", "TMF 90",
          "TMFA 60", "TMFA 90", "TMFD",
          "TMFB 26", "TMFB 60", "TMFC 26", "TMFC 60",
          "TMH 26", "TMH 40", "TMH 60", "TMH 75", "TMH 90", "TMH 125",
          "TMH 147", "TMH 160",
          "TMHA 60", "TMHA 75", "TMHA 90",
          "TMHB 26", "TMHB 40", "TMHB 60", "TMHB 75", "TMHB 90",
          "TMHG 60", "TMM 26", "TMM 60",
          "TP4A", "TP5"]:
    TDG_APP[g] = "Power"
TDG_APP["TS5"] = "Signal Processing"
TDG_APP["TS7"] = "Signal Processing"

MANUFACTURER_APP = {
    "TDK":              TDK_APP,
    "Ferroxcube":       FERROXCUBE_APP,
    "Fair-Rite":        FAIR_RITE_APP,
    "ACME":             ACME_APP,
    "DMEGC":            DMEGC_APP,
    "Magnetics":        MAGNETICS_APP,
    "Micrometals":      MICROMETALS_APP,
    "Sinomag":          SINOMAG_APP,
    "Shandong Jianuo":  SHANDONG_JIANUO_APP,
    "Magnetec":         MAGNETEC_APP,
    "Gaotune":          GAOTUNE_APP,
    "Proterial":        PROTERIAL_APP,
    "Höganäs":          HOEGANAES_APP,
    "Poco":             POCO_APP,
    "TDG":              TDG_APP,
}

# ── URL fixes: (manufacturer, name) → new datasheetUrl ───────────────────────
# Only entries that need to be added or corrected (not already correct).

_DMEGC_MNZN_CATALOG = ("https://dongyangdongci.oss-cn-hangzhou.aliyuncs.com"
                        "/uploads/20240625/%E9%94%B0%E9%94%8C%E7%9B%AE%E5%BD%95.pdf")
_TDG_POWDER_CATALOG  = "https://www.tdgcore.com/en/technical-support/data-download-145.html"

URL_FIXES = {
    # Ferroxcube 3C99 — old URL was the contact page
    ("Ferroxcube", "3C99"):
        "https://www.ferroxcube.com/upload/media/product/file/MDS/3c99.pdf",

    # ACME A061 — old URL used outdated mhw-intl.com mirror
    ("ACME", "A061"):
        "https://www.acme-ferrite.com.tw/img/CorePDF/acme_product_now.pdf",

    # DMEGC — missing entries; use the comprehensive MnZn catalog
    ("DMEGC", "DMR40"):   _DMEGC_MNZN_CATALOG,
    ("DMEGC", "DMR44"):   _DMEGC_MNZN_CATALOG,
    ("DMEGC", "DMR47"):   _DMEGC_MNZN_CATALOG,
    ("DMEGC", "DMR50"):   _DMEGC_MNZN_CATALOG,
    ("DMEGC", "DMR53"):   _DMEGC_MNZN_CATALOG,
    ("DMEGC", "DMR95"):   _DMEGC_MNZN_CATALOG,
    ("DMEGC", "DMR96A"):  _DMEGC_MNZN_CATALOG,
    # R-series without KC/KZ suffix
    ("DMEGC", "R5K"):
        "https://dongyangdongci.oss-cn-hangzhou.aliyuncs.com/uploads/20230401/R5KC%20Material%20Characteristics.pdf",
    ("DMEGC", "R7K"):
        "https://dongyangdongci.oss-cn-hangzhou.aliyuncs.com/uploads/20240817/R7KC%20Material%20Characteristics.pdf",
    ("DMEGC", "R10K"):
        "https://dongyangdongci.oss-cn-hangzhou.aliyuncs.com/uploads/20240229/R10KC%20Material%20Characteristics.pdf",
    ("DMEGC", "R12K"):
        "https://dongyangdongci.oss-cn-hangzhou.aliyuncs.com/uploads/20230401/R12KZ%20Material%20Characteristics.pdf",
    ("DMEGC", "R15K"):
        "https://dongyangdongci.oss-cn-hangzhou.aliyuncs.com/uploads/20230401/R15KZ%20Material%20Characteristics.pdf",

    # TDG — all 45 materials missing; point to consolidated powder-core catalog
    **{("TDG", g): _TDG_POWDER_CATALOG for g in TDG_APP},
}

# ── Heuristic fallback (used when no lookup-table match is found) ─────────────

def _heuristic_application(entry: dict) -> str | None:
    comp  = entry.get("materialComposition", "")
    mtype = entry.get("material", "")

    if mtype == "nanocrystalline":
        return "Interference Suppression"
    if mtype == "amorphous":
        return "Power"
    if mtype == "powder":
        return "Power"
    if comp == "NiZn" and mtype == "ferrite":
        return "Interference Suppression"
    if comp == "MnZn" and mtype == "ferrite":
        return "Power"
    return None


# ── Main ──────────────────────────────────────────────────────────────────────

def main():
    with open(DB_PATH, encoding="utf-8") as fh:
        lines = fh.readlines()

    updated_lines   = []
    app_added       = 0
    app_skipped     = 0
    url_fixed       = 0
    unresolved      = []

    for raw in lines:
        entry = json.loads(raw)
        mfr   = entry.get("manufacturerInfo", {}).get("name", "")
        name  = entry.get("name", "")

        # ── Application field ─────────────────────────────────────────────
        if "application" not in entry:
            app = (MANUFACTURER_APP.get(mfr, {}).get(name)
                   or _heuristic_application(entry))
            if app:
                entry["application"] = app
                app_added += 1
            else:
                unresolved.append(f"{mfr} / {name}")
        else:
            app_skipped += 1

        # ── datasheetUrl fixes ────────────────────────────────────────────
        fix_url = URL_FIXES.get((mfr, name))
        if fix_url:
            info = entry.setdefault("manufacturerInfo", {})
            old  = info.get("datasheetUrl", "")
            if old != fix_url:
                info["datasheetUrl"] = fix_url
                url_fixed += 1

        updated_lines.append(json.dumps(entry, ensure_ascii=False) + "\n")

    with open(DB_PATH, "w", encoding="utf-8") as fh:
        fh.writelines(updated_lines)

    total = len(lines)
    filled = sum(1 for l in updated_lines if '"application"' in l)
    print(f"application: {filled}/{total} filled  "
          f"(+{app_added} added, {app_skipped} already set)")
    print(f"datasheetUrl: {url_fixed} entries fixed/added")
    if unresolved:
        print(f"\nUnresolved ({len(unresolved)}):")
        for m in sorted(unresolved):
            print(f"  {m}")
    else:
        print("All materials resolved.")


if __name__ == "__main__":
    main()

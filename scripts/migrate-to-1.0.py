#!/usr/bin/env python3
"""Migrate a MAS document from a pre-1.0 (typically 0.2.x) shape to 1.0.

What changes:
  * Enum values are renamed to camelCase (RFC 0007). All old spellings
    in the table below are rewritten to their 1.0 equivalent.
  * pollutionDegree codes "P1" / "P2" / "P3" become "PD1" / "PD2" / "PD3"
    per IEC 60664-1 (RFC 0008).
  * overvoltageCategory values "OVC-I" ... "OVC-IV" become bare
    "I" ... "IV" per IEC 60664-1 (RFC 0008).
  * masVersion is set to "1.0.0" if missing or older.

This is an offline batch tool. For tools that need to keep accepting old
files at load time, mirror the same MAPPING in your loading layer (see
MKF's mas_compat.hpp for the C++ port).

Usage:
    python3 scripts/migrate-to-1.0.py <input.json> [-o <output.json>]
    python3 scripts/migrate-to-1.0.py <dir>/                  # in-place
"""
import argparse
import json
import os
import sys

# Single source of truth. Mirror in MKF mas_compat.hpp.
MAPPING = {
    # insulationType
    "Functional": "functional",
    "Basic": "basic",
    "Supplementary": "supplementary",
    "Double": "double",
    "Reinforced": "reinforced",
    # wiringTechnology
    "Wound": "wound",
    "Printed": "printed",
    "Stamped": "stamped",
    "Deposition": "deposition",
    # application
    "Power": "power",
    "Signal Processing": "signalProcessing",
    "Interference Suppression": "interferenceSuppression",
    # subApplication
    "Power Filtering": "powerFiltering",
    "Transforming": "transforming",
    "Isolation": "isolation",
    "Common Mode Noise Filtering": "commonModeNoiseFiltering",
    "Differential Mode Noise Filtering": "differentialModeNoiseFiltering",
    # connectionType
    "Pin": "pin",
    "Screw": "screw",
    "SMT": "smt",
    "Flying Lead": "flyingLead",
    "THT": "tht",
    "PCB Pad": "pcbPad",
    # coilAlignment
    "inner or top": "innerOrTop",
    "outer or bottom": "outerOrBottom",
    # coreType
    "two-piece set": "twoPieceSet",
    "piece and plate": "pieceAndPlate",
    "closed shape": "closedShape",
    # piece type
    "half set": "halfSet",
    # DMC configuration
    "SINGLE_PHASE": "singlePhase",
    "THREE_PHASE": "threePhase",
    "THREE_PHASE_WITH_NEUTRAL": "threePhaseWithNeutral",
    # waveformLabel
    "Custom": "custom",
    "Triangular": "triangular",
    "Sinusoidal": "sinusoidal",
    "Rectangular": "rectangular",
    "Unipolar Rectangular": "unipolarRectangular",
    "Unipolar Triangular": "unipolarTriangular",
    "Bipolar Rectangular": "bipolarRectangular",
    "Bipolar Triangular": "bipolarTriangular",
    "Flyback Primary": "flybackPrimary",
    "Flyback Secondary": "flybackSecondary",
    "Rectangular With Deadtime": "rectangularWithDeadtime",
    "Flyback Secondary With Deadtime": "flybackSecondaryWithDeadtime",
    "Rectangular DCM": "rectangularDCM",
    "Secondary Rectangular": "secondaryRectangular",
    "Secondary Rectangular With Deadtime": "secondaryRectangularWithDeadtime",
    "Triangular With Deadtime": "triangularWithDeadtime",
    # topology
    "Buck Converter": "buckConverter",
    "Boost Converter": "boostConverter",
    "Inverting Buck-Boost Converter": "invertingBuckBoostConverter",
    "Cuk Converter": "cukConverter",
    "Zeta Converter": "zetaConverter",
    "Flyback Converter": "flybackConverter",
    "Active Clamp Forward Converter": "activeClampForwardConverter",
    "Single Switch Forward Converter": "singleSwitchForwardConverter",
    "Two Switch Forward Converter": "twoSwitchForwardConverter",
    "Push-Pull Converter": "pushPullConverter",
    "Weinberg Converter": "weinbergConverter",
    "Half-Bridge Converter": "halfBridgeConverter",
    "Full-Bridge Converter": "fullBridgeConverter",
    "Phase-Shifted Full-Bridge Converter": "phaseShiftedFullBridgeConverter",
    "Phase-Shifted Half-Bridge Converter": "phaseShiftedHalfBridgeConverter",
    "Current Transformer": "currentTransformer",
    "Isolated Buck Converter": "isolatedBuckConverter",
    "Isolated Buck-Boost Converter": "isolatedBuckBoostConverter",
    "Dual Active Bridge Converter": "dualActiveBridgeConverter",
    "LLC Resonant Converter": "llcResonantConverter",
    "CLLC Resonant Converter": "cllcResonantConverter",
    "Common Mode Choke": "commonModeChoke",
    "Differential Mode Choke": "differentialModeChoke",
    "Power Factor Correction": "powerFactorCorrection",
    # flybackModes
    "Continuous Conduction Mode": "continuousConductionMode",
    "Discontinuous Conduction Mode": "discontinuousConductionMode",
    "Quasi Resonant Mode": "quasiResonantMode",
    "Boundary Mode Operation": "boundaryModeOperation",
    # pfcModes
    "Critical Conduction Mode": "criticalConductionMode",
    "Transition Mode": "transitionMode",
    # bridge types
    "Half Bridge": "halfBridge",
    "Full Bridge": "fullBridge",
    "Center Tapped": "centerTapped",
    "Current Doubler": "currentDoubler",
    # cllcPowerFlow
    "Forward": "forward",
    "Reverse": "reverse",
    # market
    "Medical": "medical",
    "Commercial": "commercial",
    "Industrial": "industrial",
    "Military": "military",
    "Space": "space",
    # materialComposition
    "Carbonyl Iron": "carbonylIron",
    "Iron": "iron",
    "Proprietary": "proprietary",
    # cti
    "Group I": "groupI",
    "Group II": "groupII",
    "Group IIIA": "groupIIIA",
    "Group IIIB": "groupIIIB",
    # pollutionDegree (RFC 0008)
    "P1": "PD1",
    "P2": "PD2",
    "P3": "PD3",
    # overvoltageCategory (RFC 0008)
    "OVC-I": "I",
    "OVC-II": "II",
    "OVC-III": "III",
    "OVC-IV": "IV",
    # shape family
    "planar e": "planarE",
    "planar er": "planarER",
    "planar el": "planarEL",
    # outputVoltagesType / outputCurrentsType (renamed in 0.2.x)
    # nothing to map for these — values were already lowercase
}

TARGET_VERSION = "1.0.0"


def remap(value):
    if isinstance(value, str) and value in MAPPING:
        return MAPPING[value]
    return value


def walk(obj):
    if isinstance(obj, dict):
        return {k: walk(v) for k, v in obj.items()}
    if isinstance(obj, list):
        return [walk(v) for v in obj]
    return remap(obj)


def migrate_doc(doc: dict) -> dict:
    out = walk(doc)
    if isinstance(out, dict):
        ver = out.get("masVersion")
        if not ver or _semver_lt(ver, TARGET_VERSION):
            new = {"masVersion": TARGET_VERSION}
            for k, v in out.items():
                if k != "masVersion":
                    new[k] = v
            out = new
    return out


def _semver_lt(a: str, b: str) -> bool:
    def parts(s):
        s = s.split("-", 1)[0]
        return tuple(int(x) for x in s.split("."))
    try:
        return parts(a) < parts(b)
    except Exception:
        return True


def migrate_path(path: str, output: str | None) -> int:
    with open(path) as fh:
        doc = json.load(fh)
    out = migrate_doc(doc)
    target = output or path
    with open(target, "w") as fh:
        json.dump(out, fh, indent=4)
        fh.write("\n")
    print(f"migrated {path} -> {target}")
    return 0


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("path", help="MAS document file or directory")
    ap.add_argument("-o", "--output", help="output file (file mode only)")
    args = ap.parse_args()

    if os.path.isdir(args.path):
        if args.output:
            print("ERROR: -o cannot be used with a directory; migration is in-place", file=sys.stderr)
            return 2
        rc = 0
        for root, _, files in os.walk(args.path):
            for f in files:
                if f.endswith(".json"):
                    p = os.path.join(root, f)
                    rc |= migrate_path(p, None)
        return rc
    return migrate_path(args.path, args.output)


if __name__ == "__main__":
    sys.exit(main())

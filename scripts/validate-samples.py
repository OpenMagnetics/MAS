#!/usr/bin/env python3
"""Validate every sample under samples/ against schemas/MAS.json.

Fails (non-zero exit) on the first invalid sample. Used in CI and as a
pre-commit sanity check.

Usage:
    python3 scripts/validate-samples.py [--root <dir>]

Default root is the current directory; expects schemas/MAS.json and
samples/**.json to live below it.
"""
import argparse
import glob
import json
import os
import sys

try:
    from jsonschema import Draft202012Validator
    from referencing import Registry, Resource
except ImportError:
    print("ERROR: pip install jsonschema referencing", file=sys.stderr)
    sys.exit(2)


def load_registry(schema_dir: str) -> Registry:
    resources = []
    for f in sorted(glob.glob(os.path.join(schema_dir, "**", "*.json"), recursive=True)):
        with open(f) as fh:
            s = json.load(fh)
        if "$id" not in s:
            print(f"WARN: {f} has no $id, skipping", file=sys.stderr)
            continue
        resources.append((s["$id"], Resource.from_contents(s)))
    return Registry().with_resources(resources)


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--root", default=".", help="repo root (default: cwd)")
    ap.add_argument(
        "--samples",
        default="samples/complete",
        help="samples dir (default: samples/complete; only complete MAS documents live here, not per-record sub-schema fixtures)",
    )
    ap.add_argument(
        "--schema",
        default="schemas/MAS.json",
        help="schema to validate against (default: schemas/MAS.json)",
    )
    args = ap.parse_args()

    os.chdir(args.root)
    registry = load_registry("schemas")
    schema = json.load(open(args.schema))
    validator = Draft202012Validator(schema, registry=registry)

    samples = sorted(glob.glob(os.path.join(args.samples, "**", "*.json"), recursive=True))
    if not samples:
        print(f"no samples found under {args.samples}/", file=sys.stderr)
        return 1

    failed = 0
    for path in samples:
        with open(path) as fh:
            doc = json.load(fh)
        errors = list(validator.iter_errors(doc))
        if errors:
            failed += 1
            print(f"FAIL {path}: {len(errors)} errors")
            for err in errors[:3]:
                loc = ".".join(map(str, err.path)) or "<root>"
                print(f"   - {loc}: {err.message[:140]}")
        else:
            print(f"OK   {path}")

    if failed:
        print(f"\n{failed} of {len(samples)} sample(s) failed validation", file=sys.stderr)
        return 1
    print(f"\nall {len(samples)} sample(s) valid")
    return 0


if __name__ == "__main__":
    sys.exit(main())

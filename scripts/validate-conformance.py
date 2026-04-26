#!/usr/bin/env python3
"""Validate every test vector under tests/conformance/class-{A,B,C}/ against
the corresponding conformance bundle.

Used in CI to enforce that the conformance classes are mechanically exercised,
not just documented.

Usage:
    python3 scripts/validate-conformance.py [--root <dir>]
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


CLASSES = ("A", "B", "C")


def load_registry(schema_dir: str) -> Registry:
    resources = []
    for f in sorted(glob.glob(os.path.join(schema_dir, "**", "*.json"), recursive=True)):
        with open(f) as fh:
            s = json.load(fh)
        if "$id" not in s:
            continue
        resources.append((s["$id"], Resource.from_contents(s)))
    return Registry().with_resources(resources)


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--root", default=".", help="repo root (default: cwd)")
    args = ap.parse_args()
    os.chdir(args.root)

    registry = load_registry("schemas")
    failed = 0
    total = 0

    for cls in CLASSES:
        bundle_path = f"schemas/conformance/class-{cls}.json"
        if not os.path.exists(bundle_path):
            print(f"SKIP class {cls}: bundle not found at {bundle_path}", file=sys.stderr)
            continue
        bundle = json.load(open(bundle_path))
        validator = Draft202012Validator(bundle, registry=registry)

        vectors = sorted(glob.glob(f"tests/conformance/class-{cls}/*.json"))
        if not vectors:
            print(f"SKIP class {cls}: no test vectors under tests/conformance/class-{cls}/")
            continue

        for path in vectors:
            total += 1
            doc = json.load(open(path))
            errors = list(validator.iter_errors(doc))
            if errors:
                failed += 1
                print(f"FAIL {path} (class {cls}): {len(errors)} errors")
                for err in errors[:3]:
                    loc = ".".join(map(str, err.path)) or "<root>"
                    print(f"   - {loc}: {err.message[:140]}")
            else:
                print(f"OK   {path} (class {cls})")

    if total == 0:
        print("no conformance test vectors found", file=sys.stderr)
        return 1
    if failed:
        print(f"\n{failed} of {total} vector(s) failed conformance validation", file=sys.stderr)
        return 1
    print(f"\nall {total} conformance vector(s) valid")
    return 0


if __name__ == "__main__":
    sys.exit(main())

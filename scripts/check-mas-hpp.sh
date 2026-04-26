#!/usr/bin/env bash
# Re-run quicktype against the schemas and fail if the committed MAS.hpp
# differs from a fresh regen. Used in CI to prevent MAS.hpp drift.
#
# Requires:
#   - quicktype on PATH (npm install -g quicktype)
#   - bash, diff
#
# Usage:
#   scripts/check-mas-hpp.sh

set -euo pipefail

cd "$(dirname "$0")/.."

if ! command -v quicktype >/dev/null 2>&1; then
    echo "ERROR: quicktype not on PATH (npm install -g quicktype)" >&2
    exit 2
fi

if [[ ! -f MAS.hpp ]]; then
    echo "ERROR: MAS.hpp not found at repo root" >&2
    exit 2
fi

# quicktype derives the root class name from the output file basename, so
# the temp file must literally be named "MAS.hpp" or the diff will be
# spurious.
tmpdir="$(mktemp -d)"
tmp="$tmpdir/MAS.hpp"
trap 'rm -rf "$tmpdir"' EXIT

quicktype -l c++ -s schema ./schemas/MAS.json \
    -S ./schemas/magnetic.json -S ./schemas/magnetic/core.json -S ./schemas/magnetic/coil.json -S ./schemas/utils.json \
    -S ./schemas/magnetic/core/gap.json -S ./schemas/magnetic/core/shape.json -S ./schemas/magnetic/core/material.json \
    -S ./schemas/magnetic/insulation/material.json -S ./schemas/magnetic/insulation/wireCoating.json -S ./schemas/magnetic/bobbin.json \
    -S ./schemas/magnetic/core/piece.json -S ./schemas/magnetic/core/spacer.json \
    -S ./schemas/magnetic/wire/basicWire.json -S ./schemas/magnetic/wire/round.json -S ./schemas/magnetic/wire/rectangular.json \
    -S ./schemas/magnetic/wire/foil.json -S ./schemas/magnetic/wire/planar.json -S ./schemas/magnetic/wire/litz.json \
    -S ./schemas/magnetic/wire/material.json -S ./schemas/magnetic/wire.json \
    -S ./schemas/inputs.json -S ./schemas/outputs.json -S ./schemas/outputs/coreLossesOutput.json \
    -S ./schemas/inputs/designRequirements.json -S ./schemas/inputs/operatingPoint.json \
    -S ./schemas/inputs/operatingConditions.json -S ./schemas/inputs/operatingPointExcitation.json \
    -S ./schemas/inputs/topologies/flyback.json -S ./schemas/inputs/topologies/currentTransformer.json \
    -S ./schemas/inputs/topologies/buck.json -S ./schemas/inputs/topologies/isolatedBuck.json \
    -S ./schemas/inputs/topologies/isolatedBuckBoost.json -S ./schemas/inputs/topologies/boost.json \
    -S ./schemas/inputs/topologies/pushPull.json -S ./schemas/inputs/topologies/forward.json \
    -o "$tmp" --namespace MAS --source-style single-source \
    --type-style pascal-case --member-style underscore-case \
    --enumerator-style upper-underscore-case --no-boost

if diff -q MAS.hpp "$tmp" >/dev/null; then
    echo "MAS.hpp is up to date"
    exit 0
fi

echo "ERROR: MAS.hpp is stale relative to schemas/. Re-run quicktype and commit." >&2
echo "Diff (head):" >&2
diff -u MAS.hpp "$tmp" | head -40 >&2
exit 1

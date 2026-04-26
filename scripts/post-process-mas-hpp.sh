#!/usr/bin/env bash
# Post-process the quicktype-generated MAS.hpp to swap two misnamed classes.
#
# Background: quicktype's type unifier collapses inputs.operatingPoints (the
# full operating point with conditions+excitations, $ref operatingPoint.json)
# and outputs.windingLosses.currentPerWinding (same shape via the same $ref)
# into a single class. The chosen name is "CurrentPerWindingElement", which
# is semantically wrong: the type is fundamentally an operating point.
# Symmetrically, baseOperatingPoint is named "OperatingPoint" because the
# topology operating-point classes inherit it via allOf and share the
# *OperatingPoint suffix. The names are swapped from what consumers expect.
#
# This script swaps them atomically via a placeholder so the rename is safe
# regardless of file order:
#   CurrentPerWindingElement -> OperatingPoint
#   OperatingPoint           -> BaseOperatingPoint
#
# Word-boundary regex (\b) keeps FlybackOperatingPoint, OperatingPointExcitation,
# etc. untouched.
#
# Usage:
#   scripts/post-process-mas-hpp.sh <path-to-MAS.hpp>
#
# SPDX-License-Identifier: Apache-2.0

set -euo pipefail

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <MAS.hpp>" >&2
    exit 2
fi

target="$1"

if [[ ! -f "$target" ]]; then
    echo "ERROR: $target not found" >&2
    exit 2
fi

sed -E -i \
    -e 's/\bOperatingPoint\b/__SWAP_BASE_OP__/g' \
    -e 's/\bCurrentPerWindingElement\b/OperatingPoint/g' \
    -e 's/\bcurrent_per_winding_element\b/operating_point/g' \
    -e 's/\bCURRENT_PER_WINDING_ELEMENT\b/OPERATING_POINT/g' \
    -e 's/\b__SWAP_BASE_OP__\b/BaseOperatingPoint/g' \
    "$target"

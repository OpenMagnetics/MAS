# MAS-RFC 0001 — Normative SI units table (v2)

- **Status:** Draft (supersedes v1, "Quantity type and explicit SI units")
- **Type:** Mostly additive (one breaking item: prose-unit cleanup); target 0.2.0
- **Author:** _to be assigned_
- **Created:** 2026-04-26
- **Revised:** 2026-04-26

## Summary

Define a single normative table that fixes the unit of every numeric field
in MAS. Numeric fields stay bare numbers — no `{value, unit}` wrapper, no
per-document unit declaration. The schema documents reference the table
instead of repeating "in Celsius" / "in Hz" prose 50+ times. Add a small
`MeasuredValue` type only for catalogue fields that legitimately carry a
tolerance band, where the structure is paying for information beyond the
unit.

## Why v2 (and what changed)

The original RFC 0001 proposed wrapping every numeric field in
`{"value": …, "unit": "…"}`. After investigating MKF — the largest
known consumer — that design would cost ~3–4 weeks of refactor, double
JSON file sizes, and break PyMKF's user-visible API. The validation
benefit (catching `"unit": "kHz"` where the spec wants `"Hz"`) is
modest, because in a standard every field has exactly one allowed unit
anyway.

Peer engineering standards (SPICE, Modelica, OpenFOAM dictionaries, IEC
61131 data types) do not encode units per value. They fix units in the
specification text and validate types only. RFC 0001 v2 follows that
precedent.

The original v1 design is preserved in git history under
`proposals/0001-quantity-units.md` (now renamed; see *Migration of this
RFC* below).

## Proposal

### 1. Normative units table (`docs/units.md`)

A new document, referenced by every schema `description`. Single source of
truth. UCUM codes are listed for downstream interop (HL7 FHIR, IEC 61360
property records).

| Quantity | Unit | UCUM code | Notes |
|----------|------|-----------|-------|
| Frequency | hertz | `Hz` | switching, resonance, sampling |
| Time | second | `s` | duty-cycle interval, dwell |
| Temperature | degree Celsius | `Cel` | aligns with IEC 60085 thermal classes and every ferrite datasheet |
| Length | metre | `m` | bare metres in JSON; tools may render mm |
| Mass | kilogram | `kg` | core, winding, total |
| Current | ampere | `A` | DC, peak, RMS unless field name says otherwise |
| Voltage | volt | `V` | likewise |
| Resistance | ohm | `Ohm` | DC and AC effective |
| Inductance | henry | `H` | self, mutual, leakage |
| Capacitance | farad | `F` | inter-winding, stray |
| Energy | joule | `J` | per-cycle, stored |
| Power | watt | `W` | losses, throughput |
| Magnetic flux density | tesla | `T` | peak, AC component |
| Magnetic field strength | ampere per metre | `A/m` | H |
| Magnetic flux | weber | `Wb` | Φ |
| Permeability (relative) | dimensionless | `1` | μᵣ; absolute permeability is rarely used in MAS |
| Loss density | watt per cubic metre | `W/m3` | core loss curves |
| Thermal resistance | kelvin per watt | `K/W` | rise per loss |
| Thermal conductivity | watt per metre kelvin | `W/(m.K)` | material property |

Notes:

- **Temperature stays Celsius** (see RFC 0004 v2 — withdrawal note).
  All existing `description` strings already say Celsius; this table
  ratifies that.
- **Length stays metres in JSON.** Catalogue dimensions look unfamiliar
  (an ETD-34 leg is `0.0341` m, not `34.1` mm). This is a presentation
  problem for tools, not a schema problem. Document the convention
  loudly in `docs/units.md`.
- **Thermal resistance uses K/W** (not °C/W) because it is a *difference*,
  and Kelvin and Celsius differences are numerically identical. Either
  works; K/W is the SI form and what IEC standards use.

### 2. Schema-text sweep

Replace 53 hand-written "in Celsius" / "in Hz" / "in metres" descriptions
with a uniform pointer:

```diff
- "description": "Switching frequency of the operating point"
+ "description": "Switching frequency. Unit: Hz (see docs/units.md)."
```

Generated tooltips, IDE hovers and quicktype documentation comments stay
useful, and the unit is now in the spot the eye looks for it.

### 3. New `MeasuredValue` type in `utils.json`

For catalogue fields that carry a tolerance band, ship a small structure
— *not* applied universally:

```json
"MeasuredValue": {
  "description": "A measured catalogue value with optional tolerance band. The unit is fixed by the field that uses this type and is documented in docs/units.md.",
  "type": "object",
  "properties": {
    "nominal":  { "type": "number" },
    "minimum":  { "type": "number" },
    "maximum":  { "type": "number" },
    "typical":  { "type": "number" },
    "method":   { "type": "string", "description": "How the value was obtained: datasheet, measured, fitted, derived." }
  },
  "required": ["nominal"],
  "additionalProperties": false
}
```

Apply selectively, only where there's information beyond the number, e.g.:

- `coreMaterial.permeability.initial` — datasheets quote ranges, not points.
- `wire.material.resistivity` — ditto.
- `gap.length` — manufacturing tolerance is real.

A simple operating-point field like `switchingFrequency` stays a bare
number. We are paying for structure only when the structure is doing
work.

### 4. SPDX header on every schema file

Add to each schema `$comment`:

```json
"$comment": "SPDX-License-Identifier: Apache-2.0"
```

Independent of the units work but small enough to land in the same PR.

## Migration policy

- **0.2.0:** add `docs/units.md`, sweep schema descriptions, add
  `MeasuredValue` to `utils.json`. **Non-breaking** — bare numbers stay
  bare; the unit they always meant is now formalised.
- **0.3.0:** start migrating selected catalogue fields to
  `MeasuredValue`. Each migration is a MINOR change (additive: nominal
  is required, old consumers can still read `nominal` as the only number
  they care about).
- **No 1.0 break planned** for this RFC. The unit table can be tightened
  in PATCH releases.

## Cost

| Component | Effort |
|-----------|--------|
| MAS schema + docs | ~1 day |
| MKF | **zero functional changes**; `MAS.hpp` types unchanged |
| PyMKF | zero changes |
| Test fixtures | zero changes |

Compare RFC 0001 v1 (estimated): 5–6 person-weeks across MAS+MKF+PyMKF.

## Migration of this RFC

The v1 file `proposals/0001-quantity-units.md` is renamed to
`proposals/0001-units.md` (this file). v1 design is preserved in git
history; the rename produces a single contiguous RFC number with the
revision history visible via `git log --follow`.

## Rejected alternatives

- **Per-field `Quantity` wrapper everywhere** (v1). Rejected: cost out
  of proportion to the validation benefit, and breaks the principle
  that a standard fixes its units in the specification text.
- **Document-level `units` block at the root.** Rejected: fragments
  (e.g. a single wire record extracted from a catalogue) lose meaning
  when the root block is left behind. Worst-of-both-worlds.
- **JSON-LD with QUDT IRIs.** Overkill, drags a semantic-web toolchain
  into a schema that ships to embedded C++ users.

## Open questions

1. Should the bundled component database (`data/*.ndjson`) be migrated
   to `MeasuredValue` for permeability and similar fields up-front, or
   gradually? Recommendation: gradually, one record class per release.
2. Do we want a `prefix` enum on `MeasuredValue` for length only
   (`m` vs `mm`)? Recommendation: no — keep one canonical wire format,
   let tools handle display.
3. Does the IEC 61360 / CDD registration work (RFC 0003) need the
   table to use IRDIs as well as UCUM codes? Probably yes once CDD
   classes are minted; add a column then.

# MAS-RFC 0001 — Normative SI units table (v2)

- **Status:** Draft (supersedes v1, "Quantity type and explicit SI units")
- **Type:** Mostly additive (one breaking item: prose-unit cleanup); target 0.2.0
- **Author:** _to be assigned_
- **Created:** 2026-04-26
- **Revised:** 2026-04-26

## Summary

Define a single normative table that fixes the unit of every numeric field
in MAS. Numeric fields stay bare numbers — no `{value, unit}` wrapper, no
per-document unit declaration, no per-value tolerance struct. The schema
documents reference the table instead of repeating "in Celsius" / "in Hz"
prose 50+ times.

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

### 3. Companion document `docs/normative-references.md`

Maps each MAS area (shapes, wires, insulation, materials, test methods)
to the external standards it defers to, plus the four genuine gaps where
no standard exists and MAS is the de-facto registry. Schema
`description` strings append a one-line pointer
(`"... See docs/normative-references.md §2a."`) so the link is
discoverable from the JSON.

### 4. SPDX header on every schema file

Add to each schema `$comment`:

```json
"$comment": "SPDX-License-Identifier: Apache-2.0"
```

Independent of the units work but small enough to land in the same PR.

## Migration policy

- **0.2.0:** add `docs/units.md`, sweep schema descriptions. **Non-breaking** —
  bare numbers stay bare; the unit they always meant is now formalised.
- **No 1.0 break planned** for this RFC. The unit table can be tightened
  in PATCH releases.

If a future field genuinely needs a tolerance band (datasheet quotes
min/max with no nominal, manufacturing acceptance criteria, etc.), add
sibling fields at that point (`<field>Min`, `<field>Max`) rather than
introducing a wrapper struct that pays no rent on the 99% of fields that
are single numbers.

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

1. Does the IEC 61360 / CDD registration work (RFC 0003) need the
   table to use IRDIs as well as UCUM codes? Probably yes once CDD
   classes are minted; add a column then.

# MAS-RFC 0004 — Temperature in Kelvin everywhere

- **Status:** Draft
- **Type:** Breaking (target: 1.0.0)
- **Author:** _to be assigned_
- **Created:** 2026-04-26

## Summary

Eliminate the °C / K split in MAS. All temperature-bearing fields move to
**Kelvin**. Conversion to °C (or °F) is presentation, handled outside the
schema.

## Motivation

Today:

- `baseOperatingPoint.ambientTemperature` is documented in **°C**
  (`utils.json:31`).
- Material loss-model coefficients and permeability curves use **K**.
- A document is silently broken if a tool reads the ambient as K (or vice
  versa) — there is no validator that catches it.

For an IEC deliverable, mixing units within a single specification is
indefensible. SI base unit for temperature is the **kelvin**; that is what
should appear on the wire.

Note that this RFC is independent of RFC 0001 (`Quantity`). With or
without `Quantity`, the unit story for temperature must be *one* unit.

## Proposal

### Schema changes (at 1.0.0)

Every numeric field whose `description` mentions "temperature" is
re-documented as Kelvin. Concretely (non-exhaustive):

- `utils.json#/$defs/baseOperatingPoint/properties/ambientTemperature`
- `outputs/temperature.json` (every field)
- `magnetic/coreMaterial.json` curve x-axes that currently mix K and °C
- `inputs/designRequirements.json` operating-temperature-range fields

### Migration policy

- **0.x → 0.(x+1):** add a `temperatureUnit` discriminator at the root of
  each operating point: `"K"` (new) or `"Cel"` (legacy). Validators warn
  on `Cel`. Default remains `Cel` to avoid breakage.
- **1.0.0:** discriminator removed; all temperature fields are kelvin.
  `temperatureUnit: "Cel"` is a validation error.
- Migration script `scripts/migrate_celsius_to_kelvin.py` rewrites in
  place: `K = Cel + 273.15`, with a `--dry-run` mode and a report of
  every field touched.

### Documentation

Add a one-page note in `docs/units.md` explaining:

- Why kelvin (SI base unit, no negative values, ratio-meaningful).
- How to render Celsius for users (`displayCelsius = K - 273.15`).
- Why the `Cel` UCUM code is **not** allowed in `Quantity.unit` for
  temperature fields (RFC 0001 cross-reference).

## Rejected alternatives

- **Keep °C for ambient, K for material** (current state). Rejected
  because there is no validator that catches a misread. The only way to
  remove the foot-gun is to remove the choice.
- **Allow either, require `Quantity` to disambiguate.** Rejected because
  it pushes the responsibility onto every consumer instead of fixing it
  once at the spec level.

## Impact assessment

- All 8 sample documents under `samples/complete/` need the
  `ambientTemperature` field rewritten (`25` → `298.15`).
- All 410+ material records under `data/core_materials.ndjson` need
  audit: the curves are already in K but the **legend / metadata** fields
  may not be.
- C++ binding: no struct changes, only documentation; consumers that
  hard-coded °C need to update their math.

## Open questions

1. Should the migration script also rewrite displayed UI strings in
   downstream tools? Out of scope; tools own their UX.
2. Do we want to add a `Quantity` form (`{value: 298.15, unit: "K"}`) at
   the same time, coupling RFC 0001 and 0004 into a single 1.0 release?
   Recommendation: yes, fewer breaking releases is better.

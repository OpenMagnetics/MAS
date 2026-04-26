# MAS-RFC 0001 — `Quantity` type and explicit SI units

- **Status:** Draft
- **Type:** Breaking (target: 1.0.0)
- **Author:** _to be assigned_
- **Created:** 2026-04-26

## Summary

Replace bare numeric fields (`switchingFrequency: 100000`,
`ambientTemperature: 25`) with a structured `Quantity` type carrying both a
magnitude and a unit. Constrain the unit by enum at every use-site to lock
SI usage into the validation surface, not the prose.

## Motivation

MAS today documents units in field `description`s but the JSON carries bare
numbers. This is fine for a single project but unacceptable for an IEC
deliverable, because:

1. A consumer cannot decide field-by-field whether `25` is °C, K, or °F
   without parsing prose.
2. Mixed conventions exist already (ambient = °C, material curves = K).
3. There is no validation lever to prevent a vendor library from quietly
   shipping `mm` where the spec says `m`.
4. Future translation to IEC 61360 / CDD properties requires each property
   to carry an explicit unit code anyway.

## Proposal

Add to `schemas/utils.json`:

```json
"$defs": {
  "Quantity": {
    "description": "A scalar SI quantity. The allowed units at each use-site are constrained by an inline `enum` on the `unit` property.",
    "type": "object",
    "properties": {
      "value":    { "type": "number" },
      "unit":     { "type": "string" },
      "nominal":  { "type": "number", "description": "Optional nominal value when `value` is a measured/typical reading." },
      "tolerance":{ "$ref": "#/$defs/Tolerance" }
    },
    "required": ["value", "unit"],
    "additionalProperties": false
  },
  "Tolerance": {
    "type": "object",
    "properties": {
      "minimum": { "type": "number" },
      "maximum": { "type": "number" }
    },
    "additionalProperties": false
  }
}
```

Use-site example (`baseOperatingPoint.switchingFrequency`):

```json
"switchingFrequency": {
  "description": "Switching frequency of the operating point.",
  "allOf": [
    { "$ref": "./utils.json#/$defs/Quantity" },
    { "properties": { "unit": { "const": "Hz" } } }
  ]
}
```

Allowed unit codes: **UCUM case-sensitive** (`Hz`, `K`, `m`, `A`, `V`, `H`,
`F`, `Ohm`, `kg`, `s`, `T`, `Wb`, `W`, `J`). Rationale: UCUM is already an
IEC/ISO-friendly choice (used by HL7 FHIR, OGC) and disambiguates `H`
(henry) from `h` (hour).

### Migration policy

- 0.x: introduce `Quantity` in `utils.json`. New fields use it. Old fields
  remain bare numbers.
- 0.(x+1): every existing numeric field gains a sibling
  `<field>Quantity` (e.g. `switchingFrequency` + `switchingFrequencyQuantity`).
  Validators warn (not error) if both are present and disagree.
- **1.0.0:** bare-number fields removed; `Quantity` form is the only
  accepted form. Migration script (`scripts/migrate_0x_to_10.py`) shipped.

### Code-generation impact

`MAS.hpp` gains a `MAS::Quantity` struct. quicktype handles `allOf` with
const refinements as a tagged subtype, so per-field unit constants survive
to C++ as `static constexpr`. Verify on a branch before merging.

## Rejected alternatives

- **Field-name suffixes** (`switchingFrequencyHz`). Cheaper, but 200+
  renames, no place to attach tolerance/nominal, and no enforcement at the
  JSON level — only at the eyeball level.
- **JSON-LD with `@context` mapping units to QUDT IRIs.** Overkill, drags a
  semantic-web toolchain into a schema that needs to ship to embedded
  C++ users.
- **xs:duration / ISO 8601 lexical forms.** Doesn't generalise to
  inductance, flux density, etc.

## Open questions

1. Should `Quantity` allow `unit: "Cel"` (UCUM for °C) at all, or force K
   everywhere? RFC 0004 covers this.
2. Do we want a `prefix` field (`"unit": "Hz", "prefix": "k"`) or rely on
   raw SI base units only? Recommendation: raw base units; UI layers
   format.
3. How are vector quantities handled (B(t), I(t) waveforms)? Out of scope
   here — separate `Waveform` type already exists in `outputs/`.

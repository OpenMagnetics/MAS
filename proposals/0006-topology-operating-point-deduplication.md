# MAS-RFC 0006 — Topology operating-point deduplication

- **Status:** Draft
- **Type:** Refactor (no semantic change for valid documents)
- **Author:** _to be assigned_
- **Created:** 2026-04-26

## Summary

Eliminate ~700 lines of inline duplication across the 16 topology
schemas by introducing a small set of named operating-point base
schemas and having each topology compose from them. Existing valid
documents continue to validate. The benefit is single-source-of-truth
behaviour for any future addition to operating-point fields (such as
the `outputVoltagesForm` / `outputCurrentsForm` discriminators added
in the recent units cleanup).

## Motivation

Of the 16 topology schemas under `schemas/inputs/topologies/`:

- **Two** (`buck.json`, `boost.json`) cleanly reference
  `utils.json#/$defs/baseOperatingPoint`.
- **Ten** (`flyback`, `forward`, `pushPull`, `llcResonant`,
  `cllcResonant`, `dualActiveBridge`, `phaseShiftFullBridge`,
  `isolatedBuck`, `isolatedBuckBoost`, `flybuck`) redefine the same
  five-field operating-point structure inline. The prose is
  identical, copy-pasted; the validation is independent.
- **Four** (`currentTransformer`, `commonModeChoke`,
  `differentialModeChoke`, `powerFactorCorrection`) have substantively
  different operating-point semantics and do not fit the standard
  five-field shape.

The cost of the inline duplication has already shown up: when the
recent units cleanup added `outputVoltagesForm` / `outputCurrentsForm`
to `baseOperatingPoint`, the addition was invisible to the ten
inlined topologies. Any future field addition has the same problem.

A second, related issue: three places independently define
"impedance at frequency" with three slightly different shapes
(`designRequirements.json#/$defs/impedanceAtFrequency`,
`commonModeChoke.json#/$defs/impedanceAtFrequency`,
`differentialModeChoke.json#/$defs/impedanceAtFrequency`). Same
concept, no shared $def.

## Proposal

### 1. Promote the standard operating point

`utils.json#/$defs/baseOperatingPoint` is already the right shape for
the ten inlined topologies. Each should reference it directly. For
topologies that need to add an extra field (e.g. flyback's `mode`,
PSFB's `phaseShift`, CLLC's `powerFlow`, DAB's modulation parameters),
the local definition becomes a small `allOf` extension:

```json
"flybackOperatingPoint": {
  "title": "flybackOperatingPoint",
  "allOf": [
    { "$ref": "../../utils.json#/$defs/baseOperatingPoint" },
    {
      "type": "object",
      "properties": {
        "mode": { "$ref": "#/$defs/flybackModes" }
      }
    }
  ]
}
```

Each of the ten inlined topologies shrinks to ~5 lines from ~50.

### 2. Promote `impedanceAtFrequency` to utils

Add `utils.json#/$defs/impedanceAtFrequency` as the canonical shape:

```json
"impedanceAtFrequency": {
  "type": "object",
  "title": "impedanceAtFrequency",
  "properties": {
    "frequency": { "type": "number", "minimum": 0,
                   "description": "Frequency. Unit: Hz." },
    "impedance": {
      "oneOf": [
        { "type": "number", "description": "Impedance magnitude. Unit: Ohm." },
        { "$ref": "../inputs/designRequirements.json#/$defs/impedancePoint" }
      ]
    }
  },
  "required": ["frequency", "impedance"]
}
```

`designRequirements`, `commonModeChoke` and `differentialModeChoke`
all reference it. The `oneOf` on `impedance` accepts both the bare
magnitude (used by EMI-choke topologies) and the structured
`impedancePoint` with magnitude/phase/real/imaginary (used by
`designRequirements.minimumImpedance`).

### 3. Topologies that don't fit baseOperatingPoint

`currentTransformer`, `commonModeChoke`, `differentialModeChoke` and
`powerFactorCorrection` have genuinely different operating-point
semantics:

- `currentTransformer` measures a primary signal, not power conversion.
- `commonModeChoke` and `differentialModeChoke` care about impedance
  at frequencies, not output voltages and currents.
- `powerFactorCorrection` uses an AC line frequency and an envelope.

These keep their bespoke shape but adopt shared $defs where they
overlap (notably `impedanceAtFrequency` from §2).

## Migration policy

- **0.2.0:** introduce the canonical `impedanceAtFrequency` in
  `utils.json` (additive). Topology operating-point refactor lands
  in the same release as the canonical reshape (also additive — no
  document that was valid before becomes invalid).
- **No 1.0 break required** for either change.

## Cost

- MAS schema: about 10 small files, each shrinking by ~30-50 lines.
  Net diff strongly negative.
- MKF: zero functional changes. The shape of valid documents is
  unchanged; quicktype-generated structs are unaffected (the field
  set per topology does not change).
- PyMKF: zero changes.

## Open questions

1. Should the `flyback` `mode` field (CCM/DCM/QRM/BCM) move to a
   shared `conductionMode` enum so PFC and DCM-flyback share the
   "Continuous/Discontinuous Conduction Mode" values? Probably yes —
   add as a follow-up.
2. The `phaseShift` field on PSFB and the `innerPhaseShift{1,2,3}` on
   DAB are all phase-shift parameters. Is there a shared
   `phaseShift` $def worth introducing? Unclear; maybe over-engineering.

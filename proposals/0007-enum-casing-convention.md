# MAS-RFC 0007 — Enum value casing convention

- **Status:** Draft
- **Type:** Breaking (target: 1.0.0)
- **Author:** _to be assigned_
- **Created:** 2026-04-26

## Summary

Pick one casing convention for enum values across all MAS schemas and
sweep. The current state is **eight different conventions**, which is
visible to every reviewer and unprofessional for a candidate
international standard.

## Motivation

The schema review found these conventions in active use, all in the
same project:

| Convention | Example field | Sample value |
|---|---|---|
| lowercase | `manufacturerInfo.status` | `production` |
| Title Case | `application` | `Power` |
| Title Case with spaces | `connectionType` | `Flying Lead` |
| lowercase with spaces | `coilAlignment` | `inner or top` |
| camelCase | `windingStyle` | `windByConsecutiveTurns` |
| UPPER_SNAKE_CASE | `differentialModeChoke.configuration` | `THREE_PHASE_WITH_NEUTRAL` |
| UPPERCASE abbreviations | `withstandVoltageType` | `AC` |
| Custom-prefixed | `pollutionDegree` | `P1` |

The cost of choosing a convention is one focused PR; the cost of
*not* choosing one is permanent.

## Proposal

Adopt **camelCase** for enum values across the schema, with two
explicit exceptions:

- **Acronyms** stay uppercase: `AC`, `DC`, `SPS`, `EPS`, `DPS`, `TPS`,
  `EMI`, `IEC`. Where an acronym is part of a larger value, it remains
  uppercase: `acVoltage` is wrong, write `ACVoltage` -- actually adopt
  the JavaScript convention: acronyms in the middle of a camelCase
  identifier are written `Title Case` (`htmlElement`, `jsonParser`),
  so `acVoltage`, `dcCurrent`. Standalone acronyms remain uppercase
  (`AC`, `DC`).
- **External standard codes** keep their canonical form: pollution
  degrees `PD1, PD2, PD3, PD4` per IEC 60664 (RFC 0008), IEC 60085
  insulation classes `Y, A, E, B, F, H, N, R, 200, 220, 250` (already
  in place).

Why camelCase rather than lowercase or PascalCase:

- Matches the property-name convention used everywhere else in MAS.
- Plays well with C++ / Python code generation (no separator handling).
- Matches how JSON Schema's own keywords are written
  (`additionalProperties`, `minItems`, `oneOf`).
- Avoids the "Title Case with spaces" problem of strings like
  `"Flying Lead"` that need quoting in switch statements.

## Mapping (the breaking surface)

A non-exhaustive mapping; the full sweep produces the authoritative list.

| Current | New |
|---|---|
| `production`, `prototype`, `obsolete` | unchanged |
| `Functional`, `Basic`, `Supplementary`, `Double`, `Reinforced` | `functional`, `basic`, `supplementary`, `double`, `reinforced` |
| `Wound`, `Printed`, `Stamped`, `Deposition` | `wound`, `printed`, `stamped`, `deposition` |
| `Power`, `Signal Processing`, `Interference Suppression` | `power`, `signalProcessing`, `interferenceSuppression` |
| `Pin`, `Screw`, `SMT`, `Flying Lead`, `THT`, `PCB Pad` | `pin`, `screw`, `smt`, `flyingLead`, `tht`, `pcbPad` |
| `inner or top`, `outer or bottom`, `spread`, `centered` | `innerOrTop`, `outerOrBottom`, `spread`, `centered` |
| `two-piece set`, `piece and plate`, `toroidal`, `closed shape` | `twoPieceSet`, `pieceAndPlate`, `toroidal`, `closedShape` |
| `SINGLE_PHASE`, `THREE_PHASE`, `THREE_PHASE_WITH_NEUTRAL` | `singlePhase`, `threePhase`, `threePhaseWithNeutral` |
| `Continuous Conduction Mode`, `Discontinuous Conduction Mode`, ... | `continuousConductionMode`, `discontinuousConductionMode`, ... |
| `Buck Converter`, `Boost Converter`, ... | `buckConverter`, `boostConverter`, ... |
| `Half Bridge`, `Full Bridge` | `halfBridge`, `fullBridge` |
| `Group I`, `Group II`, `Group IIIA`, `Group IIIB` | `groupI`, `groupII`, `groupIIIA`, `groupIIIB` |
| `OVC-I`, `OVC-II`, `OVC-III`, `OVC-IV` | `I`, `II`, `III`, `IV` (per IEC 60664; see RFC 0008) |
| `P1`, `P2`, `P3` | `PD1`, `PD2`, `PD3`, `PD4` (per IEC 60664; see RFC 0008) |
| `AC`, `DC` | unchanged (acronym exception) |
| `SPS`, `EPS`, `DPS`, `TPS` | unchanged (acronym exception) |
| `windByConsecutiveTurns`, `windByConsecutiveParallels` | unchanged |
| `cartesian`, `polar` | unchanged |
| `additive`, `subtractive`, `residual` | unchanged |

## Migration policy

- **0.x to 1.0:** introduce the new enum values *alongside* the old
  ones using `oneOf` with both string sets. Validators warn on the
  old form. Bundled `data/*.ndjson` is migrated in the same release.
- **1.0:** old enum values removed. A migration script
  (`scripts/migrate_enums.py`) is provided that rewrites every
  affected field in an instance document.

## Cost

- MAS: 1-2 days for the schema sweep, the migration script, and the
  catalogue data conversion.
- MKF: every C++ enum / switch statement on these values needs the
  new spelling. Mechanical change, ~1-2 days. PyMKF the same.
- Existing user JSON fixtures: must be migrated (script provided).

This is the highest-visibility cosmetic change in the backlog. It
would be embarrassing to land an IEC submission with eight different
casing conventions, so it must happen at some point. Doing it as part
of the 1.0 break (alongside RFC 0001 v2 units) means MKF takes one
upgrade hit, not two.

## Open questions

1. Should `withstandVoltageType` / `voltageType` keep the uppercase
   `AC`/`DC` (acronym exception) or join the lowercase trend
   (`ac`/`dc`)? Recommendation: keep uppercase — `AC` and `DC` are
   universally recognised as such.
2. Topology names such as `LLC Resonant Converter` and `CLLC Resonant
   Converter` involve acronym embedded in a longer name. Become
   `llcResonantConverter`, `cllcResonantConverter` (lowercase
   acronym at start) or `LLCResonantConverter`,
   `CLLCResonantConverter`? Recommendation: lowercase at start, per
   JavaScript / camelCase convention.
3. Single-character enum values like `Y`, `A`, `E`, `B`, `F`, `H`,
   `N`, `R` (insulation classes) — keep as-is per IEC 60085. Already
   the case.

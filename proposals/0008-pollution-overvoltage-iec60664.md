# MAS-RFC 0008 — Align pollutionDegree and overvoltageCategory with IEC 60664

- **Status:** Draft
- **Type:** Breaking (target: 1.0.0; couple with RFC 0007)
- **Author:** _to be assigned_
- **Created:** 2026-04-26

## Summary

Two safety-related enums in `inputs/designRequirements.json` use
non-standard naming. Align them with IEC 60664-1 spelling.

## Motivation

`schemas/inputs/designRequirements.json:88`:

```json
"pollutionDegree": {
  "enum": ["P1", "P2", "P3"]
}
```

IEC 60664-1 defines **four** pollution degrees, named **PD1, PD2,
PD3, PD4**. MAS lists only three and uses a non-standard prefix.
Pollution degree 4 (continuous conductivity caused by conductive dust,
rain, etc.) is a real category used in outdoor / industrial designs.

`schemas/inputs/designRequirements.json:93`:

```json
"overvoltageCategory": {
  "enum": ["OVC-I", "OVC-II", "OVC-III", "OVC-IV"]
}
```

IEC 60664-1 names these **categories I, II, III, IV** without the
`OVC-` prefix. The "OVC" prefix is a colloquial shortening occasionally
seen in vendor literature but is not the normative spelling.

These two are the only fields in the schema that explicitly
misrepresent IEC nomenclature.

## Proposal

```json
"pollutionDegree": {
  "description": "Pollution degree per IEC 60664-1.",
  "type": "string",
  "enum": ["PD1", "PD2", "PD3", "PD4"]
},
"overvoltageCategory": {
  "description": "Overvoltage category per IEC 60664-1.",
  "type": "string",
  "enum": ["I", "II", "III", "IV"]
}
```

## Migration policy

Identical to RFC 0007: introduce the new values alongside the old via
a `oneOf`, deprecate the old, remove at 1.0. The migration script
from RFC 0007 covers these mappings as well:

```
P1     -> PD1
P2     -> PD2
P3     -> PD3
(none) -> PD4
OVC-I  -> I
OVC-II -> II
OVC-III -> III
OVC-IV -> IV
```

## Cost

- MAS: trivial schema change.
- MKF: any consumer that compares the string value (e.g. switch
  statement on `pollutionDegree`) needs the new spelling. ~5 sites.
- Bundled `data/*.ndjson`: probably no records use these fields
  directly (they're design-requirement fields, not catalogue fields),
  but verify in a one-pass scan.

## Why this is a separate RFC

The casing convention (RFC 0007) is a pure cosmetic/style choice. This
RFC fixes a *normative* error: MAS currently misnames IEC 60664
categories. That's a different class of defect and deserves its own
acceptance vote.

## Open questions

1. Should `cti` group names also adopt `groupI` / `groupII` /
   `groupIIIA` / `groupIIIB` per RFC 0007's casing sweep? Yes —
   handled by RFC 0007.
2. Is there an analogous misnaming in any insulation-related field I
   haven't checked? The schema review didn't find any, but the
   bundled `data/` should be scanned.

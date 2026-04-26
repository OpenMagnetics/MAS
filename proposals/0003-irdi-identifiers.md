# MAS-RFC 0003 — IRDI identifiers via IEC CDD

- **Status:** Draft
- **Type:** Additive (target: 0.2.0)
- **Author:** _to be assigned_
- **Created:** 2026-04-26

## Summary

Carry an optional **IRDI** (International Registration Data Identifier,
ISO/IEC 11179-6 / ISO 29002-5) on every catalogue record (core shape,
core material, wire, bobbin) alongside the human-readable name. Open a
parallel work-stream with **IEC TC 3** to register MAS classes in the
**IEC Common Data Dictionary** (https://cdd.iec.ch) so that the IRDIs
have a real authority behind them.

## Motivation

Today MAS identifies catalogue items by string:

- `material: "N97"` (TDK trade name)
- `shape: "ETD 34/17/11"` (IEC 60401 dimensional designation)
- `wire: "Litz 100x0.1 - Grade 1"` (constructed string)

This is fragile across translations, vendor renaming, and downstream
databases. A globally-unique identifier issued by an authority (IEC CDD)
unblocks:

- Interoperability with **ECLASS** and **AAS / Industry 4.0 digital
  twins**, both IRDI-native.
- Citation by `IRDI` in IEC 60205 / 62317 / 63093 documents that already
  use the CDD.
- Lossless round-tripping through procurement / PLM systems.

## Proposal

### Schema changes

Add to `schemas/utils.json`:

```json
"IRDI": {
  "description": "International Registration Data Identifier per ISO/IEC 11179-6, format `RAI#DI#VI` where RAI is the registration authority identifier (e.g. `0112` for IEC CDD), DI the data identifier and VI the version identifier.",
  "type": "string",
  "pattern": "^[0-9A-Za-z]{4}(?:-[0-9A-Za-z]+)*#[0-9A-Za-z-]+#[0-9A-Za-z.-]+$",
  "examples": [
    "0112/2///62317#ETD-34-17-11#001",
    "0112/2///60404#FERRITE-N97#002"
  ]
}
```

Wire it into the `manufacturerInfo` block (and equivalent on shape /
wire / bobbin) as an optional sibling of `name`:

```json
"identifier": { "$ref": "./utils.json#/$defs/IRDI" }
```

### Registry strategy

MAS does **not** mint its own IRDIs. Two paths to authoritative IDs:

1. **IEC CDD (preferred)** — open a TC 3 / SC 3D liaison to register MAS
   shape and material classes under the existing IEC 61360-4 hierarchy.
   PSMA Magnetics Committee provides the technical content; TC 3 issues
   the IRDIs.
2. **ECLASS fallback** — for items IEC CDD won't accept (manufacturer-
   specific ferrite grades), use ECLASS, which mints IRDIs with RAI
   `0173-1`.

For records with no authoritative IRDI yet, leave the field absent. Do
**not** invent a private RAI; doing so poisons the namespace.

### Bundled-database update

`data/core_shapes.ndjson`, `data/core_materials.ndjson`,
`data/wires.ndjson`, `data/bobbins.ndjson` gain an optional `irdi` field
on each record. Initial population: leave empty pending the TC 3
dialogue. Add a `scripts/check_irdi_unique.py` to enforce uniqueness when
populated.

## Migration policy

- 0.2.0: schema gains `IRDI`; database may carry it; nothing required.
- 1.0.0+: not required even at 1.0; IRDIs are populated as the CDD
  registry catches up. Tooling that needs hard guarantees (procurement
  integrations) can refuse records lacking an IRDI on its own policy.

## Open questions

1. Who funds and staffs the TC 3 liaison? PSMA Magnetics Committee is
   the natural answer but needs a named person.
2. Should IRDIs be mandatory for core-shape records that correspond to
   IEC 62317 / 63093 normative dimensions? Leaning yes at 1.1.0 once
   coverage is sufficient.
3. How are obsolete records handled — IRDI versioning suffix or a
   separate `lifecycleStatus` field? Probably both: VI bump for revision,
   `lifecycleStatus` for production / obsolete / withdrawn (already
   partially modelled in `manufacturerInfo.status`).

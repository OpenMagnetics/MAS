# MAS-RFC 0005 — Permeability variants (incremental and reversible)

- **Status:** Implemented (schema landed)
- **Type:** Additive (target: 0.2.0)
- **Author:** _to be assigned_
- **Created:** 2026-04-26

## Summary

Add two optional permeability slots to `core/material.json`:
**incremental** (μΔ) and **reversible** (μᵣₑᵥ). Tighten the prose on
the existing `initial`, `amplitude` and `complex` slots so each cites
the IEC / IEV measurement convention it refers to. Pure addition; no
existing slot changes type or becomes required.

## Motivation

A scan of vendor datasheets (TDK N87, Ferroxcube 3C95, FDK 7H10)
confirms that ferrite materials publish at minimum three distinct
permeability quantities:

- **Initial μᵢ** — at low signal (B < 0.25 mT for TDK), low frequency
  (≤10 kHz), reference temperature.
- **Amplitude μₐ** — vs AC flux density, on a defined geometry (R34
  toroid for TDK), pinned frequency and temperature.
- **Reversible μᵣₑᵥ** — the limit of incremental permeability as the AC
  excitation tends to zero, plotted vs DC bias H.
- **Incremental μΔ** — small AC swing superimposed on a non-zero DC
  bias.

MAS today represents `initial`, `amplitude` and `complex` (μ' + jμ'').
A scalar reading "permeability: 2300" is therefore already
disambiguated by which slot it lives in — *for those three slots*. What
is missing is a slot for `incremental` and `reversible`, which are the
quantities that govern inductor design under DC bias (every PFC choke,
every output filter, every flyback primary). Today a tool can only
record those quantities by abusing the `amplitude` slot or by losing
them altogether.

The operating-point context per measurement (frequency, temperature,
magneticFluxDensityPeak, magneticFieldDcBias) already exists on
`permeabilityPoint`. This RFC does not change that — it just adds two
more slots that consume the same point type.

## Proposal

In `schemas/magnetic/core/material.json`, the `permeability` block
becomes:

```json
"permeability":{
  "description": "Relative permeability of the magnetic material, broken down by the quantity that was measured. Each quantity is optional; at minimum `initial` is required for materials that have one (essentially all ferrites and tape-wound alloys).",
  "title": "permeabilities",
  "type": "object",
  "properties": {
    "initial":     { "$ref": "#/$defs/permeability",
                     "description": "Initial relative permeability mu_i, measured at near-zero signal (e.g. B < 0.25 mT) and low frequency (typically <= 10 kHz) at the material's reference temperature. Per IEC 60401-3 catalogue convention." },
    "amplitude":   { "$ref": "#/$defs/permeability",
                     "description": "Amplitude relative permeability mu_a, the secant slope of the BH curve at a specified peak AC flux density and a defined geometry. Each point pins B_peak, frequency and temperature." },
    "incremental": { "$ref": "#/$defs/permeability",
                     "description": "Incremental relative permeability mu_delta, the slope of a small AC excursion superimposed on a non-zero DC bias H. Each point pins magneticFieldDcBias and the AC magneticFluxDensityPeak. Required to model inductors operating under DC bias." },
    "reversible":  { "$ref": "#/$defs/permeability",
                     "description": "Reversible relative permeability mu_rev, the limit of mu_delta as the AC excursion tends to zero. Tabulated vs magneticFieldDcBias. Used for small-signal AC analysis around a DC operating point." },
    "complex":     { "description": "Complex relative permeability (mu' + j*mu''). Used for high-frequency loss modelling.",
                     "title": "complexPermeabilityData",
                     "type": "object",
                     "properties": {
                       "real":      { "$ref": "#/$defs/permeability" },
                       "imaginary": { "$ref": "#/$defs/permeability" }
                     },
                     "required": ["real", "imaginary"] }
  },
  "required": ["initial"]
}
```

Effective permeability (μₑ) is intentionally **not** added here. It is
a property of the gapped *core* (depends on geometry), not of the
*material*, and is reported in the `core` block alongside the gap
description.

## Migration policy

- **0.2.0:** add `incremental` and `reversible` slots. Tighten existing
  prose. Non-breaking.
- The existing requirement that `initial` be present is retained.
- `incremental` and `reversible` are populated only as catalogue data
  permits; tools that don't read them are unaffected.

## What this is not

This RFC does not address:

- Frequency-dependent permeability surfaces beyond what the existing
  `permeabilityPoint` array already supports — that surface is already
  rich enough.
- The "loss-method volumetric vs mass" issue (RFC scope: see the
  discussion notes; resolved by description tightening in the
  current schema cleanup).
- The "inductance operating-point context" issue at the *output* level,
  resolved non-breaking by adding a `measurementCondition` block to
  `outputs.magnetizingInductance` in the current cleanup.

## Open questions

1. Should `permeabilityPoint` gain an explicit `kind` field that
   restates which permeability variant the point belongs to (defensive,
   in case a point gets copied out of context)? Recommendation: no —
   the slot a point lives in is already the discriminator, and adding
   `kind` invites disagreement between slot and field.
2. Is `effective` permeability worth adding here despite being a core
   property, for the case of pre-gapped catalogue cores published with
   their μₑ rather than computed? Probably yes — but as a `core`
   property, not a material property. Out of scope here.

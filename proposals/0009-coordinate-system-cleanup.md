# MAS-RFC 0009 — Coordinate-system cleanup (polar / cylindrical)

- **Status:** Implemented in commit (forthcoming)
- **Type:** Additive (one schema-text correction)
- **Author:** _to be assigned_
- **Created:** 2026-04-26
- **Revised:** 2026-04-26 — `spherical` replaced by `cylindrical`; spherical
  coordinates are not used in magnetic-component description.

## Summary

Resolve a small but real internal contradiction in MAS's coordinate
handling: the `coordinateSystem` enum offers `cartesian` and `polar`,
but the `coordinates` definition documents 3D coordinates as
*spherical*. Polar is 2D, spherical is 3D — and neither is what
magnetics actually uses for 3D. The natural 3D system for toroidal
cores is **cylindrical** `(r, theta, z)`.

## Motivation

`schemas/utils.json:410` (pre-fix):

```json
"coordinateSystem": {
  "enum": ["cartesian", "polar"]
}
```

`schemas/utils.json:204` (pre-fix):

```json
"coordinates": {
  "description": "Data describing coordinates in 2 or 3 dimensional
                  space. In case of cartesian coordinates, they
                  represent x, y and z; in case of spherical, they
                  represent r, tetha and gamma, in mathematics
                  convention",
  "type": "array",
  "minItems": 2,
  "maxItems": 3
}
```

Issues:

1. The enum says **polar** but the description says **spherical**.
   They are different things (2D vs 3D).
2. **Spherical coordinates are not used in magnetics.** The 3D system
   that actually maps to a magnetic component is **cylindrical**:
   toroidal cores have a natural `(r, theta, z)` parameterisation,
   where `r` is the radius from the axis, `theta` the angle around
   the toroid, `z` the axial position. The `radialWindingWindow` in
   `core.json` is already implicitly cylindrical (it uses `angle` and
   `radialHeight`).
3. Typos: "tetha" for **theta**, "gamma" for **phi** (in the
   spherical convention). Resolved by removing the spherical wording
   altogether.
4. `coil.json` redeclares the same `coordinateSystem` enum inline at
   four sites (`turn`, `layer`, `group`, `section`).

## Resolution (applied in this commit)

### 1. Add `cylindrical`, drop the spherical wording

```json
"coordinateSystem": {
  "type": "string",
  "enum": ["cartesian", "polar", "cylindrical"],
  "default": "cartesian",
  "description": "Cartesian: (x, y) or (x, y, z).
                  Polar: (r, theta) — 2D only.
                  Cylindrical: (r, theta, z) — 3D only,
                  natural for toroidal cores.
                  Spherical is not used in
                  magnetic-component description."
}
```

### 2. Fix `coordinates` description

```json
"coordinates": {
  "description": "Coordinates in 2D or 3D space. Interpretation
                  depends on the sibling coordinateSystem (default:
                  cartesian). Cartesian: (x, y) or (x, y, z).
                  Polar (2D only): (r, theta).
                  Cylindrical (3D only): (r, theta, z) — natural for
                  toroidal geometry.",
  ...
}
```

### 3. Sweep coil.json local re-declarations

The four inline `coordinateSystem` enums in `coil.json` (`turn:138`,
`layer:211`, `group:264`, `section:321`) are updated to include
`cylindrical`. A future cleanup may replace them with `$ref` to the
shared `utils.json#/$defs/coordinateSystem` definition.

## Migration policy

Adding `cylindrical` to the enum is **additive**: every document that
validated before still validates. No instance document is invalidated.

A document that previously declared `"polar"` with three coordinates
was implicitly working in cylindrical-shaped data. Strictly the
document remains valid; the spec now says it should re-declare as
`"cylindrical"`. Since `polar` with three coordinates is incoherent
JSON-Schema-wise (`maxItems: 3` accepts it but `polar` semantically
means 2D), no real document is expected to be in that state.

## Cost

- MAS schema: small (two utils edits + four coil enum sweeps).
- MKF: needs to handle the new `"cylindrical"` enum value where it
  switches on `coordinateSystem`. Likely two or three sites.
- Catalogue data (`data/`): no records use these fields directly;
  `coordinateSystem` is for geometry not catalogue.

## Open question

Should `dimensions` (which is also a 2 or 3 element array) get the
same treatment? Yes — it has the same ambiguity. Folded in here
implicitly by sharing `coordinateSystem` semantics, but a future RFC
might tighten with `if/then` (if `coordinateSystem == "polar"` then
`maxItems: 2`).

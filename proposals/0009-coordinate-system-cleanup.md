# MAS-RFC 0009 — Coordinate-system cleanup (polar vs spherical)

- **Status:** Draft
- **Type:** Mostly additive (one breaking item: enum value)
- **Author:** _to be assigned_
- **Created:** 2026-04-26

## Summary

Resolve a small but real internal contradiction in MAS's coordinate
handling: the `coordinateSystem` enum offers `cartesian` and `polar`,
but the `coordinates` definition documents 3D coordinates as
*spherical*. Polar is 2D, spherical is 3D — these are different.

## Motivation

`schemas/utils.json:410`:

```json
"coordinateSystem": {
  "enum": ["cartesian", "polar"]
}
```

`schemas/utils.json:204`:

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

1. `coordinateSystem` enum says **polar** but `coordinates`
   description says **spherical**. Polar is the 2D version
   `(r, theta)`; spherical is the 3D version `(r, theta, phi)`.
   They are not synonyms.
2. `coordinates` accepts 2 or 3 elements with no way to declare which
   form is intended. A consumer reading `[1.0, 0.5, 2.0]` doesn't
   know whether that's `(x, y, z)`, `(r, theta, phi)` or some other
   combination.
3. Typos: "tetha" for "theta", "gamma" for "phi" (the third spherical
   coordinate is conventionally `phi`, not `gamma`; `gamma` is sometimes
   used for the rotation around the Z axis, which is confusing).
4. The `coordinateSystem` field appears in `turn`, `layer`, `group`,
   `section` but is not enforced consistently — a coordinate value
   without a sibling `coordinateSystem` is implicitly cartesian.

## Proposal

### 1. Extend the enum

```json
"coordinateSystem": {
  "type": "string",
  "enum": ["cartesian", "polar", "spherical"],
  "default": "cartesian",
  "description": "Coordinate system used for sibling `coordinates` and `dimensions` arrays. Cartesian: (x, y) or (x, y, z). Polar: (r, theta) — 2D only. Spherical: (r, theta, phi) — 3D only, mathematics convention with theta as the polar angle from +Z and phi as the azimuthal angle in the XY plane."
}
```

This is **breaking** for the enum: a document that previously had
`"coordinateSystem": "polar"` and three coordinates was relying on the
older "polar means spherical when 3D" convention. After this RFC, such
a document should declare `"spherical"`.

### 2. Fix `coordinates` description

```json
"coordinates": {
  "description": "Coordinates in 2D or 3D space. Interpretation depends on the sibling coordinateSystem value (default: cartesian). Cartesian: (x, y) or (x, y, z). Polar (2D only): (r, theta). Spherical (3D only): (r, theta, phi) per mathematics convention.",
  "type": "array",
  "minItems": 2,
  "maxItems": 3
}
```

Replace "tetha" -> "theta" (typo) and "gamma" -> "phi"
(canonical name) wherever they appear.

### 3. Fix the local re-declarations

`coil.json` redeclares the same `coordinateSystem` enum inline at
lines 134-139, 207-212, 260-265, 317-322. Replace each with a
`$ref` to `utils.json#/$defs/coordinateSystem`.

`core.json` `windingWindow` declarations don't use the enum but do
use `coordinates`; check that descriptions are consistent.

## Migration policy

- **0.2.0:** add `spherical` to the enum (additive). The old
  description is fixed (cosmetic). Existing documents that set
  `"polar"` with 3 coordinates remain valid (no validator can flag
  them in JSON Schema 2020-12 without much more complex
  `if/then/else`), but the spec now says they should re-declare as
  `"spherical"`.
- **Optional 1.0 tightening:** add an `if/then` rule that if
  `coordinateSystem == "polar"` then `coordinates.maxItems = 2`,
  and `spherical` requires `minItems = 3`. Useful but adds schema
  complexity; could defer further.

## Cost

- MAS: small. Two enum/description changes plus four local-redecl
  cleanups in `coil.json`.
- MKF: any consumer that switches on `coordinateSystem` needs to
  handle `"spherical"`. Likely a couple of sites.
- Catalogue data: bundled `data/` shouldn't contain coordinates with
  spherical convention currently (everything is cartesian core
  geometry); verify.

## Open questions

1. Is "spherical" actually used anywhere in MAS today? The toroidal
   winding-window definition uses `radial` shape and angles — that's
   already a third coordinate convention. Should `coordinateSystem`
   gain a `cylindrical` value for the toroid case? Probably yes;
   `(r, theta, z)` is the natural toroid coordinate system. Defer to
   a follow-up.
2. Should `dimensions` (which is also a 2 or 3 element array) get the
   same treatment? Yes — it's the same problem, same fix.

# MAS — Conformance classes

This document describes the three conformance classes defined by MAS:
**A — Inductor Basic**, **B — Transformer**, **C — Full**. The
conformance class an implementation supports tells consumers which
subset of the MAS schema the implementation can be relied on to
produce or interpret correctly.

The ratifying specification is RFC 0002 (`proposals/0002-conformance-classes.md`).

A document declares its target class with the optional `masConformance`
field at the root:

```json
{
    "masVersion": "0.2.0",
    "masConformance": "B",
    "inputs":   { ... },
    "magnetic": { ... },
    "outputs":  []
}
```

Validation against the corresponding bundle under
`schemas/conformance/class-{A,B,C}.json` enforces the class
requirements; validation against the base `schemas/MAS.json` does not.

## Class A — Inductor Basic

A simple inductor for a non-isolated converter topology.

| Required | What it covers |
|----|----|
| `inputs.designRequirements.magnetizingInductance` | The single design parameter that defines an inductor. |
| `magnetic.core` and `magnetic.coil` | A core and at least one winding. |
| `magnetic.coil.functionalDescription` with at least one element | The single winding. |

Typical topologies served at Class A: buck inductor, boost inductor,
single-output filter choke. A Class A document may use any of the MAS
fields; Class A only specifies the *minimum* a conformant document
must contain.

## Class B — Transformer

A multi-winding magnetic for an isolated converter or coupled-inductor
topology.

| Required | What it covers |
|----|----|
| Class A requirements | Inductance, core, coil. |
| `inputs.designRequirements.turnsRatios` | The defining parameter set of a transformer. |
| `magnetic.coil.functionalDescription` with at least two elements | Primary + secondary as a minimum. |

Class B implementations are expected to additionally handle bobbins
(`magnetic.coil.bobbin`) and insulation requirements
(`inputs.designRequirements.insulation`), although these are not
hard-required at the schema level since some Class B parts are open
(no bobbin) or low-voltage (no formal insulation block).

Typical topologies served at Class B: flyback, forward (single- /
two-switch / active-clamp), push-pull, isolated buck, isolated
buck-boost, current transformer.

## Class C — Full

Every schema field, every topology, every output type. Class C is
what a fully featured magnetics-design tool targets — typically one
that performs detailed loss, thermal and insulation analyses.

Class C adds no new required fields beyond the base MAS schema; it is
the **promise** that the implementation can produce or interpret any
valid MAS document, including:

- All 16 topology schemas under `inputs/topologies/`.
- The full per-turn / per-layer / per-section coil description.
- Every output type: core losses, winding losses (with skin and
  proximity effect breakdown), inductance with operating-point
  context, impedance matrix, stray capacitance, insulation
  coordination, temperature distribution.
- Custom permeability and loss-method blocks for any vendor.

## Validating against a class

Use any JSON Schema 2020-12 validator with the appropriate bundle as
the root. The bundles `allOf`-import `MAS.json`, so all base-schema
constraints apply in addition to the class-specific requirements.

## How a tool declares its class

Tools that produce MAS documents should set `masConformance` to the
*highest* class they will guarantee for that document. A Class C tool
that emits an inductor may legitimately set `"A"` if the document
truly only uses Class A fields, or `"C"` if any Class C field is
present.

Tools that consume MAS documents should reject (or warn about) any
document whose declared class exceeds what they implement.

## Versioning

The class definitions evolve under the `GOVERNANCE.md` MAJOR-change
process. A change that tightens a class (adds a new required field) is
breaking and requires a MAJOR bump. A change that loosens a class
(removes a required field) is non-breaking. A change that adds an
entirely new class (e.g. Class D — Manufacturer Catalogue) is
additive.

## Test vectors

The samples under `samples/complete/` are not yet partitioned by
class. A future PR will move them into per-class buckets
(`tests/conformance/class-A/`, etc.) so that conformance can be
exercised mechanically. Until then the bundles can be applied
manually to the existing samples.

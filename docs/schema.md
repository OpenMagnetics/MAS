# MAS Schema Documentation

> **Informative.** This document is a structural guide to the MAS JSON
> Schema; it does not add normative requirements. For the normative
> units table see [`units.md`](units.md); for the mapping of MAS to
> existing standards (IEC, ASTM, MPIF, JIS, NEMA, IEV vocabulary) see
> [`normative-references.md`](normative-references.md).

This document describes the JSON Schema structure for MAS (Magnetic
Agnostic Structure).

## Schema Hierarchy

```
MAS.json (root)
├── inputs.json
│   ├── designRequirements.json
│   │   └── (insulation, tolerances, etc.)
│   ├── operatingPoint.json
│   │   ├── operatingConditions.json
│   │   └── operatingPointExcitation.json
│   └── topologies/
│       ├── buck.json
│       ├── flyback.json
│       └── ...
├── magnetic.json
│   ├── core.json
│   │   ├── functionalDescription (shape, material, gapping)
│   │   ├── geometricalDescription (detailed geometry)
│   │   └── processedDescription (computed values)
│   ├── coil.json
│   │   ├── functionalDescription (windings, turns)
│   │   ├── layersDescription
│   │   └── turnsDescription
│   ├── wire.json
│   │   ├── round/
│   │   ├── litz/
│   │   ├── rectangular/
│   │   └── foil/
│   └── bobbin.json
└── outputs.json
    ├── coreLosses
    ├── windingLosses
    ├── magnetizingInductance
    └── leakageInductance
```

## Required vs Optional Fields

### MAS Root (required: all three)
```json
{
  "inputs": { ... },      // Required
  "magnetic": { ... },    // Required
  "outputs": [ ... ]      // Required (can be empty array)
}
```

### Inputs (required: designRequirements, operatingPoints)
```json
{
  "designRequirements": {
    "magnetizingInductance": { ... },  // Required
    "turnsRatios": [ ... ],            // Required (empty for inductor)
    "topology": "string",              // Required
    "name": "string",                  // Optional
    "operatingTemperature": { ... },   // Optional
    "insulation": { ... },             // Optional
    "maximumDimensions": [x, y, z],    // Optional
    "maximumWeight": number,           // Optional
    "market": "string",                // Optional
    "terminalType": [ ... ]            // Optional
  },
  "operatingPoints": [                 // Required, min 1 item
    {
      "name": "string",                // Optional
      "conditions": {                  // Required
        "ambientTemperature": number   // Required
      },
      "excitationsPerWinding": [ ... ] // Required, one per winding
    }
  ]
}
```

### Magnetic (required: core, coil)
```json
{
  "name": "string",                    // Optional
  "core": {
    "name": "string",                  // Optional
    "functionalDescription": {         // Required
      "type": "string",                // Required: "two-piece set", "toroidal", etc.
      "material": "string",            // Required: material name
      "shape": "string",               // Required: shape name
      "gapping": [ ... ],              // Required: array (empty for ungapped)
      "numberStacks": number           // Optional, default 1
    }
  },
  "coil": {
    "bobbin": "string",                // Optional
    "functionalDescription": [         // Required, one object per winding
      {
        "name": "string",              // Optional
        "numberTurns": number,         // Required
        "numberParallels": number,     // Required
        "wire": "string",              // Required: wire name
        "isolationSide": "string"      // Optional: "primary", "secondary", etc.
      }
    ]
  }
}
```

## Enumeration Values

### Core Types
- `"two-piece set"` - E, ETD, PQ, RM cores
- `"toroidal"` - Ring/toroid cores
- `"closed shape"` - Pot cores
- `"piece and plate"` - Core with flat plate

### Gapping Types
- `"subtractive"` - Ground gap (reduces core height)
- `"additive"` - Spacer gap (increases core height)
- `"residual"` - Unavoidable gap at mating surfaces
- `"distributed"` - Air gap distributed in magnetic path

### Waveform Labels
- `"triangular"` - Triangular wave (inductor current)
- `"rectangular"` - Rectangular/square wave
- `"sinusoidal"` - Sine wave
- `"flybackPrimary"` - Flyback primary current shape
- `"flybackSecondary"` - Flyback secondary current shape
- `"custom"` - User-defined with data points

### Insulation Types (IEC 60664-1 §4.1)
- `"functional"` - Basic operation only, no protection against shock
- `"basic"` - Single level of protection against electric shock
- `"supplementary"` - Independent insulation additional to basic
- `"double"` - Basic insulation plus supplementary insulation
- `"reinforced"` - Single insulation system equivalent to double

### Overvoltage Categories (IEC 60664-1 §4.3)
- `"I"` - Low transient overvoltage (protected equipment)
- `"II"` - Household appliances connected to fixed installation
- `"III"` - Fixed installation equipment
- `"IV"` - Equipment at the origin of the installation

### Pollution Degrees (IEC 60664-1 §4.2)
- `"PD1"` - No pollution or only dry, non-conductive pollution
- `"PD2"` - Non-conductive pollution; temporary conductivity possible
- `"PD3"` - Conductive pollution, or dry non-conductive that becomes conductive due to condensation
- `"PD4"` - Persistent conductive pollution (e.g. conductive dust or rain)

### CTI Groups (IEC 60112)
- `"groupI"` - CTI ≥ 600
- `"groupII"` - 400 ≤ CTI < 600
- `"groupIIIA"` - 175 ≤ CTI < 400
- `"groupIIIB"` - 100 ≤ CTI < 175

### Markets
- `"commercial"`, `"industrial"`, `"medical"`, `"military"`, `"space"`

### Topologies
- `"buckConverter"`, `"boostConverter"`, `"invertingBuckBoostConverter"`
- `"flybackConverter"`, `"activeClampForwardConverter"`, `"singleSwitchForwardConverter"`, `"twoSwitchForwardConverter"`
- `"pushPullConverter"`, `"halfBridgeConverter"`, `"fullBridgeConverter"`
- `"phaseShiftedFullBridgeConverter"`, `"phaseShiftedHalfBridgeConverter"`
- `"llcResonantConverter"`, `"cllcResonantConverter"`, `"dualActiveBridgeConverter"`
- `"currentTransformer"`, `"commonModeChoke"`, `"differentialModeChoke"`
- `"isolatedBuckConverter"`, `"isolatedBuckBoostConverter"`, `"powerFactorCorrection"`

## DimensionWithTolerance Pattern

Used throughout MAS for numeric values with tolerances:

```json
// At least one of these is required:
{
  "nominal": number,        // Typical/target value
  "minimum": number,        // Lower bound
  "maximum": number,        // Upper bound
  "excludeMinimum": bool,   // true = > not >=
  "excludeMaximum": bool    // true = < not <=
}
```

Examples:
```json
// Just nominal
{"nominal": 100e-6}

// With ±10% tolerance
{"nominal": 100e-6, "minimum": 90e-6, "maximum": 110e-6}

// Maximum only
{"maximum": 0.001}
```

## Validation

The schemas use JSON Schema Draft 2020-12. Validate with:

```python
import json
import jsonschema
from jsonschema import Draft202012Validator

# Load schema
with open("schemas/MAS.json") as f:
    schema = json.load(f)

# Create validator with reference resolution
resolver = jsonschema.RefResolver(
    base_uri="file:///path/to/MAS/schemas/",
    referrer=schema
)
validator = Draft202012Validator(schema, resolver=resolver)

# Validate document
with open("my_design.json") as f:
    data = json.load(f)

validator.validate(data)
```

## File Locations

| File | Purpose |
|------|---------|
| `schemas/MAS.json` | Root schema |
| `schemas/inputs.json` | Inputs section |
| `schemas/magnetic.json` | Magnetic section |
| `schemas/outputs.json` | Outputs section |
| `schemas/inputs/*.json` | Input sub-schemas |
| `schemas/magnetic/*.json` | Magnetic sub-schemas |
| `schemas/outputs/*.json` | Output sub-schemas |
| `data/*.ndjson` | Component databases |

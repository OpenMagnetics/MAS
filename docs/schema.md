# MAS Schema Documentation

This document describes the JSON Schema structure for MAS (Magnetic Agnostic Structure).

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
- `"Triangular"` - Standard triangular wave
- `"Rectangular"` - Square/rectangular wave
- `"Sinusoidal"` - Sine wave
- `"Flyback Primary"` - Flyback primary current shape
- `"Flyback Secondary"` - Flyback secondary current shape
- `"Custom"` - User-defined with data points

### Insulation Types
- `"Functional"` - Basic operation only
- `"Basic"` - Single level of protection
- `"Supplementary"` - Additional protection
- `"Double"` - Basic + Supplementary
- `"Reinforced"` - Single system equivalent to double

### Overvoltage Categories
- `"OVC-I"` - Low transient overvoltage
- `"OVC-II"` - Household appliances
- `"OVC-III"` - Fixed installation equipment
- `"OVC-IV"` - Equipment at origin of installation

### Markets
- `"Commercial"`
- `"Industrial"`
- `"Medical"`
- `"Military"`
- `"Space"`

### Topologies
- `"Buck"`, `"Boost"`, `"BuckBoost"`
- `"Flyback"`, `"Forward"`
- `"PushPull"`, `"HalfBridge"`, `"FullBridge"`
- `"LLC"`, `"DAB"`
- `"CurrentTransformer"`, `"CommonModeChoke"`

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

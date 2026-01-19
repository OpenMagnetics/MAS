# MAS Quick Reference

## Minimal Inductor

```json
{
  "inputs": {
    "designRequirements": {
      "magnetizingInductance": {"nominal": 100e-6},
      "turnsRatios": [],
      "topology": "Buck Converter"
    },
    "operatingPoints": [{
      "conditions": {"ambientTemperature": 25},
      "excitationsPerWinding": [{
        "frequency": 100000,
        "current": {"processed": {"label": "Triangular", "peakToPeak": 1.0, "offset": 5.0, "dutyCycle": 0.5}},
        "voltage": {"processed": {"label": "Rectangular", "peakToPeak": 12.0, "offset": 0, "dutyCycle": 0.5}}
      }]
    }]
  },
  "magnetic": {
    "core": {"functionalDescription": {"type": "two-piece set", "material": "N97", "shape": "E 42/21/15", "gapping": [{"type": "subtractive", "length": 0.001}], "numberStacks": 1}},
    "coil": {"bobbin": "E 42/21/15", "functionalDescription": [{"name": "Primary", "numberTurns": 20, "numberParallels": 1, "isolationSide": "primary", "wire": "Round 1.0 - Grade 1"}]}
  },
  "outputs": []
}
```

## Minimal Transformer (2 windings)

```json
{
  "inputs": {
    "designRequirements": {
      "magnetizingInductance": {"nominal": 500e-6},
      "turnsRatios": [{"nominal": 0.1}],
      "topology": "Flyback Converter"
    },
    "operatingPoints": [{
      "conditions": {"ambientTemperature": 25},
      "excitationsPerWinding": [
        {"name": "Primary", "frequency": 100000, "current": {"processed": {"label": "Triangular", "peakToPeak": 1.0, "offset": 0.5, "dutyCycle": 0.4}}, "voltage": {"processed": {"label": "Rectangular", "peakToPeak": 325, "offset": 0, "dutyCycle": 0.4}}},
        {"name": "Secondary", "frequency": 100000, "current": {"processed": {"label": "Triangular", "peakToPeak": 10.0, "offset": 5.0, "dutyCycle": 0.6}}, "voltage": {"processed": {"label": "Rectangular", "peakToPeak": 24, "offset": 0, "dutyCycle": 0.6}}}
      ]
    }]
  },
  "magnetic": {
    "core": {"functionalDescription": {"type": "two-piece set", "material": "N97", "shape": "ETD 34/17/11", "gapping": [{"type": "subtractive", "length": 0.0005}], "numberStacks": 1}},
    "coil": {"bobbin": "ETD 34/17/11", "functionalDescription": [
      {"name": "Primary", "numberTurns": 50, "numberParallels": 1, "wire": "Litz 20x0.1 - Grade 1", "isolationSide": "primary"},
      {"name": "Secondary", "numberTurns": 5, "numberParallels": 2, "wire": "Litz 100x0.1 - Grade 1", "isolationSide": "secondary"}
    ]}
  },
  "outputs": []
}
```

---

## Core Shapes (exact names required)

| Family | Examples |
|--------|----------|
| E | `"E 19/8/5"`, `"E 42/21/15"`, `"E 55/28/21"`, `"E 65/32/27"` |
| ETD | `"ETD 29/16/10"`, `"ETD 34/17/11"`, `"ETD 49/25/16"` |
| EFD | `"EFD 15/8/5"`, `"EFD 20/10/7"`, `"EFD 25/13/9"` |
| PQ | `"PQ 20/16"`, `"PQ 26/25"`, `"PQ 35/35"`, `"PQ 40/40"` |
| RM | `"RM 6"`, `"RM 8"`, `"RM 10"`, `"RM 12"`, `"RM 14"` |
| Toroid | `"T 20/10/7"`, `"T 40/24/16"`, `"T 58/41/18"` |
| EP | `"EP 7"`, `"EP 10"`, `"EP 13"`, `"EP 17"` |
| ER | `"ER 9.5/5"`, `"ER 14.5/6"`, `"ER 25/10.5"` |

## Core Materials

| Manufacturer | Low Loss HF | General | High Flux |
|--------------|-------------|---------|-----------|
| TDK/EPCOS | `"N49"`, `"N97"` | `"N87"`, `"N95"` | - |
| Ferroxcube | `"3C97"`, `"3F46"` | `"3C90"`, `"3C95"` | - |
| Magnetics | - | - | `"MPP"`, `"High Flux"`, `"XFlux"` |
| Micrometals | - | - | `"Mix 26"`, `"Mix 52"` |

## Gapping

```json
// Ungapped (toroid, powder core)
"gapping": []

// Single gap in center leg (E core)
"gapping": [
  {"type": "subtractive", "length": 0.001},
  {"type": "residual", "length": 0.00001},
  {"type": "residual", "length": 0.00001}
]

// Spacer gap
"gapping": [{"type": "additive", "length": 0.002}]
```

## Wire Types

| Type | Format | Example |
|------|--------|---------|
| Round | `Round <diameter_mm> - Grade <1-3>` | `"Round 1.0 - Grade 1"` |
| Litz | `Litz <strands>x<strand_dia_mm> - Grade <1-3>` | `"Litz 40x0.1 - Grade 1"` |
| Rectangular | `Rectangular <width>x<height> - Grade <1-3>` | `"Rectangular 2.0x0.5 - Grade 1"` |
| Foil | `Foil <thickness_mm>` | `"Foil 0.1"` |

## Waveforms

| Label | Use Case |
|-------|----------|
| `Triangular` | Inductor current, magnetizing current |
| `Rectangular` | Inductor voltage, transformer primary |
| `Sinusoidal` | LLC, resonant, AC applications |
| `Flyback Primary` | Flyback primary current |
| `Flyback Secondary` | Flyback secondary current |

## Processed Waveform

```json
{
  "processed": {
    "label": "Triangular",
    "peakToPeak": 2.0,      // Peak-to-peak amplitude (A or V)
    "offset": 10.0,         // DC offset (A or V)
    "dutyCycle": 0.5        // 0.0 to 1.0
  }
}
```

## Topologies

- `Buck Converter`, `Boost Converter`, `Inverting Buck-Boost Converter`
- `Flyback Converter`, `Active Clamp Forward Converter`, `Single Switch Forward Converter`, `Two Switch Forward Converter`
- `Push-Pull Converter`, `Half-Bridge Converter`, `Full-Bridge Converter`
- `Isolated Buck Converter`, `Isolated Buck-Boost Converter`
- `Current Transformer`

## Units (SI)

| Quantity | Unit | Example |
|----------|------|---------|
| Inductance | H | `100e-6` = 100 µH |
| Frequency | Hz | `100000` = 100 kHz |
| Length/Gap | m | `0.001` = 1 mm |
| Temperature | °C | `40` = 40°C |
| Current | A | `10.0` = 10 A |
| Voltage | V | `400` = 400 V |

## Dimension With Tolerance

```json
// Just nominal
{"nominal": 100e-6}

// With limits
{"nominal": 100e-6, "minimum": 90e-6, "maximum": 110e-6}

// Just limits
{"minimum": 90e-6, "maximum": 110e-6}
```

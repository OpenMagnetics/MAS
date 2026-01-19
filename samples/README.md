# MAS Sample Files

This directory contains example MAS JSON files demonstrating individual magnetic component specifications.

For complete, ready-to-use MAS documents with all topologies, see [`examples/complete/`](../examples/complete/).

## Component Samples

Individual component definitions for reference:

### Inputs
- `inputs/designRequirements/` - Design requirement examples
- `inputs/operatingPointExcitation/` - Waveform and excitation examples

### Magnetic
- `magnetic/core/` - Core definitions (shapes, materials, assemblies)
- `magnetic/coil/` - Coil and winding definitions
- `magnetic/wire/` - Wire specifications (round, litz, foil, rectangular)
- `magnetic/bobbin/` - Bobbin definitions
- `magnetic/insulation/` - Insulation material specifications

## Usage

### Load in Python

```python
import json

with open("examples/complete/buck_inductor_complete.json") as f:
    mas = json.load(f)

print(f"Inductance: {mas['inputs']['designRequirements']['magnetizingInductance']['nominal'] * 1e6:.1f} µH")
print(f"Core: {mas['magnetic']['core']['functionalDescription']['shape']}")
print(f"Turns: {mas['magnetic']['coil']['functionalDescription'][0]['numberTurns']}")
```

### Validate Against Schema

```python
import json
import jsonschema

with open("schemas/MAS.json") as f:
    schema = json.load(f)

with open("examples/complete/buck_inductor_complete.json") as f:
    data = json.load(f)

jsonschema.validate(data, schema)
print("Valid MAS document!")
```

### Use with PyOpenMagnetics

```python
import PyOpenMagnetics
import json

with open("examples/complete/flyback_transformer_complete.json") as f:
    mas = json.load(f)

# Calculate core losses
losses = PyOpenMagnetics.calculate_core_losses(
    mas["magnetic"]["core"],
    mas["inputs"]["operatingPoints"][0],
    {"coreLosses": "IGSE"}
)
print(f"Core losses: {losses['coreLosses']:.3f} W")
```

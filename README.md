<p align="center">
  <img src="https://avatars.githubusercontent.com/u/99aborunda" alt="OpenMagnetics Logo" width="200"/>
</p>

<h1 align="center">MAS - Magnetic Agnostic Structure</h1>

<p align="center">
  <em>The universal data format for power magnetics design</em>
</p>

<p align="center">
  <a href="https://opensource.org/licenses/MIT"><img src="https://img.shields.io/badge/License-MIT-yellow.svg" alt="License: MIT"></a>
  <a href="https://json-schema.org/"><img src="https://img.shields.io/badge/JSON%20Schema-2020--12-blue.svg" alt="JSON Schema"></a>
  <a href="https://github.com/OpenMagnetics/MAS/actions"><img src="https://github.com/OpenMagnetics/MAS/actions/workflows/tests.yml/badge.svg" alt="Tests"></a>
</p>

---

## What is MAS?

**MAS is a standardized way to describe magnetic components**—transformers, inductors, and chokes—used in power electronics. Think of it as a universal language that precisely defines everything about a magnetic component: its core, windings, materials, and operating conditions.

### The Problem MAS Solves

As a power electronics engineer, you've probably experienced this:

1. You design a flyback transformer in your simulation tool
2. You export the specs to a spreadsheet to send to manufacturing
3. Manufacturing asks questions because some details are ambiguous
4. You re-enter the same data into thermal simulation software
5. Later, you can't remember which version of the design was actually built

**MAS eliminates this chaos.** One file contains everything:

```
┌─────────────────────────────────────────────────────────────────┐
│                         MAS Document                            │
├─────────────────────────────────────────────────────────────────┤
│  INPUTS (What you need)                                         │
│  ├── Inductance: 500 µH                                         │
│  ├── Topology: Flyback Converter                                │
│  ├── Switching frequency: 100 kHz                               │
│  ├── Input voltage: 24 V                                        │
│  ├── Output: 12 V @ 3 A                                         │
│  └── Operating temperature: 25-85°C                             │
│                                                                 │
│  MAGNETIC (What you build)                                      │
│  ├── Core: ETD 34/17/11, N97 ferrite                           │
│  ├── Primary: 42 turns, 0.5mm round wire                       │
│  ├── Secondary: 21 turns, 1.0mm round wire                     │
│  ├── Insulation: Triple-insulated wire, 3kV isolation          │
│  └── Bobbin: ETD 34, 12-pin                                    │
│                                                                 │
│  OUTPUTS (What you calculate)                                   │
│  ├── Core losses: 0.45 W                                        │
│  ├── Winding losses: 0.82 W                                     │
│  ├── Temperature rise: 35°C                                     │
│  └── Peak flux density: 0.28 T                                  │
└─────────────────────────────────────────────────────────────────┘
```

### Why This Matters for Power Electronics

| Your Current Workflow | With MAS |
|----------------------|----------|
| Specifications scattered across datasheets, emails, and spreadsheets | **One file** contains the complete component definition |
| Ambiguous drawings lead to manufacturing errors | **Machine-readable** format eliminates interpretation errors |
| Manual data entry between simulation tools | **Import/export** the same file across all compatible tools |
| "Which version did we actually build?" | **Version control** tracks every design change |
| Custom formats for each tool vendor | **Open standard** works with any software |

### Real-World Applications

MAS can describe any magnetic component in power electronics:

- **DC-DC Converters**: Buck, boost, flyback, forward, push-pull, full-bridge transformers and inductors
- **PFC Inductors**: Single and interleaved boost inductors
- **EMI Filters**: Common-mode and differential-mode chokes
- **Current Sensing**: Current transformers and shunts
- **Gate Drive**: Isolated gate drive transformers
- **Resonant Converters**: LLC and resonant tank inductors

---

## 📖 How It Works

MAS uses JSON (JavaScript Object Notation)—a simple text format that both humans and computers can read. You don't need programming experience to understand or edit a MAS file.

### The Three Sections

Every MAS document has three parts:

```
┌────────────────┐     ┌────────────────┐     ┌────────────────┐
│     INPUTS     │     │    MAGNETIC    │     │    OUTPUTS     │
├────────────────┤     ├────────────────┤     ├────────────────┤
│ What you NEED  │  +  │ What you BUILD │  =  │ What you GET   │
│                │     │                │     │                │
│ • Inductance   │     │ • Core shape   │     │ • Losses       │
│ • Topology     │     │ • Core material│     │ • Temperature  │
│ • Frequency    │     │ • Windings     │     │ • Efficiency   │
│ • Voltage/Curr │     │ • Wire type    │     │ • Flux density │
│ • Temperature  │     │ • Insulation   │     │ • Parasitics   │
└────────────────┘     └────────────────┘     └────────────────┘
```

**Why separate Inputs and Magnetic?** Because the same physical transformer can be used in different applications. A flyback transformer's core and windings (Magnetic) stay the same whether you run it at 80 kHz or 120 kHz (Inputs). MAS keeps these concerns separate so you can:

- Reuse magnetic designs across different operating conditions
- Compare how the same component performs in different applications
- Share component libraries independently of specific circuit designs

### Standard Components Built-In

MAS includes databases of real components you can reference by name:

```
Core Shapes:    "ETD 34", "PQ 26/25", "RM 12", "E 42/21/15", ...
Materials:      "N87", "N97", "3C95", "High Flux 60", "MPP 125", ...
Wire Types:     "Round 0.5 - Grade 2", "Litz 100x0.1", "Foil 0.1mm", ...
```

No need to manually enter all the dimensions and properties—just reference the standard name.

---

## 💡 Example: 500 kHz Buck Inductor

Here's what a complete MAS document looks like for a buck converter inductor:

```json
{
  "inputs": {
    "designRequirements": {
      "magnetizingInductance": {"nominal": 4.7e-6},
      "turnsRatios": [],
      "topology": "Buck Converter"
    },
    "operatingPoints": [{
      "conditions": {"ambientTemperature": 40},
      "excitationsPerWinding": [{
        "frequency": 500000,
        "current": {
          "processed": {
            "label": "Triangular",
            "peakToPeak": 3.0,
            "offset": 10.0,
            "dutyCycle": 0.275
          }
        },
        "voltage": {
          "processed": {
            "label": "Rectangular",
            "peakToPeak": 8.7,
            "offset": 0,
            "dutyCycle": 0.275
          }
        }
      }]
    }]
  },
  "magnetic": {
    "core": {
      "functionalDescription": {
        "type": "toroidal",
        "material": "High Flux 60",
        "shape": "T 10.0/6.0/4.5",
        "gapping": [],
        "numberStacks": 1
      }
    },
    "coil": {
      "bobbin": "T 10.0/6.0/4.5",
      "functionalDescription": [{
        "name": "Primary",
        "numberTurns": 8,
        "numberParallels": 3,
        "isolationSide": "primary",
        "wire": "Round 0.6 - Grade 1"
      }]
    }
  },
  "outputs": []
}
```

**What this describes:**
- A 4.7 µH inductor for a buck converter switching at 500 kHz
- Triangular current ripple of 3 A peak-to-peak with 10 A DC offset
- High Flux 60 powder core (low losses at high frequency)
- 8 turns of 0.6 mm wire, 3 parallel strands to reduce AC resistance
- Operating at 40°C ambient

---

## 🔧 Supported Topologies

MAS includes ready-to-use examples for all major power converter topologies:

### Non-Isolated DC-DC

| Topology | Component | Example |
|----------|-----------|---------|
| **Buck Converter** | Output inductor | 4.7µH, 500kHz, toroidal powder core |
| **Boost Converter** | Input inductor | 47µH, 200kHz, PQ ferrite core |
| **Buck-Boost** | Inductor | Coupled or uncoupled variants |

### Isolated DC-DC

| Topology | Component | Example |
|----------|-----------|---------|
| **Flyback** | Coupled inductor | 500µH, 100kHz, ETD gapped ferrite |
| **Forward (Single Switch)** | Transformer + reset winding | 400V→24V, 150kHz, RM core |
| **Forward (Two Switch)** | Transformer | 400V→5V, 250kHz, E core |
| **Forward (Active Clamp)** | Transformer | 400V→12V, 200kHz, RM core |
| **Push-Pull** | Center-tapped transformer | 48V→12V, 100kHz, ETD core |
| **Half/Full Bridge** | Transformer | High power, planar magnetics |

### Specialty Components

| Type | Application | Example |
|------|-------------|---------|
| **Current Transformer** | Sensing, protection | 100:1 ratio, 50Hz-100kHz |
| **Common Mode Choke** | EMI filtering | 2.2mH, 10A rated |
| **Gate Drive Transformer** | Isolated gate drive | High dV/dt immunity |

---

## 🗄️ Component Database

MAS comes with extensive databases of real components from major manufacturers:

### What's Included

| Category | Examples | Count |
|----------|----------|-------|
| **Core Shapes** | E, ETD, EFD, PQ, RM, PM, EP, toroid, planar | 500+ |
| **Ferrite Materials** | N87, N97, 3C90, 3C95, PC40, PC95 | 50+ |
| **Powder Materials** | High Flux, MPP, Kool Mu, XFlux, iron powder | 50+ |
| **Round Wire** | AWG 10-44, metric sizes, Grade 1/2/3 | 200+ |
| **Litz Wire** | Various strand counts and diameters | 300+ |
| **Foil & Rectangular** | Copper and aluminum foil conductors | 100+ |
| **Bobbins** | Matching all standard core shapes | 200+ |

All data includes:
- Dimensional information (with tolerances)
- Electrical properties (permeability, saturation, resistivity)
- Loss parameters (Steinmetz coefficients, temperature dependence)
- Manufacturer part numbers

---

## 📚 Documentation

| Document | Description |
|----------|-------------|
| **[Quick Reference](docs/quickref.md)** | Cheat sheet with minimal examples |
| **[Inputs Specification](docs/inputs.md)** | Design requirements and operating conditions |
| **[Magnetic Specification](docs/magnetic.md)** | Core, coil, and material definitions |
| **[Schema Reference](docs/schema.md)** | Complete JSON schema documentation |
| **[AI Reference](llms.txt)** | Comprehensive reference for AI assistants |

---

## 🔗 Software That Uses MAS

| Tool | Description |
|------|-------------|
| **PyOpenMagnetics** | Python library for loss calculation, thermal modeling, and design optimization |
| **MKF** | High-performance C++ simulation engine |

### Example: Calculate Losses with Python

```python
import PyOpenMagnetics
import json

# Load a MAS document
with open("flyback_transformer.json") as f:
    mas = json.load(f)

# Calculate core losses using IGSE model
core_losses = PyOpenMagnetics.calculate_core_losses(
    mas["magnetic"]["core"],
    mas["inputs"]["operatingPoints"][0],
    {"coreLosses": "IGSE"}
)

# Calculate winding losses including proximity effect
winding_losses = PyOpenMagnetics.calculate_winding_losses(
    mas["magnetic"],
    mas["inputs"]["operatingPoints"][0],
    {"windingLosses": "Dowell"}
)

print(f"Core losses:    {core_losses['coreLosses']:.2f} W")
print(f"Winding losses: {winding_losses['windingLosses']:.2f} W")
print(f"Total losses:   {core_losses['coreLosses'] + winding_losses['windingLosses']:.2f} W")
```

## 🏗️ Design Philosophy

### Separation of Inputs and Magnetic

A magnetic component's behavior depends equally on its construction and its excitation. The same physical component can function as a transformer in one converter or as an inductor in another. MAS keeps these concerns separate:

- **Inputs**: Design requirements and operating conditions
- **Magnetic**: Physical construction (core + coil)
- **Outputs**: Calculated results

### Reference Support (Indirection)

Properties can be specified directly or by reference:

```json
// By reference (standard component)
{"material": "N97"}

// Directly (custom component)
{"material": {"name": "Custom Ferrite", "permeability": 2000, ...}}
```

This allows mixing standard components (RM 12, N87, Litz 800x0.1) with custom definitions.

## 🛠️ Contributing

Contributions are welcome! Whether you're:

- Adding new component definitions to the databases
- Improving documentation
- Fixing bugs in the schemas
- Suggesting new features

Please see the [OpenMagnetics](https://github.com/OpenMagnetics) organization for guidelines.

## 📫 Get in Touch

- **Issues**: [GitHub Issues](https://github.com/OpenMagnetics/MAS/issues)
- **Discussions**: [GitHub Discussions](https://github.com/OpenMagnetics/MAS/discussions)

## 📄 License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.

## 🙏 Acknowledgments

MAS stands for "Magnetic Agnostic Structure" and also means "more" in Spanish—reflecting our goal to enable more powerful magnetic component design.

---

<p align="center">
  Made with ❤️ by the <a href="https://github.com/OpenMagnetics">OpenMagnetics</a> community
</p>

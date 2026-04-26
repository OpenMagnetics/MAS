# MAS — Normative and informative references

This document lists the external standards that MAS schemas defer to, the
ones that are useful informative context, and the genuine gaps where no
standard exists and MAS itself is the de-facto registry.

It is organised by the area of MAS each standard backs (shapes, wires,
insulation, materials, units, test methods). For each entry:

- **Status**:
  - **Normative** — MAS field values shall be selected from this standard.
  - **Informative** — useful background, optional citation in tooling.
  - **Historic** — withdrawn or issuing body inactive; cite for context only.
  - **Gap** — no standard exists; MAS is the working registry.
- **Cite at**: the schema files that should reference this standard in
  their `description` text.

The end of this document lists the four "Gap" areas where MAS could
become the primary reference if standardisation is later pursued.

---

## 1. Core shapes (ferrite)

| Standard | Title | Status | Cite at |
|----|----|----|----|
| **IEC 62317** (parts 1–13) | Ferrite cores — dimensions | Normative | `magnetic/core/shape.json`, per family (`E`, `ETD`, `EFD`, `PQ`, `RM`, `EP`, `PM`, `EER`, `P`, `U`) |
| **IEC 62317-7** | Ring (toroid) cores — dimensions | Normative | `magnetic/core/shape.json` (toroid family) |
| **IEC 63093-9** | Planar cores — dimensions (supersedes IEC 62317-9) | Normative | `magnetic/core/shape.json` (planar family) |
| **IEC 60401-1 / -3** | Terms, nomenclature and presentation of soft-ferrite catalogue data | Normative | `magnetic/core.json`, `magnetic/core/shape.json` |
| **IEC 60205** | Calculation of effective parameters of magnetic piece parts (Aₑ, lₑ, Vₑ, C₁) | Normative | every output / schema field that names Ae/le/Ve/C1 |
| **DIN 41280** | German predecessor for ferrite core dimensions | Historic | reference only; absorbed into IEC 60401 |
| **JIS C 2560:1992** | General rules for ferrite cores (Japanese Standards Association) | Normative (JP-market) | `magnetic/core/shape.json` (informative cross-ref) |

## 2. Core materials (ferrite, powder, amorphous, nanocrystalline, electrical steel)

### 2a. Soft ferrites (MnZn, NiZn — power and signal)

| Standard | Title | Status | Notes |
|----|----|----|----|
| **IEC 60404-1** | Magnetic materials — classification | Normative | top-level classification |
| **IEC 60404-2** | Methods of measurement of magnetic properties of electrical steel strip and sheet | Normative for measurement (electrical steel; analogous methods used for ferrite characterisation) | |
| **IEC 60404-8-1** | Specifications for individual materials — soft ferrites for telecommunications | Normative (telecom-grade only) | does **not** cover power-electronics ferrite grades |
| **IEEE Std 393-1991 (R2007)** | Test procedures for magnetic cores (permeability, core loss, saturation) | Normative for test methods, widely cited | aging but still referenced by Magnetics Inc, Fair-Rite |
| **MMPA SFG-98 / IMA "Soft Ferrites — A User's Guide"** | Compendium of MnZn/NiZn classifications and de-facto grade nomenclature | **Historic** (MMPA → IMA in 2001, IMA inactive since 2006) | still referenced by vendor literature for legacy grade names |
| **JIS C 2560**, **JIS C 2565** | Ferrite cores — general rules / measuring methods | Normative (JP-market) | cited by TDK, FDK, Hitachi/Proterial datasheets |

> **Gap:** there is **no active vendor-neutral standard** that classifies
> power-electronics ferrite grades (N87, 3C95, 3F36, etc.). Each grade is
> a manufacturer trademark. MAS encodes them via
> `materialComposition` (MnZn/NiZn enum) + `name` (trade name) +
> `family` (manufacturer designation), which is the correct fallback. See
> "Standardisation gaps" at end of this document.

### 2b. Soft magnetic powder cores

| Standard | Title | Status | Notes |
|----|----|----|----|
| **MPIF Standard 35 — Materials Standards for PM Soft Magnetic Materials** (2024 ed.) | Iron-based PM soft magnetic alloy classes: FL, FLN, FLN2, FLN4, FLN6, FLNC | Normative (iron-based families only) | does **not** cover MPP / High Flux / Kool Mµ / Sendust / XFlux |
| **IEC 60404-8-9** | Specifications for individual materials — soft composite materials | Normative (general) | does not enumerate commercial powder-core alloys |

> **Gap:** trademarked powder-core families (MPP, High Flux, Kool Mµ,
> Sendust, XFlux) have **no industry-wide registry**. Manufacturers'
> internal codes are all that exists (Magnetics Inc: 55 = MPP,
> 58 = High Flux, 77 = Kool Mµ). MAS encodes via `materialComposition`
> enum (FeSiAl, FeNiMo, FeSi, Carbonyl Iron, etc.) — again the correct
> fallback.

### 2c. Amorphous and nanocrystalline alloys

| Standard | Title | Status | Notes |
|----|----|----|----|
| **ASTM A901-12** | Amorphous magnetic core alloys, semi-processed types (Fe-base ribbon) | Normative | |
| **IEC 60404-8-10** | Amorphous magnetic strip | Normative | |
| **IEC 60404-8-11** | Nanocrystalline strip | Normative | |

> **Gap:** trade-name grades (Metglas, FINEMET, Vitroperm, Nanoperm) are
> proprietary; no public class registry beyond the IEC/ASTM measurement
> standards above.

### 2d. Electrical steels (laminated cores)

| Standard | Title | Status | Notes |
|----|----|----|----|
| **ASTM A772/A772M** | AC permeability and core loss using sinusoidal current | Normative | |
| **ASTM A773/A773M-21** | DC magnetic properties via hysteresigraph | Normative | |
| **ASTM A977/A977M-07(2020)** | High-coercivity hysteresigraph testing | Normative | |
| **ASTM A1086-20** | Thin-gauge non-oriented fully processed electrical steel | Normative | |
| **ASTM A753-21** | Wrought Ni-Fe soft magnetic alloys (UNS K94490 / K94840 / N14076 / N14080) | Normative | |
| **JIS C 2552:2014** | Cold-rolled non-oriented electrical steel strip/sheet | Normative (JP-market) | |
| **JIS C 2550-1** | Epstein-frame measurement | Normative (JP-market) | |
| **SAE AMS 7717F** | 50Ni-50Fe sheet/strip, annealed forming grade | Normative (aerospace) | |
| **SAE AMS 7718G** | 50Ni-50Fe bars/rods/tubing/forgings | Normative (aerospace) | |
| **SAE AMS 7701** | Ni-Fe sheet/strip, higher-Ni grade | Normative (aerospace) | |
| **MIL-PRF-27G w/Amd 1** | Transformers and inductors (audio / power / HP-pulse), general specification | Normative (defence supply chain) | |

## 3. Wires

### 3a. Conductor sizes

| Standard | Title | Status |
|----|----|----|
| **IEC 60228:2023** | Conductors of insulated cables (metric sizes; classes 1–6) | Normative |
| **ASTM B258-18** | American Wire Gauge (AWG) — diameters, areas, tolerances | Normative (US-market); informative for IEC documents |

### 3b. Magnet-wire enamel

| Standard | Title | Status |
|----|----|----|
| **IEC 60317** (multi-part) | Specifications for particular types of winding wires; sub-parts cover specific enamel chemistries and grade levels | Normative — each MAS `coating.grade` value (Grade 1 / 2 / 3, plus chemistry) maps to a specific 60317 sub-part |
| **NEMA MW 1000** | Magnet wire (US equivalent of IEC 60317), parts MW-15-C through MW-83-C and beyond | Normative (US-market) |

### 3c. Litz construction

> **Gap:** **no standard exists for litz construction** (twist scheme,
> bunching, serving). The strands themselves are covered by IEC 60317 /
> NEMA MW 1000. The bundle is purely vendor-defined (New England Wire,
> MWS, Rubadue, Elektrisola). MAS captures `strandCount` and
> `outerDiameter`; an optional `twistPitch` would close most of the gap.
> DIN 46435-1:1966 covered enamelled-wire dimensions but is withdrawn.

## 4. Insulation and safety

| Standard | Title | Status |
|----|----|----|
| **IEC 60085:2007 Ed. 4** | Electrical insulation thermal classes (Y/A/E/B/F/H/N/R/200/220/250) | Normative |
| **IEC 60112** | Comparative Tracking Index (CTI) and pollution groups (Group I, II, IIIA, IIIB) | Normative |
| **IEC 60664-1** | Insulation coordination for low-voltage equipment (clearances, creepage) | Normative |
| **IEC 62368-1** | Audio/video, information and communication technology equipment — safety (modern source for reinforced/double/basic insulation definitions) | Normative |
| **IEC 61558** (multi-part) | Safety of transformers, reactors, power supply units | Normative |
| **IEC 60950** | Information technology equipment — safety (legacy, superseded by 62368-1) | Historic |
| **UL 1446** (8th ed., 2025) | Systems of insulating materials — covers magnet-wire coatings, varnishes, insulation systems for transformers / motors / solenoids | Normative (US-market) |

## 5. Bobbins

> **Gap:** no IEC, ASTM, JIS, or NEMA standard exists for bobbin
> geometry or naming. Bobbins are matched to cores in vendor catalogues.
> MAS's bobbin schema is itself the working description — a genuine
> contribution rather than a citation.

## 6. Quantities, units, and vocabulary

| Standard | Title | Status | Cite at |
|----|----|----|----|
| **ISO 80000-6** | Quantities and units — Part 6: Electromagnetism | Normative | `docs/units.md`, top |
| **ISO 80000-5** | Quantities and units — Part 5: Thermodynamics (justifies Celsius alongside Kelvin) | Normative | `docs/units.md` (temperature note) |
| **IEC 60050-121** | International Electrotechnical Vocabulary — Electromagnetism | Normative | top of `docs/` |
| **IEC 60050-221** | International Electrotechnical Vocabulary — Magnetic materials and components | Normative | top of `docs/` |

## 7. Test and measurement methods

| Standard | Title | Status |
|----|----|----|
| **IEC 60205** | Effective magnetic parameters | Normative |
| **IEC 61007** | Transformers and inductors — measuring methods | Normative |
| **IEC 62044** (parts 1–3) | Cores for inductive components — measuring methods | Normative |
| **IEEE Std 393-1991 (R2007)** | Test procedures for magnetic cores | Normative (legacy but active) |

## 8. Loss models

| Reference | Notes | Status |
|----|----|----|
| Steinmetz (1892) — `Trans. AIEE` empirical power law | Foundational; cited in every Steinmetz-method MAS field | Informative (academic) |
| Roshen (2007) — `IEEE Trans. Magnetics` 43(8) | Excess-loss + resistivity dependencies | Informative (academic) |
| Micrometals, Magnetics Inc, Fair-Rite, TDG, Magnetec, Poco loss models | Vendor-published curve fits | Informative (vendor) |

> Loss-model coefficients in MAS are **informative**, not normative — IEC
> 62044 / 61007 standardise the *measurement* of core losses, not the
> curve-fitting model. Each MAS loss-method block should carry a
> `source` field citing the primary literature or vendor reference for
> traceability.

## 9. Identifier registries (downstream interop)

| Standard | Title | Status | Used by |
|----|----|----|----|
| **ISO/IEC 11179-6** / **ISO 29002-5** | Metadata registries — IRDI format `RAI#DI#VI` | Normative | RFC 0003 |
| **IEC 61360-4** | Common Data Dictionary for electrical/electronic components | Normative | RFC 0003 |
| **IEC CDD** (https://cdd.iec.ch) | Authoritative IRDI registration authority for IEC classes | Normative | RFC 0003 |
| **ECLASS** | IEC 61360-conformant industrial classification, IRDI-native (RAI `0173-1`) | Normative (industrial fallback) | RFC 0003 |

---

## Standardisation gaps (where MAS is the working registry)

These four areas have **no active, vendor-neutral standard** at the
material-class or construction level. MAS encodes them pragmatically
today and is the natural document to fill the gap if international
standardisation is later pursued.

| Area | What is missing | Current MAS workaround | Owner if pursued |
|----|----|----|----|
| Power-electronics ferrite grade nomenclature (N87, 3C95, 3F36, …) | Vendor-neutral registry of grade classes | `materialComposition` enum + `name` + `family` strings | PSMA Magnetics Committee + IEC TC 51 |
| Powder-core alloy families (MPP, High Flux, Kool Mµ, Sendust, XFlux) | MPIF 35 covers iron-based only; trademarked families have no public registry | `materialComposition` enum (FeSiAl, FeNiMo, FeSi, Carbonyl Iron, Iron) | MPIF + PSMA + IEC TC 68 |
| Amorphous / nanocrystalline grade classes (Metglas, FINEMET, Vitroperm, Nanoperm) | Trademarked grades, no class registry beyond IEC/ASTM measurement standards | `materialComposition` + `name` + `family` | IEC TC 68 SC 2 |
| Litz wire construction (twist pitch, bunching, serving) | Standards cover the strand only; bundle construction is vendor-defined | `strandCount` + `outerDiameter` (extend with optional `twistPitch`) | NEMA + IEC TC 55 |

Bobbin geometry is a fifth gap but smaller in scope — it would be folded
into a future planar-and-bobbin work item rather than its own programme.

---

## Maintenance

- Review this document at every MINOR release: confirm referenced
  standard editions are current; flag any superseded parts (e.g. 62317-9
  → 63093-9 historically).
- The MAS field-level `description` strings should append a one-line
  pointer to the relevant section here, e.g.
  `"... See docs/normative-references.md §2a."`
- Discovery of additional national standards (CN GB, KR KS, IN BIS) is
  welcome via the contribution process.

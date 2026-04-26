# MAS — Units of measurement

This document is **normative**: every numeric field in the MAS schema
carries the unit listed here. JSON values are bare numbers; the unit is
fixed by this table and is not encoded per value. Tools may render values
in scaled units (e.g. mm, kHz, °F) for human display, but the wire format
is what this document specifies.

The ratifying specification is RFC 0001 v2
(`proposals/0001-units.md`). For the rationale on why MAS does not wrap
values in `{value, unit}` objects, see that RFC.

## Quick reference

| Quantity                  | Unit                       | UCUM code  | Typical fields |
|---------------------------|----------------------------|------------|----------------|
| Frequency                 | hertz                      | `Hz`       | `switchingFrequency`, ringing frequency, resonance |
| Time                      | second                     | `s`        | dwell time, duty-cycle interval |
| Temperature               | degree Celsius             | `Cel`      | `ambientTemperature`, Curie temperature, insulation class, loss-curve x-axes |
| Temperature difference    | kelvin                     | `K`        | temperature rise, thermal resistance numerator |
| Length                    | metre                      | `m`        | core dimensions, gap length, winding window dims |
| Area                      | square metre               | `m2`       | effective area `Ae`, window area `Aw` |
| Volume                    | cubic metre                | `m3`       | effective volume `Ve` |
| Mass                      | kilogram                   | `kg`       | core mass, winding mass |
| Current                   | ampere                     | `A`        | DC, peak, RMS (per field name) |
| Voltage                   | volt                       | `V`        | DC, peak, RMS (per field name) |
| Resistance                | ohm                        | `Ohm`      | DC resistance, AC effective |
| Inductance                | henry                      | `H`        | self, mutual, leakage |
| Capacitance               | farad                      | `F`        | inter-winding, stray |
| Charge                    | coulomb                    | `C`        | partial-discharge inception |
| Energy                    | joule                      | `J`        | per-cycle, stored |
| Power                     | watt                       | `W`        | losses, throughput |
| Loss density              | watt per cubic metre       | `W/m3`     | core-loss curve y-axis |
| Magnetic flux density     | tesla                      | `T`        | peak, AC, saturation |
| Magnetic field strength   | ampere per metre           | `A/m`      | H, coercive force `Hc` |
| Magnetic flux             | weber                      | `Wb`       | Φ |
| Magnetic permeability     | dimensionless (relative)   | `1`        | μᵣ; absolute μ is rarely used |
| Reluctance                | per henry                  | `/H`       | core, gap |
| Thermal resistance        | kelvin per watt            | `K/W`      | rise per loss |
| Thermal conductivity      | watt per metre kelvin      | `W/(m.K)`  | bobbin / potting / core property |
| Specific heat capacity    | joule per kilogram kelvin  | `J/(kg.K)` | thermal mass |
| Density (mass)            | kilogram per cubic metre   | `kg/m3`    | material density |
| Resistivity               | ohm metre                  | `Ohm.m`    | wire material |
| Conductivity              | siemens per metre          | `S/m`      | wire material (alternate to resistivity) |
| Angle                     | radian                     | `rad`      | phase, mechanical angle |
| Plane angle (catalogue)   | degree                     | `deg`      | only where the catalogue source specifies degrees; document at the field |
| Dimensionless ratios      | dimensionless              | `1`        | duty cycle, turns ratio, winding fill factor |

## Conventions

### Temperature is Celsius

MAS uses **°C** for every temperature field. Rationale:

- Every ferrite datasheet, every wire enamel temperature class
  (IEC 60085: B = 130 °C, F = 155 °C, H = 180 °C, etc.), every Curie
  point, every insulation rating is published in °C.
- ISO 80000-5 explicitly accepts °C alongside K and recommends °C for
  "everyday temperatures."
- IEC standards in the magnetics space (60085, 60401, 62317, 63093) use
  °C.

**Exception:** *temperature differences* (rise above ambient, the
denominator inside thermal resistance) use **K**. A Celsius difference
and a Kelvin difference are numerically identical, but K is the SI form
for differences. Field names that mean a difference (e.g.
`temperatureRise`, `K/W` thermal resistance) are in K.

### Length is metres

JSON values for length, area and volume are in **base SI** (m, m², m³).
A core leg of `34.1 mm` is encoded as `0.0341`. Catalogue UIs typically
display millimetres; that is a presentation choice, not a schema one.

### RMS, peak and DC

Field names always disambiguate: `rmsCurrent`, `peakCurrent`,
`dcCurrent`. A bare `current` (without a qualifier) is a documentation
defect — open an issue.

### Single numbers, not tolerance bands

Datasheet values are encoded as single numbers. Permeability, saturation
flux density, gap length, resistivity — one nominal value per field, no
`{nominal, minimum, maximum}` wrapper. If a future field genuinely needs
a band, sibling fields (`<field>Min`, `<field>Max`) are added at that
point.

### Out-of-table units

If you encounter a numeric field whose unit is not covered here, that is
a bug in either the spec or this document — please open an issue. Do not
silently introduce a new unit in a downstream tool.

## UCUM

The UCUM codes in the third column are **informative**, included so that
downstream consumers (HL7 FHIR exporters, AAS / Industry 4.0 digital
twins, IEC 61360 / CDD records) can carry MAS data lossless. They are
not currently encoded in the JSON. RFC 0001 v2 §3 discusses why.

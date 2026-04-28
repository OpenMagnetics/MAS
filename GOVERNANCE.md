# MAS Governance

This document describes how the **Magnetic Agnostic Structure (MAS)** is
maintained and how decisions are made. It is a working draft.

## 1. Scope

MAS is a vendor-neutral data model for describing magnetic components used in
power electronics: their inputs (operating points, requirements), construction
(core, coil, materials, gaps, insulation) and computed outputs (losses,
inductance, temperature). The deliverables governed by this document are:

- The JSON Schema files under `schemas/`.
- The bundled reference component database under `data/`.
- The conformance test suite (samples + validators).
- Generated language bindings (currently `MAS.hpp`).

Out of scope: any specific implementation of a magnetic-design tool.

## 2. Stewardship

| Phase | Steward | Status |
|------|---------|--------|
| Incubation | OpenMagnetics maintainers | current |
| Industry adoption | PSMA Magnetics Committee Working Group on MAS | proposed |

## 3. Roles

- **Maintainers** — merge PRs, cut releases, triage issues. Listed in
  `MAINTAINERS.md` (to be added).
- **Working Group** — open meeting body with representatives from
  manufacturers, tool vendors and end-users; reviews proposals for MAJOR
  changes and conformance profiles.
- **Editor** — single named individual responsible for spec text consistency.

## 4. Decision making

- **PATCH / MINOR changes**: lazy consensus on the PR. Any maintainer may
  merge after 72 h with no unresolved objection.
- **MAJOR changes**: require a written proposal (issue or `proposals/`
  directory), a 14-day comment period, and explicit approval by a
  super-majority (2/3) of the Working Group.
- **License, governance and IPR changes**: require unanimous maintainer
  approval and notification to all known contributors.

## 5. Intellectual property

- All contributions are licensed under **Apache-2.0**, including the explicit
  patent grant in §3 of that license.
- Contributors must agree to the project DCO (`Signed-off-by:` trailer) on
  every commit. A formal CLA may be introduced if required by a future
  steward organisation.
- Schema files carry SPDX headers (`SPDX-License-Identifier: Apache-2.0`).
- The bundled component database (`data/*.ndjson`) contains catalogue data
  derived from publicly available manufacturer datasheets. Each record
  identifies its source manufacturer; corrections from manufacturers are
  welcome and processed under the same governance.

## 6. Versioning and stability

See `CHANGELOG.md` for the versioning policy. The summary:

- Pre-1.0: minor breaking changes are permitted but must be flagged in the
  changelog.
- 1.0 onward: SemVer applies strictly. Breaking changes require a MAJOR
  bump, a deprecation period of at least one MINOR release, and a
  documented migration path.

## 7. Conformance

MAS will define conformance **classes** (Class A — Inductor Basic, Class B —
Transformer, Class C — Full) so that small implementations may claim
partial conformance. Conformance class definitions, once added, are
governed as MAJOR changes.

## 8. Communication

- Issue tracker: GitHub issues on the canonical repository.
- Working Group meetings: cadence and venue to be set if and when a
  Working Group is formed.
- Security / IPR concerns: see `SECURITY.md` (to be added).

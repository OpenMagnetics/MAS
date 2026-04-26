# Changelog

All notable changes to the Magnetic Agnostic Structure (MAS) specification are
documented in this file. The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/)
and the project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

Versioning rules for MAS:

- **MAJOR** — backwards-incompatible schema changes (removed/renamed fields,
  tightened constraints, semantic changes to existing fields).
- **MINOR** — backwards-compatible additions (new optional fields, new schemas,
  new enum values that consumers are required to ignore-on-unknown).
- **PATCH** — clarifications, documentation, examples, bug fixes that do not
  alter the validation surface.

A change to the bundled component database (`data/*.ndjson`) follows the same
rules: adding a material/shape/wire is MINOR, removing or renaming one is
MAJOR.

## [Unreleased]

### Added
- Apache-2.0 license (replaces BSD-4-Clause). Required for the planned
  PSMA → IEC PAS transposition path.
- `masVersion` (optional) on the root MAS object. Will become **required** in
  1.0.0 with a one-minor-version deprecation warning beforehand.
- `CHANGELOG.md`, `GOVERNANCE.md`, `CONTRIBUTING.md` to document project
  process and the path to standardisation.

### Deprecated
- BSD-4-Clause license. Removed in this release; downstream redistributors
  relying on the advertising clause should switch to Apache-2.0 attribution.

## [0.1.0] - prior to this changelog

Initial public schema, C++ binding generation via quicktype, component
databases for cores, materials, wires and bobbins. See git history for
per-commit detail.

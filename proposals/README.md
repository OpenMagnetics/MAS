# MAS Proposals (RFCs)

Design documents for non-trivial changes to the MAS specification.
Per `GOVERNANCE.md` §4, every MAJOR change starts here as a numbered RFC,
gets a 14-day comment period, and needs Working-Group super-majority
before any schema or code lands.

## Workflow

1. Copy `0000-template.md` (TBD) to `NNNN-short-slug.md`, where `NNNN`
   is the next free four-digit number.
2. Open a PR adding the file with **Status: Draft**.
3. Discuss on the PR. Material changes update the RFC text.
4. When the WG reaches super-majority, set **Status: Accepted** and
   merge.
5. Implementation lands in a *separate* PR that links back to the
   accepted RFC. On merge of the implementation, set **Status:
   Implemented** with the SemVer release that ships it.
6. RFCs that are abandoned move to **Status: Withdrawn** (kept for
   historical record).

## Index

| # | Title | Status | Target |
|---|-------|--------|--------|
| [0001](0001-quantity-units.md) | `Quantity` type and explicit SI units | Draft | 1.0.0 |
| [0002](0002-conformance-classes.md) | Conformance classes (A / B / C) | Draft | 0.2.0 / 1.0.0 |
| [0003](0003-irdi-identifiers.md) | IRDI identifiers via IEC CDD | Draft | 0.2.0 |
| [0004](0004-temperature-kelvin.md) | Temperature in Kelvin everywhere | Draft | 1.0.0 |

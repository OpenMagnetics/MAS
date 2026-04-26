# Security and IPR concerns

MAS is a data-format specification rather than executable software, so
the conventional "security vulnerability" surface is small. The two
classes of concern that warrant private reporting are:

1. **Schema or reference-implementation bugs** that could cause a
   downstream tool to misinterpret a MAS document in a way that
   produces unsafe physical designs (e.g. mis-sized insulation,
   under-rated wire). Even though MAS itself does not generate hardware,
   a defect that propagates silently into manufacturing falls under
   this policy.
2. **Suspected patent encumbrance** of any field, structure or method
   defined in the schema. The Apache-2.0 patent grant in
   [`LICENSE.md`](LICENSE.md) covers contributions, but a third-party
   patent that reads on the spec is a separate matter — and one that
   matters greatly for the planned IEC submission.

## How to report

- **Do not open a public GitHub issue** for either class.
- Email the current maintainers (see [`MAINTAINERS.md`](MAINTAINERS.md))
  directly. Use a subject line that begins with `[MAS-SECURITY]` or
  `[MAS-IPR]` so the report is triaged quickly.
- Include enough detail for the maintainers to reproduce or assess the
  concern: the schema file or field, an example MAS document, the
  patent number or claim text, and your contact information.

## Handling

- Acknowledgement within 5 working days.
- A coordinated-disclosure timeline agreed with the reporter.
- Once a fix is published or an IPR matter is resolved, public
  disclosure with credit to the reporter (unless the reporter requests
  anonymity).

## Scope

This policy covers the contents of the MAS specification repository
(`schemas/`, `docs/`, `proposals/`, the reference C++ binding
`MAS.hpp`, the bundled `data/` catalogue and the project-level Markdown
files). Downstream tools that *consume* MAS documents — including the
MKF library and PyMKF — have their own security policies.

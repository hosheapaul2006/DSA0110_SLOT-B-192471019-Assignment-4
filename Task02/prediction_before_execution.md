# Predictions recorded before either program is executed

## Original virtual diamond

Exact bytes after newline decoding: `Person Student Staff TA \n`. There is a space immediately before the newline. The most-derived TA initializes the single virtual Person first, followed by direct bases Student and Staff in base-list order, then its own body. No compiler diagnostic is expected.

## Both virtual keywords removed

Exact bytes: `Person Student Person Staff TA \n`. Student owns one Person and Staff another. Each Person is built before its respective direct base. The unchanged source never refers to a shared Person member, so no ambiguity error is expected in this particular program. A separate member-access example will demonstrate ambiguity.

These predictions must remain unchanged after observation.

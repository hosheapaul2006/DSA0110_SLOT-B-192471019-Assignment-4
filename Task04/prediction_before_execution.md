# Prediction before first execution

Exact output: `3.5 4 3.5\n`. The default stream formatting prints 4, not 4.0. p has static type Account*, so the nonvirtual rate call selects Account::rate (3.5). The virtual vrate call dispatches to SeniorAccount (4). copy is a complete Account object sliced from s, so its virtual vrate selects Account (3.5). The original prediction must remain unchanged after observation.

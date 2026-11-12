# Task 4 - Polymorphism forensics

## Defects and safe evidence boundaries

1. The supplied rate() is nonvirtual. Through Account*, static binding calls Account::rate regardless of the allocated object's derived type; both print 3.5.
2. Deleting a SeniorAccount through Account* with the supplied nonvirtual destructor is undefined behavior. The language gives no valid destruction guarantee. A common symptom is that only Account's destructor runs and history is not released, but neither that outcome nor a sanitizer diagnosis is guaranteed by C++.
3. `Account copy = SeniorAccount()` creates a complete Account containing only the base portion. Derived state and behavior are sliced away; it is not a polymorphic reference. The temporary SeniorAccount is destroyed through its actual type. Slicing itself is defined in this case; the earlier base-pointer deletion is the undefined operation.

original_banking.hpp preserves the supplied design and adds constructor/destructor messages. original.cpp wraps the statements in main and flushes trace output so sanitizer termination does not discard it. This unsafe driver is compiled only on Windows, then executed solely under isolated Ubuntu AddressSanitizer. ASan may stop before the last slicing statement, so safe_slicing.cpp observes slicing separately on a stack object, without the bad delete. Absence of an ASan diagnostic would not establish safety. Actual Ubuntu ASan output in outputs/sanitizer_original_stderr.txt reports new-delete-type-mismatch: allocated type 8 bytes, deleted type 1 byte, exit 1. The trace prints two base destructor messages but no SeniorAccount destructor before the sanitizer abort. The last slicing statement is not reached in that run. No leaked-byte total is inferred from this stopped execution. Repaired/safe cases exit 0 with empty sanitizer stderr. See Reference/sanitizer_summary.json and the successful workflow linked in README.

## Repair

accounts.hpp makes rate virtual, annotates the derived signature override, and declares a virtual base destructor. repaired.cpp processes base pointers without copying, manually deletes two owners, then demonstrates vector<unique_ptr<Account>>. SeniorAccount owns its history via unique_ptr<double[]> so array cleanup is automatic. The repaired base disables copy construction/assignment to reject accidental by-value slicing; slicing_rejected.cpp is an expected compiler-error example. Destructor counters and traces establish complete cleanup in safe executions, with sanitizer verification providing additional evidence.

## Prevention rules

- A base intended for polymorphic ownership must have a public virtual destructor; a protected nonvirtual destructor is another deliberate design when deletion through base is forbidden.
- Declare overriding functions override, and put the intended virtual interface in the base.
- Pass polymorphic objects by pointer/reference; disable copying or design an explicit virtual clone rather than silently slicing.
- Express ownership with RAII; distinguish owning and borrowed pointers.

## Prediction

The exact supplied prediction source has a virtual destructor already. It tests binding/slicing, not unsafe deletion. The immutable pre-execution prediction is `3.5 4 3.5` followed by newline; default stream formatting omits the .0. Compare against outputs/predict_output_actual.txt and its verification record.

## Raw pointers versus vector of unique_ptr

Both versions process n accounts in O(n) time, with O(1) dispatch per call and O(n) pointers plus account/history storage. A raw pointer alone is an address, not an ownership contract. Manual new/delete needs exception-safe cleanup if later allocations or operations throw; the simple raw demonstration is intentionally bounded. The vector of unique_ptr cleans up existing owners if construction or later code throws, prevents copying ownership, and preserves dynamic types. Neither smart pointers nor containers make a nonvirtual rate dynamically dispatched, add a virtual destructor to an unsafe base, or prevent some other code from copying into a vector<Account>. unique_ptr<Account> still requires correct polymorphic destruction. Actual safe cleanup and expected negative diagnostics are separate results, not evidence that the supplied design is safe.

Viva practice: explain why a smart pointer cannot fix missing virtual dispatch; demonstrate how disabling the base copy constructor prevents slicing at compile time.

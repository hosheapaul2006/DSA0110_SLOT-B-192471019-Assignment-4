# Prediction comparison

Both original pre-execution predictions matched exactly after platform newline decoding, with no other whitespace normalization. Both valid supplied variants compiled without warnings. See outputs/predict_output_verification.json and diamond_experiment_verification.json. The separate ambiguity.cpp source failed as expected because it actually accesses a duplicated name member; removing virtual alone does not make every diamond program ill-formed.

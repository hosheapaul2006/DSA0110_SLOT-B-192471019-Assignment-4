# Prediction before first execution

Exact output is `Vehicle Engine Car | ~Car ~Engine ~Vehicle \n`, including the space before newline. The direct Vehicle base is constructed first, followed by the Engine member, then the Car constructor body. After the bar printed in main, Car's destructor body runs, then Engine's destructor, then Vehicle's destructor. No virtual dispatch or deletion is involved here. The prediction must not be edited after observing output.

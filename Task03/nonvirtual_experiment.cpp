// Compile the same trace model with virtual inheritance removed.
// Its indirect Person initializer is also removed as required by C++.
#define NONVIRTUAL_PERSON
#include "main.cpp"

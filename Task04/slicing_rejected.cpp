// Expected compiler error: production design disables base copying.
#include "accounts.hpp"
int main() { SeniorAccount senior; Account sliced = senior; }

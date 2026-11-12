#include "original_banking.hpp"
#include "../tools/checks.hpp"
int main(int argc, char** argv) {
    Tests tests(argc, argv);
    SeniorAccount senior;
    Account* base = &senior;
    Account copy = senior; // Slicing is defined here; senior itself is destroyed by its real type.
    tests.check(senior.rate() == 4 && base->rate() == 3.5 && copy.rate() == 3.5,
                "nonvirtual binding and safely observed slicing");
    std::cout << "Direct=" << senior.rate() << " base=" << base->rate() << " sliced=" << copy.rate() << '\n'
              << "sizeof base=" << sizeof(Account) << " senior=" << sizeof(SeniorAccount) << '\n';
}

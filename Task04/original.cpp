// Intentionally undefined deletion: compile locally, execute only in isolated ASan.
#include "original_banking.hpp"
int main() {
    std::cout << std::unitbuf;
    Account* accounts[2] = { new Account, new SeniorAccount };
    for (Account* a : accounts) std::cout << a->rate() << "\n";
    for (Account* a : accounts) delete a;
    Account copy = SeniorAccount();
    std::cout << "Sliced rate: " << copy.rate() << '\n';
}

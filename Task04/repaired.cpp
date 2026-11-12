#include "accounts.hpp"
#include "../tools/checks.hpp"
#include <iomanip>
#include <vector>
int main(int argc, char** argv) {
    Tests tests(argc, argv);
    std::cout << std::fixed << std::setprecision(1);
    {
        Account* accounts[2] = { new Account, new SeniorAccount };
        tests.check(accounts[0]->rate() == 3.5, "base interest rate");
        tests.check(accounts[1]->rate() == 4.0, "derived interest through base pointer");
        for (Account* account : accounts) std::cout << "Raw owner rate=" << account->rate() << '\n';
        for (Account* account : accounts) delete account;
    }
    tests.check(Account::live == 0 && SeniorAccount::destroyed == 1, "raw polymorphic destruction completes");
    {
        std::vector<std::unique_ptr<Account>> accounts;
        accounts.push_back(std::make_unique<Account>());
        accounts.push_back(std::make_unique<SeniorAccount>());
        tests.check(accounts[0]->rate() == 3.5 && accounts[1]->rate() == 4.0, "smart-owner polymorphism");
        for (const auto& account : accounts) std::cout << "Smart owner rate=" << account->rate() << '\n';
    }
    tests.check(Account::live == 0 && SeniorAccount::destroyed == 2 && Account::destroyed == 4,
                "all smart owners destroy complete objects");
    std::vector<std::unique_ptr<Account>> empty;
    tests.check(empty.empty() && Account::live == 0, "empty collection has no owned accounts");
}

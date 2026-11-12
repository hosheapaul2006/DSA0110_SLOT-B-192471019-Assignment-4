#ifndef ACCOUNTS_HPP
#define ACCOUNTS_HPP
#include <iostream>
#include <memory>
class Account {
public:
    inline static int live = 0;
    inline static int destroyed = 0;
    Account() { ++live; }
    Account(const Account&) = delete; // Prevent accidental by-value slicing in this design.
    Account& operator=(const Account&) = delete;
    virtual double rate() const { return 3.5; }
    virtual ~Account() { --live; ++destroyed; std::cout << "~Account\n"; }
};
class SeniorAccount : public Account {
    std::unique_ptr<double[]> history_ = std::make_unique<double[]>(100);
public:
    inline static int destroyed = 0;
    double rate() const override { return 4.0; }
    ~SeniorAccount() override { ++destroyed; std::cout << "~SeniorAccount (history owner released next)\n"; }
};
#endif
#ifndef COURSEWORK_CHECKS_HPP
#define COURSEWORK_CHECKS_HPP
#include <iostream>
#include <stdexcept>
#include <string>
class Tests {
    bool logging;
    unsigned count=0;
public:
    Tests(int argc,char** argv):logging(argc>1 && std::string(argv[1])=="--tests") {}
    void check(bool condition,const char* scenario) {
        ++count;
        if(logging) std::cerr << "CHECK|" << scenario << '|' << (condition?"PASS":"FAIL") << '\n';
        if(!condition) throw std::runtime_error(scenario);
    }
    ~Tests() { if(logging) std::cerr << "TEST_COUNT " << count << '\n'; }
};
#endif
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

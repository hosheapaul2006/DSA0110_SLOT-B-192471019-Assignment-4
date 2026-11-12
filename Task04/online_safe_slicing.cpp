#ifndef ORIGINAL_BANKING_HPP
#define ORIGINAL_BANKING_HPP
#include <iostream>
// Supplied design, with constructor/destructor trace messages added.
class Account {
public:
    Account() { std::cout << "+Account\n"; }
    double rate() const { return 3.5; }
    ~Account() { std::cout << "~Account\n"; }
};
class SeniorAccount : public Account {
    double* history = new double[100];
public:
    SeniorAccount() { std::cout << "+SeniorAccount\n"; }
    double rate() const { return 4.0; }
    ~SeniorAccount() { std::cout << "~SeniorAccount (release history)\n"; delete[] history; }
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

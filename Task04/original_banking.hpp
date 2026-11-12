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

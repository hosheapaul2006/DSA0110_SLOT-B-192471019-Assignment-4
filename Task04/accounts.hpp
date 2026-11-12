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

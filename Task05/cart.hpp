#ifndef CART_HPP
#define CART_HPP
#include <array>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <string>

class Cart {
    std::array<std::string, 10> items_;
    std::array<double, 10> prices_{};
    std::size_t count_ = 0;
    double discount = 0;
public:
    Cart& add(const std::string& item, double price) {
        if (item.empty() || !std::isfinite(price) || price < 0)
            throw std::invalid_argument("Invalid item or price");
        if (count_ == items_.size()) throw std::length_error("Cart capacity is ten");
        double subtotal = price;
        for (std::size_t i = 0; i < count_; ++i) subtotal += prices_[i];
        if (!std::isfinite(subtotal)) throw std::overflow_error("Cart total overflow");
        items_[count_] = item;
        prices_[count_] = price;
        ++count_;
        return *this;
    }
    Cart& setDiscount(double discount) {
        if (!std::isfinite(discount) || discount < 0 || discount > 100)
            throw std::invalid_argument("Discount must be 0..100 percent");
        this->discount = discount;
        return *this;
    }
    double total() const {
        double amount = 0;
        for (std::size_t i = 0; i < count_; ++i) amount += prices_[i];
        return amount * ((100 - discount) / 100); // multiply by a factor <= 1, avoiding overflow
    }
    std::size_t size() const { return count_; }
    const Cart* identity() const { return this; }
};

inline Cart* highestCart(Cart* const* carts, std::size_t count) {
    if (count != 0 && carts == nullptr) throw std::invalid_argument("Missing pointer array");
    Cart* best = nullptr;
    for (std::size_t i = 0; i < count; ++i)
        if (carts[i] && (!best || carts[i]->total() > best->total())) best = carts[i];
    return best; // empty/all-null -> nullptr; equal totals keep the first cart
}
#endif

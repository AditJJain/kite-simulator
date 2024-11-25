#ifndef BUY_HPP
#define BUY_HPP

#include <string>
#include <iostream>

class BuyOrder {
public:
    // Constructor to initialize a buy order
    BuyOrder(const std::string& symbol, double price, int quantity)
        : symbol_(symbol), price_(price), quantity_(quantity) {}

    // Member function to execute the buy order
    void execute() const {
        std::cout << "Executing buy order for " << quantity_ << " shares of " 
                  << symbol_ << " at $" << price_ << " per share." << std::endl;
    }

private:
    std::string symbol_;  // Stock symbol (e.g., "AAPL")
    double price_;        // Price per share
    int quantity_;        // Number of shares to buy
};

#endif // BUY_HPP

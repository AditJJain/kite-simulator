#ifndef INTRADAYPOSITION_H
#define INTRADAYPOSITION_H

#include <string>

struct IntradayPosition {
    std::string symbol;
    int quantity;       // Positive for long, negative for short
    double entryPrice;  // Buy price for long, sell price for short
    double currentPrice;
    double unrealizedPL;
    std::string timestamp;   // Added to store timestamp
};

#endif // INTRADAYPOSITION_H


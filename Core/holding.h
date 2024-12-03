#ifndef HOLDING_H
#define HOLDING_H

#include <string>

struct Holding {
    std::string symbol;
    int quantity;
    double averagePrice;
    double unrealizedPL;
    double currentPrice;
    std::string timestamp;
};

#endif // HOLDING_H
// intraday.h
#ifndef INTRADAY_H
#define INTRADAY_H

#include <string>
#include <vector>
#include <utility>

struct IntradayPosition {
    std::string symbol;
    int quantity;       // Positive for long, negative for short
    double entryPrice;  // Buy price for long, sell price for short
    double currentPrice;
    double unrealizedPL;
    std::string timestamp;   // Added to store timestamp
};

std::pair<double, std::string> getCurrentPrice(const std::string& symbol);
void loadIntradayPositions(const std::string& username, std::vector<IntradayPosition>& positions);
void calculateUnrealizedPL(std::vector<IntradayPosition>& positions);
void displayIntradayPositions(const std::vector<IntradayPosition>& positions);
void setNonBlockingMode(bool enable);
void runPositions(const std::string& username);

#endif // INTRADAY_H
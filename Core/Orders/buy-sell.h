#ifndef BUY_SELL_H
#define BUY_SELL_H

#include <string>
#include <utility>

std::string trim(const std::string& str); // Utility function to trim strings
bool checkFunds(double amount, const std::string& username);
void modifyFunds(double amount, const std::string& username);
void addPortfolioEntry(const std::string& username, const std::string& symbol, int quantity, double price);
bool removePortfolioEntry(const std::string& username, const std::string& symbol, int quantity);
void addPositionEntry(const std::string& username, const std::string& symbol, int quantity, double price, const std::string& timestamp);
void buyStock(const std::string& username);
void sellStock(const std::string& username);
void runBuySell(const std::string& username);

#endif // BUY_SELL_H

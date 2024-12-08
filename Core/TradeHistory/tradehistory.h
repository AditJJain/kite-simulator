#ifndef TRADEHISTORY_H
#define TRADEHISTORY_H

#include <string>

std::string getCurrentTimestamp();  // Declaration for the function

void logTradeHistory(const std::string& username, const std::string& action, const std::string& symbol, int quantity, double price);
void runTradeHistory(const std::string& username);

#endif  // TRADEHISTORY_H


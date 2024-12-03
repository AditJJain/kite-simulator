#ifndef APICALL_KITECONNECT_LTP_H
#define APICALL_KITECONNECT_LTP_H

#include <string>
#include <utility>

// Function to get the current market price and timestamp for a given stock symbol
std::pair<double, std::string> getCurrentMarketPrice(const std::string& symbol);

#endif // APICALL_KITECONNECT_LTP_H

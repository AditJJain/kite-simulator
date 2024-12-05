#include "apicall_KiteConnect_LTP.h"
#include <iostream>
#include <cstdio>
#include <memory>
#include <sstream>
#include <stdexcept>
using namespace std;

// Function to get current market price using a Python script
pair<double, string> getCurrentMarketPrice(const string& symbol) {
    string command = "/opt/anaconda3/bin/python Core/APIs/api_KiteConnectWS_LTP_Once.py " + symbol;
    char buffer[128];
    string result;

    unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        cerr << "Error: Failed to execute market price API." << endl;
        return {0.0, ""};
    }

    while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
        result += buffer;
    }

    result.erase(result.find_last_not_of(" \n\r\t") + 1);
    string price_str, timestamp;
    istringstream iss(result);

    if (getline(iss, price_str, ',') && getline(iss, timestamp)) {
        try {
            double price = stod(price_str);
            return {price, timestamp};
        } catch (const invalid_argument&) {
            cerr << "Error: Invalid price received for " << symbol << ". Output was: " << result << endl;
        }
    }
    return {0.0, ""};
}
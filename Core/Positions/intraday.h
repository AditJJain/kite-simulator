#ifndef INTRADAY_H
#define INTRADAY_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <cstdio>
#include <memory>
#include <thread>
#include <chrono>
#include <algorithm>
#include <map>
#include <numeric>

using namespace std;

struct IntradayPosition {
    string symbol;
    int quantity;       // Positive for long, negative for short
    double entryPrice;  // Buy price for long, sell price for short
    double currentPrice;
    double unrealizedPL;
    string timestamp;   // Added to store timestamp
};

// Function to call the external Python script and get the current market price and timestamp
pair<double, string> getCurrentMarketPrice(const string& symbol);

// Function to load intraday positions from a file
void loadIntradayPositions(const string& username, vector<IntradayPosition>& positions);

// Function to calculate unrealized P&L for intraday positions
void calculateUnrealizedPL(vector<IntradayPosition>& positions);

// Function to display intraday positions with unrealized P&L
void displayIntradayPositions(const vector<IntradayPosition>& positions);

#endif // INTRADAY_H

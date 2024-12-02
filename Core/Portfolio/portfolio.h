#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include <string>
#include <vector>
#include <utility>

using namespace std;

struct Holding {
    string symbol;
    int quantity;
    double averagePrice;
    double unrealizedPL;
    double currentPrice;
    string timestamp;
};

pair<double, string> getCurrentMarketPrice(const string& symbol);
void loadPortfolio(const string& username, vector<Holding>& portfolio);
void calculateUnrealizedPL(vector<Holding>& portfolio);
void displayPortfolio(const vector<Holding>& portfolio);
void displayAllocation(const vector<Holding>& portfolio);
void runPortfolio();

#endif // PORTFOLIO_H
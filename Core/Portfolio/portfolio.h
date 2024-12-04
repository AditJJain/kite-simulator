#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include <string>
#include <vector>
#include <utility>
#include "Core/holding.h"
using namespace std;

void loadPortfolio(const string& username, vector<Holding>& portfolio);
void calculateUnrealizedPL(vector<Holding>& portfolio);
void displayPortfolio(const vector<Holding>& portfolio, const string& username);
void displayAllocation(const vector<Holding>& portfolio);
void runPortfolio(const string& username);

#endif // PORTFOLIO_H
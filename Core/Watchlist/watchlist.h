#ifndef WATCHLIST_H
#define WATCHLIST_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>    // For find
#include <cctype>       // For toupper
#include <iomanip>      // For formatting output
#include <cstdio>       // For popen()
#include <memory>       // For managing dynamic allocations
#include <thread>       // For sleep
#include <chrono>       // For time
#include <termios.h>    // For non-blocking input
#include <unistd.h>     // For read
#include <sys/select.h> // For select

using namespace std;

// Function declarations
void watchlistMenu(const string& username);
void loadWatchlist(const string& username, vector<string>& watchlist);
void saveWatchlist(const string& username, const vector<string>& watchlist);
void displayWatchlist(const vector<string>& watchlist);
void addStockToWatchlist(vector<string>& watchlist);
void removeStockFromWatchlist(vector<string>& watchlist);
pair<double, string> getCurrentMarketPrice(const string& symbol);
string toUpperCase(const string& str);

// Helper functions for non-blocking input
struct termios orig_termios;

void resetTerminalMode();
void setTerminalMode();
bool isKeyPressed(char& c);

#endif // WATCHLIST_H

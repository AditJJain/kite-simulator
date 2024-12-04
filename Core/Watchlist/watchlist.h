// watchlist.h
#ifndef WATCHLIST_H
#define WATCHLIST_H

#include <string>
#include <vector>
#include <utility>

void loadWatchlist(const std::string& username, std::vector<std::string>& watchlist);
void saveWatchlist(const std::string& username, const std::vector<std::string>& watchlist);
std::pair<double, std::string> getCMP(const std::string& symbol);
void displayWatchlist(const std::vector<std::string>& watchlist);
void addStock(std::vector<std::string>& watchlist);
void removeStock(std::vector<std::string>& watchlist);

// Helper function to convert a string to uppercase
std::string toUpperCase(const std::string& str);

// Helper functions for non-blocking input
void resetTerminalMode();
void setTerminalMode();
bool isKeyPressed(char& c);

void runWatchlist(const std::string& username);

#endif // WATCHLIST_H
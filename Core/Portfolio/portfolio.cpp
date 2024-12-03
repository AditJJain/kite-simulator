#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <iomanip>   // For formatting output
#include <sstream>   // For parsing strings
#include <algorithm> // For sorting and calculations
#include <map>       // For holding portfolio data
#include <numeric>   // For accumulating totals
#include <cstdio>    // For popen()
#include <memory>    // For managing dynamic allocations
#include <thread>    // For sleep
#include <chrono>    // For time
#include "Core/holding.h"
#include "Core/APIs/apicall_KiteConnect_LTP.h"
#include "Core/setNonBlockingMode.h"
using namespace std;

// Function to load the portfolio from a file
void loadPortfolio(const string& username, vector<Holding>& portfolio) {
    string filename = "Core/Portfolio/" + username + "_portfolio.dat";
    ifstream infile(filename);

    if (infile.is_open()) {
        string line;
        while (getline(infile, line)) {
            istringstream iss(line);
            Holding holding;
            holding.unrealizedPL = 0.0; // Initialize unrealized P/L

            if (iss >> holding.symbol >> holding.quantity >> holding.averagePrice) {
                portfolio.push_back(holding);
            }
        }
        infile.close();
    } else {
        // Portfolio file doesn't exist; start with an empty portfolio
        portfolio.clear();
    }
}

// Function to calculate unrealized P/L for each holding and fetch CMP
void calculateUnrealizedPL(vector<Holding>& portfolio) {
    for (auto& holding : portfolio) {
        // Get the current market price and timestamp
        auto [price, timestamp] = getCurrentMarketPrice(holding.symbol);
        holding.currentPrice = price;
        holding.timestamp = timestamp;

        // If the current price is available, calculate unrealized P/L
        if (holding.currentPrice > 0.0) {
            holding.unrealizedPL = (holding.currentPrice - holding.averagePrice) * holding.quantity;
        } else {
            cout << "Current price for " << holding.symbol << " not available." << endl;
            holding.unrealizedPL = 0.0;
        }
    }
}

// Function to display the user's portfolio with only unrealized P/L, CMP, and timestamp
void displayPortfolio(const vector<Holding>& portfolio, const string& username) {
    // Move cursor to top-left
    cout << "\033[H";
    cout << "User: " << username << endl;
    cout << "\n=== Your Portfolio ===" << endl;
    cout << "Timestamp: " << (portfolio.empty() ? "N/A" : portfolio.front().timestamp) << endl;
    cout << left << setw(10) << "Symbol"
         << right << setw(10) << "Quantity"
         << setw(15) << "Avg Price"
         << setw(15) << "Current Price"
         << setw(15) << "Unrealized P/L" << endl;
    cout << "-----------------------------------------------------" << endl;

    double totalUnrealizedPL = 0.0;
    for (const auto& holding : portfolio) {
        cout << left << setw(10) << holding.symbol
             << right << setw(10) << holding.quantity
             << setw(15) << fixed << setprecision(2) << holding.averagePrice
             << setw(15) << holding.currentPrice
             << setw(15) << holding.unrealizedPL << endl;
        totalUnrealizedPL += holding.unrealizedPL;
    }

    cout << "-----------------------------------------------------" << endl;
    cout << left << setw(35) << "Total Unrealized P/L:"
         << right << setw(15) << totalUnrealizedPL << endl;
}

// Function to display portfolio allocation and diversification
void displayAllocation(const vector<Holding>& portfolio) {
    if (portfolio.empty()) {
        return;
    }

    // Calculate total investment
    double totalInvestment = 0.0;
    for (const auto& holding : portfolio) {
        totalInvestment += holding.averagePrice * holding.quantity;
    }
    
    cout << "\n=== Portfolio Allocation ===" << endl;
    cout << left << setw(10) << "Symbol"
         << right << setw(20) << "Allocation (%)" << endl;
    cout << "----------------------------------" << endl;

    for (const auto& holding : portfolio) {
        double allocation = ((holding.averagePrice * holding.quantity) / totalInvestment) * 100;
        cout << left << setw(10) << holding.symbol
             << right << setw(20) << fixed << setprecision(2) << allocation << endl;
    }

    cout << "----------------------------------" << endl;
    cout << "Total Diversification: " << portfolio.size() << " assets" << endl;
    cout << "\nPress 'Q' to return to the main menu." << endl << flush;
}

void runPortfolio(const string& username) {
    vector<Holding> portfolio;
    loadPortfolio(username, portfolio);

    if (portfolio.empty()) {
        cout << "Your portfolio is currently empty." << endl;
        this_thread::sleep_for(chrono::seconds(5));
        system("clear");
        return;
    }

    setNonBlockingMode(true); // Enable non-blocking mode

    while (true) {
        calculateUnrealizedPL(portfolio);

        // Update the portfolio display
        cout << "\033[2J\033[H"; // Clear screen and move cursor to top-left
        displayPortfolio(portfolio, username);
        displayAllocation(portfolio);

        // Sleep for 5 seconds while checking for user input
        for (int i = 0; i < 50; ++i) {
            char ch;
            if (read(STDIN_FILENO, &ch, 1) > 0) {
                if (ch == 'Q' || ch == 'q') {
                    setNonBlockingMode(false); // Disable non-blocking mode
                    system("clear"); // Clear the screen
                    return; // Exit the function
                }
            }
            this_thread::sleep_for(chrono::seconds(5));
        }
    }

    setNonBlockingMode(false); // Disable non-blocking mode before exiting
}
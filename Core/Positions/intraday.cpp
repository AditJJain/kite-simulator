#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <iomanip>   // For formatting output
#include <sstream>   // For parsing strings
#include <cstdio>    // For popen()
#include <memory>    // For managing dynamic allocations
#include <thread>    // For sleep
#include <chrono>    // For time
#include <algorithm> // For sorting and calculations
#include <map>       // For holding portfolio data
#include <numeric>   // For accumulating totals
#include "Core/APIs/apicall_KiteConnect_LTP.h"
#include "Core/intradayPosition.h"
#include "Core/setNonBlockingMode.h"
using namespace std;

// Function to load intraday positions from a file
void loadIntradayPositions(const string& username, vector<IntradayPosition>& positions) {
    string filename = "Core/Positions/" + username + "_positions.dat";
    ifstream infile(filename);

    if (infile.is_open()) {
        string line;
        while (getline(infile, line)) {
            istringstream iss(line);
            IntradayPosition position;
            position.unrealizedPL = 0.0; // Initialize unrealized P/L

            // Load symbol, quantity, and entry price from the file
            if (iss >> position.symbol >> position.quantity >> position.entryPrice) {
                positions.push_back(position);
            }
        }
        infile.close();
    } else {
        cerr << "Error: Could not open positions file for user: " << username << endl;
        positions.clear();
    }
}

// Function to calculate unrealized P&L for intraday positions
void calculateUnrealizedPL(vector<IntradayPosition>& positions) {
    for (auto& position : positions) {
        // Get the current market price and timestamp
        auto [price, timestamp] = getCurrentMarketPrice(position.symbol);
        position.currentPrice = price;
        position.timestamp = timestamp;

        // Calculate Unrealized P&L
        if (position.currentPrice > 0.0) {
            if (position.quantity > 0) { // Long position
                position.unrealizedPL = (position.currentPrice - position.entryPrice) * position.quantity;
            } else { // Short position
                position.unrealizedPL = (position.entryPrice - position.currentPrice) * abs(position.quantity);
            }
        } else {
            cerr << "Error: Current price not available for " << position.symbol << endl;
            position.unrealizedPL = 0.0;
        }
    }
}

// Function to display intraday positions with unrealized P&L
void displayIntradayPositions(const vector<IntradayPosition>& positions) {
    if (positions.empty()) {
        cout << "No intraday positions to display." << endl;
        return;
    }

    cout << "\n=== Intraday Positions ===" << endl;
    cout << "Timestamp: " << positions.front().timestamp << endl;
    cout << left << setw(10) << "Symbol"
         << right << setw(10) << "Quantity"
         << setw(15) << "Entry Price"
         << setw(15) << "Current Price"
         << setw(15) << "Unrealized P/L" << endl;
    cout << "-----------------------------------------------------" << endl;

    double totalUnrealizedPL = 0.0;
    for (const auto& position : positions) {
        cout << left << setw(10) << position.symbol
             << right << setw(10) << position.quantity
             << setw(15) << fixed << setprecision(2) << position.entryPrice
             << setw(15) << position.currentPrice
             << setw(15) << position.unrealizedPL << endl;
        totalUnrealizedPL += position.unrealizedPL;
    }

    cout << "-----------------------------------------------------" << endl;
    cout << left << setw(35) << "Total Unrealized P/L:"
         << right << setw(15) << totalUnrealizedPL << endl;
    cout << "\nPress 'Q' to return to the main menu." << endl << flush;
}

void runPositions(const string& username) {
    vector<IntradayPosition> positions;
    loadIntradayPositions(username, positions);

    if (positions.empty()) {
        cout << "No intraday positions to display." << endl;
        return; // Return to main menu
    }

    setNonBlockingMode(true); // Enable non-blocking mode

    while (true) {
        calculateUnrealizedPL(positions);
        system("clear");
        displayIntradayPositions(positions);

        // Sleep for 5 seconds while checking for user input
        for (int i = 0; i < 50; ++i) {
            char ch;
            if (read(STDIN_FILENO, &ch, 1) > 0) {
                if (ch == 'Q' || ch == 'q') {
                    setNonBlockingMode(false); // Disable non-blocking mode
                    system("clear");
                    return; // Exit the function
                }
            }
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }
    setNonBlockingMode(false); // Disable non-blocking mode before exiting
}
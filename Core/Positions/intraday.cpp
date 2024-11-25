#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>   // For formatting output
#include <sstream>   // For parsing strings
#include <cstdio>    // For popen()
#include <memory>    // For managing dynamic allocations
using namespace std;

struct IntradayPosition {
    string symbol;
    int quantity;       // Positive for long, negative for short
    double entryPrice;  // Buy price for long, sell price for short
    double currentPrice;
    double unrealizedPL;
};

// Function to call the external Python script and get the current market price
double getCurrentMarketPrice(const string& symbol) {
    string command = "/opt/anaconda3/bin/python ../APIs/api_AlphaVantage2.py " + symbol;
    char buffer[128];
    string result;

    // Use popen to execute the Python script and capture output
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        cerr << "Error: Failed to execute market price API." << endl;
        return 0.0;
    }

    // Read the output of the Python script
    while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
        result += buffer;
    }

    // Remove any trailing newline characters
    result.erase(result.find_last_not_of(" \n\r\t") + 1);

    try {
        // Convert the result to a double and return it
        return stod(result);
    } catch (const invalid_argument& e) {
        cerr << "Error: Invalid price received for " << symbol << ". Output was: " << result << endl;
        return 0.0;
    }
}

// Function to load intraday positions from a file
void loadIntradayPositions(const string& username, vector<IntradayPosition>& positions) {
    string filename = username + "_positions.dat";
    ifstream infile(filename);

    if (infile.is_open()) {
        string line;
        while (getline(infile, line)) {
            istringstream iss(line);
            IntradayPosition position;
            position.unrealizedPL = 0.0; // Initialize unrealized P&L

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
        // Get the current market price
        position.currentPrice = getCurrentMarketPrice(position.symbol);

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
}

int main() {
    // Assume the user has successfully logged in
    string username = "AJ"; // Replace with actual login function

    vector<IntradayPosition> positions;
    loadIntradayPositions(username, positions);

    // Calculate unrealized P&L for each intraday position
    calculateUnrealizedPL(positions);

    // Display the intraday positions with unrealized P&L
    displayIntradayPositions(positions);
}
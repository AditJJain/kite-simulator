#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>   // For formatting output
#include <sstream>   // For parsing strings
#include <algorithm> // For sorting and calculations
#include <map>       // For holding portfolio data
#include <numeric>   // For accumulating totals
#include <cstdio>    // For popen()
#include <memory>    // For managing dynamic allocations
using namespace std;

struct Holding {
    string symbol;
    int quantity;
    double averagePrice;
    double unrealizedPL;
    double currentPrice;
};

// Function to call the external Python script and get the current market price
double getCurrentMarketPrice(const string& symbol) {
    // Construct the command to call the Python script
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

// Function to load the portfolio from a file
void loadPortfolio(const string& username, vector<Holding>& portfolio) {
    string filename = username + "_portfolio.dat";
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
        // Get the current market price
        holding.currentPrice = getCurrentMarketPrice(holding.symbol);

        // If the current price is available, calculate unrealized P/L
        if (holding.currentPrice > 0.0) {
            holding.unrealizedPL = (holding.currentPrice - holding.averagePrice) * holding.quantity;
        } else {
            cout << "Current price for " << holding.symbol << " not available." << endl;
            holding.unrealizedPL = 0.0;
        }
    }
}

// Function to display the user's portfolio with only unrealized P/L and CMP
void displayPortfolio(const vector<Holding>& portfolio) {
    if (portfolio.empty()) {
        cout << "Your portfolio is currently empty." << endl;
        return;
    }

    cout << "\n=== Your Portfolio ===" << endl;
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
}

int main() {
    // Assume the user has successfully logged in
    string username = "AJ"; // Replace with actual login function

    vector<Holding> portfolio;
    loadPortfolio(username, portfolio);

    // Calculate unrealized P/L for each holding in the portfolio and get the current market price
    calculateUnrealizedPL(portfolio);

    // Display the user's portfolio with CMP and unrealized P/L
    displayPortfolio(portfolio);

    // Display portfolio allocation and diversification
    displayAllocation(portfolio);
}
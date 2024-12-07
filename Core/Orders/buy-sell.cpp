#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <cctype>
#include <utility>
#include <cstdlib> // For system()
#include "Core/holding.h"
#include "Core/intradayPosition.h"
#include "Core/APIs/apicall_KiteConnect_LTP.h"
#include "Core/TradeHistory/tradehistory.h"
using namespace std;

// Utility function to trim strings
string trim(const string& str) {
    auto start = str.find_first_not_of(" \t");
    auto end = str.find_last_not_of(" \t");
    return (start == string::npos) ? "" : str.substr(start, end - start + 1);
}

// Check if there are enough funds
bool checkFunds(double amount, const string& username) {
    ifstream infile("Core/Funds/" + username + "_balance.csv");
    if (!infile.is_open()) {
        cerr << "Error: Unable to open funds file.\n";
        return false;
    }
    double balance = 0.0;
    infile >> balance;
    infile.close();
    return balance >= amount;
}

// Modify user's funds (add or deduct)
void modifyFunds(double amount, const string& username) {
    ifstream infile("Core/Funds/" + username + "_balance.csv");
    if (!infile.is_open()) {
        cerr << "Error: Unable to open funds file.\n";
        return;
    }
    double balance = 0.0;
    infile >> balance;
    infile.close();

    balance += amount;

    ofstream outfile("Core/Funds/" + username + "_balance.csv");
    if (!outfile.is_open()) {
        cerr << "Error: Unable to open funds file for writing.\n";
        return;
    }
    outfile << balance;
    outfile.close();
}

// Add a new entry to the portfolio
void addPortfolioEntry(const string& username, const string& symbol, int quantity, double price) {
    string portfolioFile = "Core/Portfolio/" + username + "_portfolio.dat";
    vector<Holding> holdings;
    bool found = false;

    // Read existing portfolio
    ifstream infile(portfolioFile);
    if (infile.is_open()) {
        string line;
        while (getline(infile, line)) {
            istringstream iss(line);
            Holding holding;
            if (iss >> holding.symbol >> holding.quantity >> holding.averagePrice) {
                if (holding.symbol == symbol) {
                    holding.quantity += quantity;
                    holding.averagePrice = ((holding.averagePrice * (holding.quantity - quantity)) + (price * quantity)) / holding.quantity;
                    found = true;
                }
                holdings.push_back(holding);
            }
        }
        infile.close();
    }

    // If not found, add new holding
    if (!found) {
        Holding newHolding = {symbol, quantity, price};
        holdings.push_back(newHolding);
    }

    // Write updated portfolio
    ofstream outfile(portfolioFile, ios::trunc);
    if (!outfile.is_open()) {
        cerr << "Error: Unable to open portfolio file for writing.\n";
        return;
    }
    for (const auto& holding : holdings) {
        outfile << holding.symbol << " " << holding.quantity << " " << holding.averagePrice << endl;
    }
    outfile.close();
}

// Remove an entry from the portfolio
bool removePortfolioEntry(const string& username, const string& symbol, int quantity) {
    string portfolioFile = "Core/Portfolio/" + username + "_portfolio.dat";
    ifstream infile(portfolioFile);
    if (!infile.is_open()) {
        cerr << "Error: Unable to open portfolio file.\n";
        return false;
    }

    ofstream tempFile("Core/Portfolio/temp.dat");
    if (!tempFile.is_open()) {
        cerr << "Error: Unable to open temporary file.\n";
        infile.close();
        return false;
    }

    bool found = false;
    string line;
    while (getline(infile, line)) {
        istringstream iss(line);
        Holding holding;
        if (iss >> holding.symbol >> holding.quantity >> holding.averagePrice) {
            if (holding.symbol == symbol) {
                if (holding.quantity >= quantity) {
                    holding.quantity -= quantity;
                    found = true;
                }
                if (holding.quantity > 0) {
                    tempFile << holding.symbol << " " << holding.quantity << " " << holding.averagePrice << endl;
                }
            } else {
                tempFile << holding.symbol << " " << holding.quantity << " " << holding.averagePrice << endl;
            }
        }
    }

    infile.close();
    tempFile.close();

    if (!found) {
        cerr << "Error: Stock not found or insufficient quantity.\n";
        remove("Core/Portfolio/temp.dat");
        return false;
    }

    remove(portfolioFile.c_str());
    rename("Core/Portfolio/temp.dat", portfolioFile.c_str());
    return true;
}

// Add or update a position entry
void addPositionEntry(const std::string& username, const std::string& symbol, int quantity, double price, const std::string& timestamp) {
    std::string positionsFile = "Core/Positions/" + username + "_positions.dat";
    std::vector<IntradayPosition> positions;
    bool found = false;

    // Read existing positions
    std::ifstream infile(positionsFile);
    if (infile.is_open()) {
        std::string line;
        while (std::getline(infile, line)) {
            std::istringstream iss(line);
            IntradayPosition pos;
            if (iss >> pos.symbol >> pos.quantity >> pos.entryPrice >> pos.currentPrice >> pos.unrealizedPL >> pos.timestamp) {
                if (pos.symbol == symbol) {
                    pos.quantity += quantity;
                    pos.entryPrice = ((pos.entryPrice * (pos.quantity - quantity)) + (price * quantity)) / pos.quantity;
                    pos.timestamp = timestamp; // Update timestamp if needed
                    // Optionally update currentPrice and unrealizedPL if applicable
                    found = true;
                }
                positions.push_back(pos);
            }
        }
        infile.close();
    }

    // If not found, add new position
    if (!found) {
        IntradayPosition newPos;
        newPos.symbol = symbol;
        newPos.quantity = quantity;
        newPos.entryPrice = price;
        newPos.currentPrice = price; // Initialize currentPrice
        newPos.unrealizedPL = 0.0;  // Initialize unrealized P/L
        newPos.timestamp = timestamp;
        positions.push_back(newPos);
    }

    // Write updated positions
    std::ofstream outfile(positionsFile, std::ios::trunc);
    if (!outfile.is_open()) {
        std::cerr << "Error: Unable to open positions file for writing.\n";
        return;
    }
    for (const auto& pos : positions) {
        outfile << pos.symbol << " "
                << pos.quantity << " "
                << pos.entryPrice << " "
                << pos.currentPrice << " "
                << pos.unrealizedPL << " "
                << pos.timestamp << std::endl;
    }
    outfile.close();
}

tuple<string, int, double> buyStock(const string& username) {
    string symbol, orderType;
    int quantity;

    cout << "Enter instrument symbol: ";
    cin >> symbol;

    auto [price, timestamp] = getCurrentMarketPrice(symbol); // Capture timestamp

    if (price <= 0) {
        cerr << "Error: Failed to retrieve stock price.\n";
        return {};  // Return an empty tuple
    }

    cout << "Current price for " << symbol << " (at " << timestamp << ")" << ": ₹" << price << endl;
    cout << "Enter quantity: ";
    cin >> quantity;

    double totalCost = price * quantity;
    cout << "Enter order type (CNC/NRML/MIS): ";
    cin >> orderType;
    transform(orderType.begin(), orderType.end(), orderType.begin(), ::toupper);

    if (orderType == "MIS") totalCost /= 5;

    if (!checkFunds(totalCost, username)) {
        cerr << "Error: Insufficient funds.\n";
        return {};  // Return an empty tuple
    }

    modifyFunds(-totalCost, username);

    if ((orderType == "CNC" || orderType == "NRML") && quantity > 0) {
        addPortfolioEntry(username, symbol, quantity, price);
        cout << "Bought " << quantity << " shares of " << symbol << " (CNC/NRML) at ₹" << price << ".\n";
    } else if (quantity > 0) {
        addPositionEntry(username, symbol, quantity, price, timestamp);
        cout << "Bought " << quantity << " shares of " << symbol << " (MIS) at ₹" << price << ".\n";
    }

    // Log the trade history for the buy transaction
    logTradeHistory(username, "Buy", symbol, quantity, price);

    return {symbol, quantity, price}; // Return trade details
}

tuple<string, int, double> sellStock(const string& username) {
    string symbol, orderType;
    int quantity;

    cout << "Enter stock symbol to sell: ";
    cin >> symbol;
    cout << "Enter quantity to sell: ";
    cin >> quantity;
    cout << "Enter order type (CNC/NRML/MIS): ";
    cin >> orderType;
    transform(orderType.begin(), orderType.end(), orderType.begin(), ::toupper);

    auto [price, timestamp] = getCurrentMarketPrice(symbol);

    if (price <= 0) {
        cerr << "Error: Failed to retrieve stock price.\n";
        return {}; // Return an empty tuple
    }

    double totalProceeds = price * quantity;
    bool success = false;

    if ((orderType == "CNC" || orderType == "NRML") && quantity > 0) {
        if (removePortfolioEntry(username, symbol, quantity)) {
            modifyFunds(totalProceeds, username);
            cout << "Sold " << quantity << " shares of " << symbol << " (CNC) at ₹" << price << ".\n";
            success = true;
        }
    } else if (quantity > 0) {
        addPositionEntry(username, symbol, -quantity, price, timestamp);
        modifyFunds(totalProceeds, username);
        cout << "Shorted " << quantity << " shares of " << symbol << " (MIS) at ₹" << price << ".\n";
        success = true;
    }

    // Log only if the transaction was successful
    if (success) {
        logTradeHistory(username, "Sell", symbol, quantity, price);
    } else {
        cerr << "Error: Could not complete the sell transaction.\n";
    }

    return success ? make_tuple(symbol, quantity, price) : tuple<string, int, double>{};
}


void runBuySell(const string& username) {
    char choice = '0';

    do {
        system("clear");
        cout << "User: " << username << endl;
        cout << "\n=== Buy/Sell Menu ===\n";
        cout << "1. Buy Instrument\n2. Sell Instrument\nQ. Exit to Main Menu" << endl;
        cout << "\nEnter your choice (1-2 or Q): ";
        cin >> choice;
        cin.clear(); // Clear error flags
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore remaining input

        switch (toupper(choice)) {
            case '1': {
                auto [symbol, quantity, price] = buyStock(username);  // Capture return values
                // No need to log here, it's already logged inside buyStock()
                break;
            }
            case '2': {
                auto [symbol, quantity, price] = sellStock(username);  // Capture return values
                // No need to log here, it's already logged inside sellStock()
                break;
            }
            case 'Q':
                system("clear");
                break;
            default:
                cout << "Invalid choice. Enter 1, 2, or Q.\n";
                break;
        }

        if (toupper(choice) != 'Q') {
            this_thread::sleep_for(std::chrono::seconds(5));
        }
    } while (toupper(choice) != 'Q');
}
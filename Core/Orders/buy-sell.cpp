#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "Core/holding.h"
#include "Core/intradayPosition.h"
#include "Core/APIs/apicall_KiteConnect_LTP.h"
#include "Core/Orders/getLotSize.h"
#include "Core/TradeHistory/tradehistory.h"
using namespace std;

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

void addPositionEntry(const string& username, const string& symbol, int quantity, double price) {
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
                << pos.entryPrice << " ";
    }
    outfile.close();
}

void removePositionEntry(const string& username, const string& symbol, int quantity) {
    string positionsFile = "Core/Positions/" + username + "_positions.dat";
    ifstream infile(positionsFile);
    if (!infile.is_open()) {
        cerr << "Error: Unable to open positions file.\n";
    }

    ofstream tempFile("Core/Positions/temp.dat");
    if (!tempFile.is_open()) {
        cerr << "Error: Unable to open temporary file.\n";
        infile.close();
    }

    bool found = false;
    string line;
    while (getline(infile, line)) {
        istringstream iss(line);
        IntradayPosition pos;
        if (iss >> pos.symbol >> pos.quantity >> pos.entryPrice) {
            if (pos.symbol == symbol) {
                if (pos.quantity >= quantity) {
                    pos.quantity -= quantity;
                    found = true;
                }
                if (pos.quantity > 0) {
                    tempFile << pos.symbol << " " << pos.quantity << " " << pos.entryPrice << "\n";
                }
            } else {
                tempFile << pos.symbol << " " << pos.quantity << " " << pos.entryPrice << "\n";
            }
        }
    }

    infile.close();
    tempFile.close();

    if (!found) {
        cerr << "Error: Position not found or insufficient quantity.\n";
        remove("Core/Positions/temp.dat");
    }

    remove(positionsFile.c_str());
    rename("Core/Positions/temp.dat", positionsFile.c_str());
}

void buyStock(const string& username) {
    string symbol, orderType;
    int quantity;

    cout << "Enter instrument symbol: ";
    cin >> symbol;
    transform(symbol.begin(), symbol.end(), symbol.begin(), ::toupper);
    auto [price, timestamp] = getCurrentMarketPrice(symbol); // Capture timestamp

    if (price <= 0) {
        cerr << "Error: Failed to retrieve stock price.\n";
        return;
    }

    cout << "Current price for " << symbol << " (at " << timestamp << ")" << ": ₹" << price << endl;
    cout << "Minimum quantity for " << symbol << ": " << getLotSize(symbol) << endl;
    cout << "Enter quantity: ";
    cin >> quantity;

    if (quantity < getLotSize(symbol)) {
        cerr << "Error: Quantity is less than the lot size.\n";
        return;
    } else if (quantity % getLotSize(symbol) != 0) {
        cerr << "Error: Quantity is not a multiple of the lot size.\n";
        return;
    }

    cout << "Enter order type (CNC/NRML/MIS): ";
    cin >> orderType;
    transform(orderType.begin(), orderType.end(), orderType.begin(), ::toupper);
    
    auto [finalPrice, finalTimestamp] = getCurrentMarketPrice(symbol); // Capture timestamp
    double totalCost = finalPrice * quantity;

    if (orderType == "CNC" || orderType == "NRML") {
        cout << "Final price for " << quantity << " shares of " << symbol << " (at " << finalTimestamp << "): ₹" << totalCost << endl;
    } else if (orderType == "MIS") {
        totalCost /= 5;
        cout << "Final price for " << quantity << " shares of " << symbol << " (at " << finalTimestamp << "): ₹" << totalCost << " (MIS - 5x Leverage)\n";
    }
    
    cout << "Do you want to proceed with the order? (Y/N): ";
    char confirm; cin >> confirm;

    if (toupper(confirm) != 'Y') {
        cout << "Order cancelled.\n";
        return;
    }

    if (!checkFunds(totalCost, username)) {
        cerr << "Error: Insufficient funds.\n";
        return;
    }

    if ((orderType == "CNC" || orderType == "NRML") && quantity > 0) {
        addPortfolioEntry(username, symbol, quantity, price);
        modifyFunds(-totalCost, username);
        logTradeHistory(username, "Buy", symbol, quantity, price);
        cout << "Bought " << quantity << " shares of " << symbol << " (CNC/NRML) at ₹" << price << ".\n";
    } else if (quantity > 0) {
        addPositionEntry(username, symbol, quantity, price);
        modifyFunds(-totalCost, username);
        logTradeHistory(username, "Buy", symbol, quantity, price);
        cout << "Bought " << quantity << " shares of " << symbol << " (MIS) at ₹" << price << ".\n";
    }
}

void sellStock(const string& username) {
    string symbol, orderType;
    int quantity;

    cout << "Enter stock symbol to sell: ";
    cin >> symbol;
    transform(symbol.begin(), symbol.end(), symbol.begin(), ::toupper);

    auto [price, timestamp] = getCurrentMarketPrice(symbol); // Capture timestamp

    if (price <= 0) {
        cerr << "Error: Failed to retrieve stock price.\n";
    }

    cout << "Current price for " << symbol << " (at " << timestamp << ")" << ": ₹" << price << endl;
    cout << "Enter quantity: ";
    cin >> quantity;

    cout << "Enter order type (CNC/NRML/MIS): ";
    cin >> orderType;
    transform(orderType.begin(), orderType.end(), orderType.begin(), ::toupper);

    if(orderType == "MIS" || orderType == "NRML") { // We only check against the lot size for MIS/NRML because the user is shorting an instrument. For CNC, the user is selling from their portfolio.
        int lotSize = getLotSize(symbol);
        if (quantity < lotSize) {
            cerr << "Error: Quantity is less than the lot size.\n";
            cout << "Lot size for " << symbol << ": " << lotSize << endl;
            return;
        } else if (quantity % lotSize != 0) {
            cerr << "Error: Quantity is not a multiple of the lot size.\n";
            cout << "Lot size for " << symbol << ": " << lotSize << endl;
            return;
        }
    }

    auto [finalPrice, finalTimestamp] = getCurrentMarketPrice(symbol);
    double totalProceeds = finalPrice * quantity;

    if (orderType == "CNC") {
        cout << "Final proceeds for " << quantity << " shares of " << symbol << " (at " << finalTimestamp << "): ₹" << totalProceeds << endl;
    } else if (orderType == "MIS") {
        totalProceeds /= 5;
        cout << "Final proceeds for " << quantity << " shares of " << symbol << " (at " << finalTimestamp << "): ₹" << totalProceeds << " (MIS - 5x Leverage)\n";
    }
    cout << "Do you want to proceed with the order? (Y/N): ";
    char confirm; cin >> confirm;

    if(toupper(confirm) != 'Y') {
        cout << "Order cancelled.\n";
        return;
    }

    if ((orderType == "CNC") && quantity > 0) {
        if (removePortfolioEntry(username, symbol, quantity)) {
            modifyFunds(totalProceeds, username);
            logTradeHistory(username, "Sell", symbol, quantity, price);
            cout << "Sold " << quantity << " shares of " << symbol << " (CNC) at ₹" << price << ".\n";
        }
    } else if (quantity > 0) {
        removePositionEntry(username, symbol, quantity);
        modifyFunds(totalProceeds, username);
        logTradeHistory(username, "Sell", symbol, quantity, price);
        cout << "Sold " << quantity << " shares of " << symbol << " (MIS) at ₹" << price << ".\n";
    }
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
                buyStock(username);
                break;
            }
            case '2': {
                sellStock(username);
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
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm> // For find
#include <cctype>    // For toupper
#include <iomanip>   // For formatting output
#include <cstdio>    // For popen()
#include <memory>    // For managing dynamic allocations
using namespace std;

// Function declarations
void watchlistMenu(const string& username);
void loadWatchlist(const string& username, vector<string>& watchlist);
void saveWatchlist(const string& username, const vector<string>& watchlist);
void displayWatchlist(const vector<string>& watchlist);
void addStockToWatchlist(vector<string>& watchlist);
void removeStockFromWatchlist(vector<string>& watchlist);
double getCurrentMarketPrice(const string& symbol);
string toUpperCase(const string& str);

int main() {
    // Assume the user has successfully logged in
    string username = "AJ"; // Replace with actual login function

    watchlistMenu(username);
}

// Watchlist menu
void watchlistMenu(const string& username) {
    vector<string> watchlist;
    loadWatchlist(username, watchlist);

    int choice = 0;
    do {
        cout << "\n=== Watchlist Menu for User: " << username << " ===" << endl;
        cout << "1. View Watchlist" << endl;
        cout << "2. Add Stock to Watchlist" << endl;
        cout << "3. Remove Stock from Watchlist" << endl;
        cout << "4. Return to Main Menu" << endl;
        cout << "Enter your choice (1-4): ";
        cin >> choice;

        // Clear the input buffer to handle any leftover input
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                displayWatchlist(watchlist);
                break;
            case 2:
                addStockToWatchlist(watchlist);
                saveWatchlist(username, watchlist);
                break;
            case 3:
                removeStockFromWatchlist(watchlist);
                saveWatchlist(username, watchlist);
                break;
            case 4:
                cout << "Returning to Main Menu..." << endl;
                break;
            default:
                cout << "Invalid choice. Please enter a number between 1 and 4." << endl;
        }
    } while (choice != 4);
}

// Function to load the watchlist from a file
void loadWatchlist(const string& username, vector<string>& watchlist) {
    string filename = username + "_watchlist.txt";
    ifstream infile(filename);

    if (infile.is_open()) {
        string symbol;
        while (getline(infile, symbol)) {
            if (!symbol.empty()) {
                watchlist.push_back(symbol);
            }
        }
        infile.close();
    } else {
        // Watchlist file doesn't exist; start with an empty watchlist
        watchlist.clear();
    }
}

// Function to save the watchlist to a file
void saveWatchlist(const string& username, const vector<string>& watchlist) {
    string filename = username + "_watchlist.txt";
    ofstream outfile(filename);

    if (outfile.is_open()) {
        for (const auto& symbol : watchlist) {
            outfile << symbol << endl;
        }
        outfile.close();
    } else {
        cerr << "Error: Unable to save watchlist to file for user " << username << "." << endl;
    }
}

// Function to call the Python file to get the CMP for a stock symbol
double getCurrentMarketPrice(const string& symbol) {
    string command = "/opt/anaconda3/bin/python ../APIs/api_AlphaVantage_LTP.py " + symbol;
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

// Function to display the watchlist with CMP
void displayWatchlist(const vector<string>& watchlist) {
    if (watchlist.empty()) {
        cout << "Your watchlist is currently empty." << endl;
        return;
    }

    cout << "\n=== Your Watchlist ===" << endl;
    cout << left << setw(10) << "Symbol" << setw(15) << "Current Price" << endl;
    cout << "----------------------------" << endl;

    for (const auto& symbol : watchlist) {
        double cmp = getCurrentMarketPrice(symbol);
        if (cmp > 0.0) {
            cout << left << setw(10) << symbol << setw(15) << fixed << setprecision(2) << cmp << endl;
        } else {
            cout << left << setw(10) << symbol << "Price Not Available" << endl;
        }
    }
}

// Function to add a stock to the watchlist
void addStockToWatchlist(vector<string>& watchlist) {
    string symbol;
    cout << "Enter the stock symbol to add: ";
    getline(cin, symbol);

    symbol = toUpperCase(symbol);

    if (symbol.empty()) {
        cout << "Stock symbol cannot be empty." << endl;
        return;
    }

    if (find(watchlist.begin(), watchlist.end(), symbol) != watchlist.end()) {
        cout << "Stock " << symbol << " is already in your watchlist." << endl;
    } else {
        watchlist.push_back(symbol);
        cout << "Stock " << symbol << " has been added to your watchlist." << endl;
    }
}

// Function to remove a stock from the watchlist
void removeStockFromWatchlist(vector<string>& watchlist) {
    string symbol;
    cout << "Enter the stock symbol to remove: ";
    getline(cin, symbol);

    symbol = toUpperCase(symbol);

    auto it = find(watchlist.begin(), watchlist.end(), symbol);
    if (it != watchlist.end()) {
        watchlist.erase(it);
        cout << "Stock " << symbol << " has been removed from your watchlist." << endl;
    } else {
        cout << "Stock " << symbol << " is not in your watchlist." << endl;
    }
}

// Helper function to convert a string to uppercase
string toUpperCase(const string& str) {
    string result;
    for (char c : str) {
        result += toupper(static_cast<unsigned char>(c));
    }
    return result;
}
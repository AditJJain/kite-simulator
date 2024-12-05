#include <iostream>
#include <fstream>
#include <iomanip>   // For output formatting
#include <sstream>   // For string manipulation
#include <chrono>    // For time functions
#include "Core/Orders/buy-sell.h"            // Header for buy/sell functions
#include "Core/intradayPosition.h"  
#include <sys/stat.h>
#include <ctime>
using namespace std;

// Function to get current timestamp
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&currentTime));
    return std::string(buffer);
}

// Ensures the directory exists or creates it
void createDirectoryIfNotExist(const string& dirPath) {
    struct stat info;
    if (stat(dirPath.c_str(), &info) != 0) {
        if (mkdir(dirPath.c_str(), 0777) == -1) {
            cerr << "Error: Unable to create directory " << dirPath << endl;
        }
    }
}

// Log trade history to a file
void logTradeHistory(const string& username, const string& action, const string& symbol, int quantity, double price) {
    string timestamp = getCurrentTimestamp();
    string historyFile = "Core/TradeHistory/" + username + "_history.log";

    createDirectoryIfNotExist("Core/TradeHistory");  // Ensure directory exists

    ofstream outfile(historyFile, ios::app); // Append to the history file
    if (!outfile.is_open()) {
        cerr << "Error: Unable to open trade history file: " << historyFile << endl;
        return;
    }

    outfile << timestamp << " | " << action << " | " << symbol << " | Quantity: " << quantity << " | Price: ₹" << fixed << setprecision(2) << price << endl;
    outfile.close();
    cout << "Trade history written successfully." << endl;  // Debug output
}

// Displays the trade history for a given user
void showTradeHistory(const string& username) {
    string historyFile = "Core/TradeHistory/" + username + "_history.log";

    // Check if file exists, if not create it
    ifstream infile(historyFile);
    if (!infile.is_open()) {
        // Create a new empty file if it doesn't exist
        ofstream newFile(historyFile);
        if (!newFile) {
            cerr << "Error: Unable to create trade history file.\n";
            return;
        } else {
            cout << "New trade history file created for user " << username << endl;
        }
        newFile.close(); // Close the newly created file
        return;
    }

    // Display the content of the file
    cout << "\n=== Trade History for " << username << " ===\n";
    cout << left << setw(20) << "Timestamp"
         << setw(10) << "Action"
         << setw(10) << "Symbol"
         << setw(10) << "Quantity"
         << setw(15) << "Price" << endl;
    cout << "----------------------------------------------------------\n";

    string line;
    while (getline(infile, line)) {
        cout << line << endl;
    }
    infile.close();
    cout << "\nPress 'Q' to return to the main menu." << endl;
}

// Displays the trade history for a user interactively
void runTradeHistory(const string& username) {
    char choice;

    do {
        showTradeHistory(username);
        cout << "\nEnter 'Q' to quit or any other key to refresh: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer

    } while (toupper(choice) != 'Q');
}


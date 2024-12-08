#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>   // For output formatting
#include <iostream>
#include <sstream>   // For string manipulation
#include <sys/stat.h>
#include <termios.h>
#include <thread>
#include <unistd.h>
#include "Core/Orders/buy-sell.h"
#include "Core/setNonBlockingMode.h"
using namespace std;

// Function to get current timestamp
string getCurrentTimestamp() {
    auto now = chrono::system_clock::now();
    time_t currentTime = chrono::system_clock::to_time_t(now);
    char buffer[100];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&currentTime));
    return string(buffer);
}

// Log trade history to a file
void logTradeHistory(const string& username, const string& action, const string& symbol, int quantity, double price) {
    string timestamp = getCurrentTimestamp();
    string historyFile = "Core/TradeHistory/" + username + "_history.log";

    ofstream outfile(historyFile, ios::app); // Append to the history file
    if (!outfile.is_open()) {
        cerr << "Error: Unable to open trade history file: " << historyFile << endl;
        return;
    }

    outfile << timestamp << " | " << action << " | " << symbol << " | Quantity: " << quantity << " | Price: ₹" << fixed << setprecision(2) << price << endl;
    outfile.close();
}

// Displays the trade history for a given user
void runTradeHistory(const string& username) {
    string historyFile = "Core/TradeHistory/" + username + "_history.log";

    setNonBlockingMode(true); // Enable non-blocking mode
    while (true) {
        cout << "\033[2J\033[H"; // Clear screen and move cursor to top-left
        
        ifstream infile(historyFile);

        // Display the content of the file
        cout << "\033[2J\033[H"; // Clear screen and move cursor to top-left
        cout << "User: " << username << endl;
        cout << "\n=== Trade History ===" << endl;
        cout << left << setw(20) << "Timestamp"
            << setw(10) << "Action"
            << setw(10) << "Symbol"
            << setw(10) << "Quantity"
            << setw(15) << "Price" << endl;

        string line;
        while (getline(infile, line)) {
            cout << line << endl;
        }
        infile.close();
        cout << "----------------------------------------------------------\n";
        cout << "\nPress 'Q' to return to the main menu." << endl << flush;

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
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }
    setNonBlockingMode(false); // Disable non-blocking mode before exiting
}
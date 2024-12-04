#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>    // For find
#include <cctype>       // For toupper
#include <iomanip>      // For formatting output
#include <cstdio>       // For popen()
#include <memory>       // For managing dynamic allocations
#include <thread>       // For sleep
#include <chrono>       // For time
#include <termios.h>    // For non-blocking input
#include <unistd.h>     // For read
#include <sys/select.h> // For select
#include "Core/APIs/apicall_KiteConnect_LTP.h"
using namespace std;

// Helper function to convert a string to uppercase
string toUpperCase(const string& str) {
    string result;
    for (char c : str) {
        result += toupper(static_cast<unsigned char>(c));
    }
    return result;
}

// Helper functions for non-blocking input
struct termios orig_termios;

void resetTerminalMode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

void setTerminalMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(resetTerminalMode);

    struct termios new_termios = orig_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO); // Disable buffering and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

// Check if a key has been pressed and store it in 'c'
bool isKeyPressed(char& c) {
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv)) {
        read(STDIN_FILENO, &c, 1);
        return true;
    }
    return false;
}

// Function to load the watchlist from a file
void loadWatchlist(const string& username, vector<string>& watchlist) {
    string filename = "Core/Watchlist/" + username + "_watchlist.txt";
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
    string filename = "Core/Watchlist/" + username + "_watchlist.txt";
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

void displayWatchlist(const vector<string>& watchlist) {
    if (watchlist.empty()) {
        cout << "Your watchlist is currently empty." << endl;
        return;
    }

    setTerminalMode();

    // Initial display
    cout << "\033[2J\033[H"; // Clear screen and move cursor to top-left

    // Print headers
    cout << "=== Your Watchlist ===" << endl;
    cout << "Timestamp: " << "N/A" << endl; // Placeholder, will update later
    cout << left << setw(10) << "Symbol"
         << right << setw(15) << "Current Price" << endl;
    cout << "----------------------------" << endl;

    // Initial data display
    for (const auto& symbol : watchlist) {
        auto priceTimestamp = getCurrentMarketPrice(symbol); // Modify this function to return a pair<double, string>
        double cmp = priceTimestamp.first;
        string timestamp = priceTimestamp.second;

        if (cmp > 0.0) {
            cout << left << setw(10) << symbol
                 << right << setw(15) << fixed << setprecision(2) << cmp << endl;
        } else {
            cout << left << setw(10) << symbol
                 << right << setw(15) << "Price N/A" << endl;
        }
    }

    cout << "\nPress 'Q' to return to the Watchlist Menu." << endl;

    while (true) {
        string lastTimestamp;

        // Update each stock's current price
        for (size_t i = 0; i < watchlist.size(); ++i) {
            auto priceTimestamp = getCurrentMarketPrice(watchlist[i]);
            double cmp = priceTimestamp.first;
            string timestamp = priceTimestamp.second;
            lastTimestamp = timestamp; // Update the timestamp

            // Move cursor to the specific stock line
            int line = 4 + i + 1;

            cout << "\033[" << line << ";1H"; // Move cursor to line

            if (cmp > 0.0) {
                cout << left << setw(10) << watchlist[i]
                     << right << setw(15) << fixed << setprecision(2) << cmp << " ";
            } else {
                cout << left << setw(10) << watchlist[i]
                     << right << setw(15) << "Price N/A" << " ";
            }
        }

        // Update the timestamp display
        cout << "\033[2;1H"; // Move cursor to the second line
        cout << "Timestamp: " << setw(20) << lastTimestamp << "   ";

        // Display exit instruction
        cout << "\033[" << (4 + watchlist.size() + 2) << ";1H"
             << "Press 'Q' to return to the Watchlist Menu." << endl << flush;

        // Wait for 5 seconds or until 'Q' is pressed
        auto start = chrono::steady_clock::now();
        while (chrono::steady_clock::now() - start < chrono::seconds(5)) {
            char c;
            if (isKeyPressed(c)) {
                if (tolower(c) == 'q') {
                    cout << "\nReturning to Watchlist Menu..." << endl;
                    resetTerminalMode();
                    return;
                }
            }
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }
}

void addStock(vector<string>& watchlist) {
    string symbol;
    cout << "Enter the instrument symbol to add: ";
    getline(cin, symbol);

    symbol = toUpperCase(symbol);

    if (symbol.empty()) {
        cout << "Instrument symbol cannot be empty." << endl;
        this_thread::sleep_for(std::chrono::seconds(5));
        return;
    }

    if (find(watchlist.begin(), watchlist.end(), symbol) != watchlist.end()) {
        cout << "Instrument " << symbol << " is already in your watchlist." << endl;
        this_thread::sleep_for(std::chrono::seconds(5));
    } else {
        watchlist.push_back(symbol);
        cout << "Instrument " << symbol << " has been added to your watchlist." << endl;
        this_thread::sleep_for(std::chrono::seconds(5));
    }
}

void removeStock(vector<string>& watchlist) {
    string symbol;
    cout << "Enter the instrument symbol to remove: ";
    getline(cin, symbol);

    symbol = toUpperCase(symbol);

    auto it = find(watchlist.begin(), watchlist.end(), symbol);
    if (it != watchlist.end()) {
        watchlist.erase(it);
        cout << "Instrument " << symbol << " has been removed from your watchlist." << endl;
        this_thread::sleep_for(std::chrono::seconds(5));
    } else {
        cout << "Instrument " << symbol << " is not in your watchlist." << endl;
        this_thread::sleep_for(std::chrono::seconds(5));
    }
}

// Watchlist menu
void runWatchlist(const string& username) {
    vector<string> watchlist;
    loadWatchlist(username, watchlist);

    char choice = 0;
    do {
        cout << "\033[2J\033[H";
        cout << "User: " << username << endl;
        cout << "\n=== Watchlist Menu ===" << endl;
        cout << "1. View Watchlist" << endl;
        cout << "2. Add Instrument to Watchlist" << endl;
        cout << "3. Remove Instrument from Watchlist" << endl;
        cout << "Q. Return to Main Menu" << endl;
        cout << "\nEnter your choice (1-3 or Q): ";
        cin >> choice;
        cin.clear();
        // Clear the input buffer to handle any leftover input
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (toupper(choice)) {
            case '1':
                displayWatchlist(watchlist);
                break;
            case '2':
                addStock(watchlist);
                saveWatchlist(username, watchlist);
                break;
            case '3':
                removeStock(watchlist);
                saveWatchlist(username, watchlist);
                break;
            case 'Q':
                system("clear");
                break;
            default:
                cout << "Invalid choice. Please enter a number between 1 and 4." << endl;
                this_thread::sleep_for(chrono::seconds(3));
                system("clear");
        }
    } while (toupper(choice) != 'Q');
}
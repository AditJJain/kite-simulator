#include <iostream>
#include <string>
#include <cstdlib> // For system()
#include <thread>
#include <chrono>
#include <iomanip> // For formatting
#include <cstdio>  // For popen, pclose
#include <unistd.h> // For read()
#include <fcntl.h>  // For fcntl()
#include "Core/setNonBlockingMode.h"
using namespace std;

void viewMarketData(const string& username) {
    string stockSymbol;
    cout << "Enter stock symbol: ";
    cin >> stockSymbol;
    
    char choice;
    cout << "Would you like to see the chart? (Y/N): ";
    cin >> choice;

    // Function to fetch OHLC data and timestamp
    auto fetchOHLC = [&](const string& symbol) -> string {
        string command = "/opt/anaconda3/bin/python Core/APIs/api_KiteConnectWS_OHLC_Once.py " + symbol;
        FILE* pipe = popen(command.c_str(), "r");
        if (!pipe) {
            return "Error: Failed to execute Python script.\n";
        }
        char buffer[128];
        string result;
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        pclose(pipe);
        return result;
    };

    setNonBlockingMode(true); // Enable non-blocking mode

    if (choice == 'Y' || choice == 'y') {
        string url = "https://in.tradingview.com/chart/?symbol=" + stockSymbol;

        #ifdef __APPLE__
            string openCommand = "open " + url;
        #elif __linux__
            string openCommand = "xdg-open " + url;
        #else
            cerr << "Error: Unsupported platform.\n";
            setNonBlockingMode(false);
            return;
        #endif

        system(openCommand.c_str());
        setNonBlockingMode(false);
        return;
    }

    while (true) {
        // Fetch updated OHLC data each loop iteration
        string ohlcData = fetchOHLC(stockSymbol);

        // Clear the screen
        cout << "\033[H\033[J";
        cout << "User: " << username << endl;
        cout << "\n=== Market Data ===" << endl;
        cout << ohlcData << endl;
        cout << "Press 'Q' to return to the main menu." << endl << flush;

        // Wait for 5 seconds and check for 'Q' input periodically
        for (int i = 0; i < 50; ++i) { // Check for input every 100 ms (50 * 100ms = 5 seconds)
            char ch;
            if (read(STDIN_FILENO, &ch, 1) > 0) {
                if (toupper(ch) == 'Q') {
                    setNonBlockingMode(false); // Reset to blocking mode
                    system("clear");
                    return;
                }
            }
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }
}
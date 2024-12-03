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
        string result = "";
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        pclose(pipe);
        return result;
    };

    setNonBlockingMode(true); // Enable non-blocking mode

    while (true) {
        // Clear the screen
        cout << "\033[H\033[J";

        if (choice == 'Y' || choice == 'y') {
            string url = "https://in.tradingview.com/chart/?symbol=" + stockSymbol;

#ifdef _WIN32
            string openCommand = "start " + url;
#elif __APPLE__
            string openCommand = "open " + url;
#elif __linux__
            string openCommand = "xdg-open " + url;
#else
            cerr << "Error: Unsupported platform.\n";
            break;
#endif

        // Execute the command to open the browser
        int openResult = system(openCommand.c_str());
        if (openResult != 0) {
                cerr << "Error: Failed to open the TradingView chart.\n";
                break;
            } else {
                cout << "Opening the chart for " << stockSymbol << "...\n";
                this_thread::sleep_for(chrono::seconds(5));
            }
            setNonBlockingMode(false);
            system("clear");
            return;
        } else {
            // Fetch OHLC data
            string ohlcData = fetchOHLC(stockSymbol);

            // Display OHLC and timestamp
            cout << "User: " << username << endl;
            cout << "\n=== Market Data ===" << endl;
            cout << ohlcData;
        }

        // Check for 'Q' to exit
        for (int i = 0; i < 50; ++i) { // 50 * 100ms = 5 seconds
            char ch;
            if (read(STDIN_FILENO, &ch, 1) > 0) {
                if (ch == 'Q' || ch == 'q') {
                    setNonBlockingMode(false); // Disable non-blocking mode
                    system("clear"); // Clear the screen
                    return; // Exit the function
                }
            }
            this_thread::sleep_for(chrono::milliseconds(500));
        }
    }

    setNonBlockingMode(false); // Disable non-blocking mode before exiting
}
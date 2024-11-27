#include <iostream>
#include <string>
#include <cstdlib> // For system()
using namespace std;

int main() {
    string stockSymbol;
    cout << "Enter stock symbol: ";
    cin >> stockSymbol;
    
    char choice;
    cout << "Would you like to see the chart? (Y/N): ";
    cin >> choice;

    if (choice == 'Y' || choice == 'y') {
        string url = "https://in.tradingview.com/chart/?symbol=" + stockSymbol;

        // Command to open the URL in the default browser
#ifdef _WIN32
        string openCommand = "start " + url;
#elif __APPLE__
        string openCommand = "open " + url;
#elif __linux__
        string openCommand = "xdg-open " + url;
#else
        cerr << "Error: Unsupported platform.\n";
        return 1;
#endif

        // Execute the command to open the browser
        int openResult = system(openCommand.c_str());
        if (openResult != 0) {
            cerr << "Error: Failed to open the TradingView chart.\n";
        } else {
            cout << "Opening the chart for " << stockSymbol << "...\n";
        }
    } else {
        // Command to call the Python script
        string command = "/opt/anaconda3/bin/python api_AlphaVantage.py " + stockSymbol;

        // Execute the Python script
        int result = system(command.c_str());

        // Check if the script executed successfully
        if (result != 0) {
            cerr << "Error: Failed to execute Python script.\n";
        } else {
            cout << "Fetching data for " << stockSymbol << "...\n";
        }
    }
}
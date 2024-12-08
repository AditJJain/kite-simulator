#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

int getLotSize(const string& tradingSymbol) {
    const string filename = "Core/APIs/KiteConnect_filteredInstruments.csv";
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        return -1;
    }

    string line, symbol, lotSizeStr;
    getline(file, line); // Skip the header line

    while (getline(file, line)) {
        stringstream ss(line);
        string token;

        // Parse fields: instrument_token, trading_symbol, lot_size
        getline(ss, token, ','); // Skip instrument_token
        getline(ss, symbol, ','); // Get trading_symbol
        getline(ss, lotSizeStr, ','); // Get lot_size

        if (symbol == tradingSymbol) {
            return stoi(lotSizeStr);
        }
    }
    return -1; // Return -1 if the trading symbol is not found
}
#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

const int MAX_STOCKS = 100;
int numOfStocks = 0;

struct Stock {
    string symbol;
    double buyPrice;
    double currentPrice;
    int quantity;
    string productType; // "CNC" or "MIS"
    bool isMIS;         // True if MIS, False if CNC
};

Stock portfolio[MAX_STOCKS];

// Function to fetch stock data from Alpha Vantage API
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

string get_stock_data(const string& symbol, const string& api_key) {
    CURL* curl;
    CURLcode res;
    string read_buffer;

    string url = "https://www.alphavantage.co/query?function=TIME_SERIES_INTRADAY&symbol="
        + symbol + "&interval=1min&apikey=" + api_key;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            cerr << "Curl request failed: " << curl_easy_strerror(res) << endl;
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return read_buffer;
}

// Function to extract the stock's latest price from the Alpha Vantage response
double extractPrice(const string& apiResponse) {
    try {
        auto j = json::parse(apiResponse);
        if (j.contains("Time Series (1min)")) {
            auto timeSeries = j["Time Series (1min)"];
            if (!timeSeries.empty()) {
                auto latestEntry = timeSeries.begin();
                string latestClosePrice = latestEntry.value()["4. close"];
                return stod(latestClosePrice);
            } else {
                cerr << "No time series data available." << endl;
                return -1;
            }
        } else {
            cerr << "Invalid API response or time series data missing." << endl;
            return -1;
        }
    } catch (const exception& e) {
        cerr << "Error parsing the stock data: " << e.what() << endl;
        return -1;
    }
}

// Utility functions for validation
bool isValidSymbol(const string& symbol) {
    return !symbol.empty();
}

bool isValidQuantity(int quantity) {
    return quantity > 0;
}

void showError(const string& error) {
    cout << error << endl;
}

// Function to handle CNC Sell
void sellCNC() {
    string symbol;
    int quantity;

    cout << "Enter stock symbol to sell: ";
    cin >> symbol;

    // Search for the stock in the portfolio
    int index = -1;
    for (int i = 0; i < numOfStocks; ++i) {
        if (portfolio[i].symbol == symbol && portfolio[i].productType == "CNC") {
            index = i;
            break;
        }
    }

    if (index == -1) {
        showError("Stock not found in portfolio or not a CNC stock.");
        return;
    }

    cout << "Enter quantity to sell: ";
    cin >> quantity;

    if (quantity <= 0 || quantity > portfolio[index].quantity) {
        showError("Invalid quantity.");
        return;
    }

    // Simulate selling the stock
    double sellPrice = portfolio[index].currentPrice;
    portfolio[index].quantity -= quantity;

    if (portfolio[index].quantity == 0) {
        // Remove the stock from portfolio if quantity is 0
        for (int i = index; i < numOfStocks - 1; ++i) {
            portfolio[i] = portfolio[i + 1];
        }
        --numOfStocks;
    }

    cout << "Sold " << quantity << " shares of " << symbol << " for ₹" << sellPrice << " each.\n";
}

// Function to handle MIS Sell
void sellMIS() {
    string symbol;
    int quantity;

    cout << "Enter stock symbol to sell (MIS): ";
    cin >> symbol;

    // Search for the stock in the portfolio
    int index = -1;
    for (int i = 0; i < numOfStocks; ++i) {
        if (portfolio[i].symbol == symbol && portfolio[i].isMIS) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        showError("Stock not found in portfolio or not a MIS stock.");
        return;
    }

    cout << "Enter quantity to sell: ";
    cin >> quantity;

    if (quantity <= 0 || quantity > portfolio[index].quantity) {
        showError("Invalid quantity.");
        return;
    }

    // Simulate selling the stock
    double sellPrice = portfolio[index].currentPrice;
    portfolio[index].quantity -= quantity;

    if (portfolio[index].quantity == 0) {
        // Remove the stock from portfolio if quantity is 0
        for (int i = index; i < numOfStocks - 1; ++i) {
            portfolio[i] = portfolio[i + 1];
        }
        --numOfStocks;
    }

    cout << "Sold " << quantity << " shares of " << symbol << " for ₹" << sellPrice << " each.\n";
}

int main() {
    string api_key = "YOUR_ALPHA_VANTAGE_API_KEY"; // Replace with your API key
    int choice;

    do {
        cout << "\nMenu:\n";
        cout << "1. Sell CNC\n";
        cout << "2. Sell MIS\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                sellCNC();
                break;
            case 2:
                sellMIS();
                break;
            case 3:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 3);

    return 0;
}

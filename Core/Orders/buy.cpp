#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include "../../Libraries/JSON/json.hpp"

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

// Function to handle CNC Buy
void buyCNC(const string& api_key) {
    string symbol;
    int quantity;

    cout << "Enter stock symbol: ";
    cin >> symbol;

    if (!isValidSymbol(symbol)) {
        showError("Invalid stock symbol.");
        return;
    }

    string apiResponse = get_stock_data(symbol, api_key);
    double price = extractPrice(apiResponse);

    if (price == -1) {
        showError("Failed to retrieve stock price.");
        return;
    }

    cout << "The current price for " << symbol << " is ₹" << price << endl;

    cout << "Enter quantity: ";
    cin >> quantity;

    if (!isValidQuantity(quantity)) {
        showError("Invalid quantity.");
        return;
    }

    // Add to portfolio
    if (numOfStocks < MAX_STOCKS) {
        portfolio[numOfStocks] = {symbol, price, price, quantity, "CNC", false};
        ++numOfStocks;
        cout << quantity << " shares of " << symbol << " bought for CNC at ₹" << price << " each.\n";
    } else {
        showError("Portfolio is full. Cannot buy more stocks.");
    }
}

// Function to handle MIS Buy
void buyMIS(const string& api_key) {
    string symbol;
    int quantity;
    double leverage = 5.0; // Example leverage

    cout << "Enter stock symbol: ";
    cin >> symbol;

    if (!isValidSymbol(symbol)) {
        showError("Invalid stock symbol.");
        return;
    }

    string apiResponse = get_stock_data(symbol, api_key);
    double price = extractPrice(apiResponse);

    if (price == -1) {
        showError("Failed to retrieve stock price.");
        return;
    }

    cout << "The current price for " << symbol << " is ₹" << price << endl;

    cout << "Enter quantity: ";
    cin >> quantity;

    if (!isValidQuantity(quantity)) {
        showError("Invalid quantity.");
        return;
    }

    double marginPrice = price / leverage;

    // Add to portfolio as MIS
    if (numOfStocks < MAX_STOCKS) {
        portfolio[numOfStocks] = {symbol, marginPrice, price, quantity, "MIS", true};
        ++numOfStocks;
        cout << quantity << " shares of " << symbol << " bought for MIS (leveraged price ₹" 
             << marginPrice << ") at ₹" << price << " each.\n";
    } else {
        showError("Portfolio is full. Cannot buy more stocks.");
    }
}

int main() {
    string api_key = "YOUR_ALPHA_VANTAGE_API_KEY"; // Replace with your API key
    int choice;

    do {
        cout << "\nMenu:\n";
        cout << "1. Buy CNC\n";
        cout << "2. Buy MIS\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                buyCNC(api_key);
                break;
            case 2:
                buyMIS(api_key);
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

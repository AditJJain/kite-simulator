// Alpha Vantage API Key: 642YHS2LTSM8E7AH

#include <iostream>
#include "Libraries/HTTP/httplib.h"
#include "Libraries/JSON/json.hpp"
#include <string>
#include <openssl/ssl.h> 

using json = nlohmann::json;

class AlphaVantageAPI {
private:
    std::string apiKey;

public:
    AlphaVantageAPI(const std::string& key) : apiKey(key) {}

    // Function to fetch real-time stock data
    json getStockData(const std::string& symbol) {
        httplib::SSLClient client("https://www.alphavantage.co");  // SSL-enabled client for HTTPS
        std::string url = "/query?function=TIME_SERIES_INTRADAY&symbol=" + symbol + "&interval=1min&apikey=" + apiKey;

        // Send GET request over HTTPS
        auto res = client.Get(url.c_str());

        if (res && res->status == 200) {
            // Parse the JSON response
            try {
                json data = json::parse(res->body);
                return data;
            } catch (const std::exception& e) {
                std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            }
        } else {
            std::cerr << "Failed to fetch data for " << symbol << std::endl;
        }

        return json();  // Return empty JSON if something goes wrong
    }
};

void printStockData(const json& data, const std::string& symbol) {
    if (data.empty()) {
        std::cout << "No data available for " << symbol << std::endl;
        return;
    }

    try {
        // The JSON response contains "Time Series (1min)"
        auto timeSeries = data["Time Series (1min)"];
        
        std::cout << "Real-time stock data for " << symbol << ":\n";
        for (auto it = timeSeries.begin(); it != timeSeries.end(); ++it) {
            std::cout << "Time: " << it.key() << ", ";
            std::cout << "Open: " << it.value()["1. open"] << ", ";
            std::cout << "High: " << it.value()["2. high"] << ", ";
            std::cout << "Low: " << it.value()["3. low"] << ", ";
            std::cout << "Close: " << it.value()["4. close"] << ", ";
            std::cout << "Volume: " << it.value()["5. volume"] << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error processing data: " << e.what() << std::endl;
    }
}

int main() {
    std::string apiKey = "642YHS2LTSM8E7AH";  // Replace with your Alpha Vantage API Key
    std::string symbol = "AAPL";          // Example stock symbol (Apple)

    AlphaVantageAPI api(apiKey);
    json stockData = api.getStockData(symbol);
    
    printStockData(stockData, symbol);

    return 0;
}
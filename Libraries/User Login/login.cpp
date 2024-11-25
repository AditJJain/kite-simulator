#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <sstream>
#include <iomanip>
#include <thread>
#include <chrono>

// Function to write the response data from the API to a string
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

class AlphaVantageAPI {
public:
    AlphaVantageAPI(const std::string& api_key)
        : api_key(api_key), base_url("https://www.alphavantage.co/query") {}

    nlohmann::json get_stock_data(const std::string& symbol, const std::string& interval = "1min") {
        std::string url = base_url + "?function=TIME_SERIES_INTRADAY&symbol=" + symbol +
                          "&interval=" + interval + "&apikey=" + api_key;
        std::string response_data = fetch_data(url);
        if (!response_data.empty()) {
            try {
                return nlohmann::json::parse(response_data);
            } catch (nlohmann::json::parse_error& e) {
                std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            }
        }
        return nlohmann::json();
    }

private:
    std::string api_key;
    std::string base_url;

    std::string fetch_data(const std::string& url) {
        CURL* curl;
        CURLcode res;
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
        if (curl) {
            std::string response_data;
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                std::cerr << "CURL Error: " << curl_easy_strerror(res) << std::endl;
            }
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return response_data;
        }
        return "";
    }
};

void buy_stock(const std::string& symbol, double price) {
    std::cout << "Buying stock " << symbol << " at price $" << price << std::endl;
}

void sell_stock(const std::string& symbol, double price) {
    std::cout << "Selling stock " << symbol << " at price $" << price << std::endl;
}

void trade_stock(const std::string& symbol, double buy_threshold, double sell_threshold) {
    std::string api_key = "YOUR_API_KEY";  // Replace with your Alpha Vantage API key
    AlphaVantageAPI api(api_key);

    while (true) {
        auto data = api.get_stock_data(symbol);
        if (data.contains("Time Series (1min)")) {
            auto time_series = data["Time Series (1min)"];
            for (auto& [timestamp, values] : time_series.items()) {
                double current_price = std::stod(values["4. close"].get<std::string>());
                std::cout << "Current price of " << symbol << " at " << timestamp << ": $" << current_price << std::endl;

                if (current_price < buy_threshold) {
                    buy_stock(symbol, current_price);
                } else if (current_price > sell_threshold) {
                    sell_stock(symbol, current_price);
                }

                break;  // Exit after one data point for now
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(60));  // Wait 1 minute to comply with API limits
    }
}

int main() {
    std::string symbol = "AAPL";  // Replace with your desired stock symbol
    double buy_threshold = 150.00;  // Set your buy threshold
    double sell_threshold = 160.00; // Set your sell threshold

    trade_stock(symbol, buy_threshold, sell_threshold);

    return 0;
}

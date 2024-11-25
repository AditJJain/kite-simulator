# API Resets at 12:00 AM EST.
# Maximum 25 queries/day.

import json
import requests
import sys
import time

# Documentation Example Code:
# api_key = "AJ_AlphaVantage.key"
# url = f'https://www.alphavantage.co/query?function=TIME_SERIES_INTRADAY&symbol=IBM&interval=5min&apikey={api_key}'
# r = requests.get(url)
# data = r.json()
# print(data)

class AlphaVantageAPI:
    def __init__(self, api_key):
        self.api_key = api_key
        self.base_url = "https://www.alphavantage.co/query"

    def get_stock_data(self, symbol, interval="1min"):
        """Fetch real-time stock data."""
        params = {
            "function": "TIME_SERIES_INTRADAY",
            "symbol": symbol,
            "interval": interval,
            "apikey": self.api_key
        }
        response = requests.get(self.base_url, params=params)
        
        # Check for successful API call
        if response.status_code == 200:
            try:
                data = response.json()
                if "Time Series (1min)" in data:
                    return data
                else:
                    print(f"Error fetching data: {data.get('Note', 'Unknown Error')}")
            except json.JSONDecodeError:
                print("Error decoding JSON response.")
        else:
            print(f"HTTP Error: {response.status_code}")
        
        return {}

    def format_stock_data(self, data, symbol):
        """Format and display stock data."""
        if not data:
            print("No data to format.")
            return
        
        time_series = data.get("Time Series (1min)", {})
        if not time_series:
            print("No time series data available.")
            return
        
        print(f"\nReal-time stock data for {symbol}:")
        print("-" * 50)
        for timestamp, values in list(time_series.items())[:1]:  # Display only the latest data point
            print(f"Timestamp: {timestamp}")
            print(f"  Open:   ${values['1. open']}")
            print(f"  High:   ${values['2. high']}")
            print(f"  Low:    ${values['3. low']}")
            print(f"  Close:  ${values['4. close']}")
            print(f"  Volume: {values['5. volume']}")
            print("-" * 50)

# Main function
def main():
    api_key = "myKey.key"  # Replace with your Alpha Vantage API key
    stock_symbol = sys.argv[1]    # Replace with your desired stock symbol

    api = AlphaVantageAPI(api_key)

    try:
        while True:
            data = api.get_stock_data(stock_symbol)
            api.format_stock_data(data, stock_symbol)
            time.sleep(60)  # Pause for 60 seconds to respect API rate limits
    except KeyboardInterrupt:
        print("\nProgram exited by user.")

if __name__ == "__main__":
    main()
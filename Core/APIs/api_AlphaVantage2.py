import json
import requests
import sys

class AlphaVantageAPI:
    def __init__(self, api_key):
        self.api_key = api_key
        self.base_url = "https://www.alphavantage.co/query"

    def get_stock_price(self, symbol, interval="1min"):
        """Fetch the current stock price."""
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
                    # Get the most recent data point
                    latest_time = list(data["Time Series (1min)"].keys())[0]
                    latest_data = data["Time Series (1min)"][latest_time]
                    return {
                        "timestamp": latest_time,
                        "price": latest_data["4. close"]
                    }
                else:
                    print(f"Error fetching data: {data.get('Note', 'Unknown Error')}")
            except json.JSONDecodeError:
                print("Error decoding JSON response.")
        else:
            print(f"HTTP Error: {response.status_code}")
        
        return {}

def main():
    api_key = "myKey.key"  # Replace with your Alpha Vantage API key
    stock_symbol = sys.argv[1]    # Replace with your desired stock symbol

    api = AlphaVantageAPI(api_key)

    try:
        stock_data = api.get_stock_price(stock_symbol)
        if stock_data:
            print(stock_data['price'])
        else:
            print("Failed to fetch stock price.")
    except KeyboardInterrupt:
        print("\nProgram exited by user.")

if __name__ == "__main__":
    main()
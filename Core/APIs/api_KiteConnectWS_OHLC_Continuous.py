import logging
import sys
import time
import threading
from datetime import datetime
from kiteconnect import KiteTicker

# Enable logging
logging.basicConfig(level=logging.WARNING)

# Read API key and access token from files
api_key = open("../../Keys/KiteConnect.key", "r").read().strip()
access_token = open("../../Keys/KiteConnect_AccessToken.key", "r").read().strip()

# Validate arguments
if len(sys.argv) < 2:
    print("Usage: python script_name.py <symbol>")
    sys.exit(1)

# Get stock symbol from command-line arguments
stock_symbol = sys.argv[1].strip().upper()  # e.g., "GOLD24DECFUT"

# Map stock symbol to instrument tokens
symbol_to_token = {
    "GOLD24DECFUT": 109175815,
}

if stock_symbol not in symbol_to_token:
    print(f"Error: Symbol '{stock_symbol}' not found in token mapping.")
    sys.exit(1)

instrument_token = symbol_to_token[stock_symbol]

# Initialise KiteTicker
kws = KiteTicker(api_key, access_token)

# Global variable to store the latest tick
latest_tick = {}

def on_ticks(ws, ticks):
    """Callback to receive ticks."""
    global latest_tick
    for tick in ticks:
        if tick["instrument_token"] == instrument_token:
            latest_tick = tick

def on_connect(ws, response):
    """Callback on successful connect."""
    # Subscribe to the instrument token
    ws.subscribe([instrument_token])
    # Set mode to `full` to receive OHLC data
    ws.set_mode(ws.MODE_FULL, [instrument_token])

def on_close(ws, code, reason):
    """Callback on connection close."""
    ws.stop()

# Assign the callbacks
kws.on_ticks = on_ticks
kws.on_connect = on_connect
kws.on_close = on_close

# Function to display OHLC every 5 seconds
def display_ohlc(interval=5):
    """Print the OHLC values with timestamp."""
    global latest_tick
    while True:
        if latest_tick:
            ohlc = latest_tick.get("ohlc", {})
            last_price = latest_tick.get("last_price", "N/A")
            timestamp = latest_tick.get("exchange_timestamp")
            formatted_time = timestamp.strftime("%Y-%m-%d %H:%M:%S") if timestamp else "N/A"
            
            print(f"OHLC for {stock_symbol} at {formatted_time}:")
            print(f"  Open: {ohlc.get('open', 'N/A')}")
            print(f"  High: {ohlc.get('high', 'N/A')}")
            print(f"  Low: {ohlc.get('low', 'N/A')}")
            print(f"  Close: {ohlc.get('close', 'N/A')}")
            print(f"  Last Traded Price: {last_price}")
            print("-" * 30)
        else:
            print(f"Waiting for data for {stock_symbol}...")
        time.sleep(interval)

# Function to start the KiteTicker WebSocket
def start_kws():
    kws.connect()

# Start the KiteTicker WebSocket in a separate thread
ticker_thread = threading.Thread(target=start_kws)
ticker_thread.daemon = True  # Daemon thread will exit when the main program exits
ticker_thread.start()

# Display OHLC in the main thread every 5 seconds
try:
    display_ohlc()
except KeyboardInterrupt:
    print("\nExiting...")
    kws.close()    
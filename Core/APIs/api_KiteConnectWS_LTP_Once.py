import logging
import sys
import threading
from datetime import datetime
from kiteconnect import KiteTicker

# Disable logging to keep output clean
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
    "ANGELONE" : 82945,
}

if stock_symbol not in symbol_to_token:
    print(f"Error: Symbol '{stock_symbol}' not found in token mapping.")
    sys.exit(1)

instrument_token = symbol_to_token[stock_symbol]

# Initialize KiteTicker
kws = KiteTicker(api_key, access_token)

# Global variable to store the latest tick
latest_tick = {}
received_data = threading.Event()

def on_ticks(ws, ticks):
    """Callback to receive ticks."""
    global latest_tick
    for tick in ticks:
        if tick["instrument_token"] == instrument_token:
            latest_tick = tick
            received_data.set()
            ws.close()  # Close the connection after receiving data

def on_connect(ws, response):
    """Callback on successful connect."""
    ws.subscribe([instrument_token])
    ws.set_mode(ws.MODE_FULL, [instrument_token])

def on_close(ws, code, reason):
    """Callback on connection close."""
    ws.stop()

# Assign the callbacks
kws.on_ticks = on_ticks
kws.on_connect = on_connect
kws.on_close = on_close

# Start the KiteTicker WebSocket and wait for data
def start_kws():
    kws.connect(threaded=True)
    received = received_data.wait(timeout=5)
    if not received:
        print("Error: Timeout while waiting for data.")
        sys.exit(1)

start_kws()

# Get last price and timestamp
last_price = latest_tick.get("last_price", "N/A")
timestamp = latest_tick.get("exchange_timestamp")
formatted_time = timestamp.strftime("%Y-%m-%d %H:%M:%S") if timestamp else "N/A"

# Output price and timestamp
print(f"{last_price},{formatted_time}")
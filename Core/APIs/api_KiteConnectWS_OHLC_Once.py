from datetime import datetime
import logging
import subprocess
import sys
import os
import threading
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
try:
    result = subprocess.run(
        os.chdir(os.path.dirname(os.path.abspath(__file__))) # Changes the CWD to avoid file not found error
        ["python3", "../APIs/instrumentSearch.py", stock_symbol], 
        capture_output=True, text=True, check=True
    )
    instrument_token = int(result.stdout.strip())  # Convert the output to an integer
except subprocess.CalledProcessError as e:
    print(f"Error: Unable to fetch token for {stock_symbol}. {e.stderr}")
    sys.exit(1)

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

# Get OHLC and timestamp
ohlc = latest_tick.get("ohlc", {})
last_price = latest_tick.get("last_price", "N/A")
timestamp = latest_tick.get("exchange_timestamp")
formatted_time = timestamp.strftime("%Y-%m-%d %H:%M:%S") if timestamp else "N/A"

# Output OHLC and timestamp
print(f"OHLC for {stock_symbol} at {formatted_time}:")
print(f"  Open: {ohlc.get('open', 'N/A')}")
print(f"  High: {ohlc.get('high', 'N/A')}")
print(f"  Low: {ohlc.get('low', 'N/A')}")
print(f"  Close: {ohlc.get('close', 'N/A')}")
print(f"  Last Traded Price: {last_price}")
print("-" * 30)
import logging
from kiteconnect import KiteTicker

logging.basicConfig(level=logging.DEBUG)

# Initialise
kws = KiteTicker(open("../../Keys/KiteConnect.key", "r").read(), open("../../Keys/KiteConnect_AccessToken.key", "r").read())

def on_ticks(ws, ticks):
    # Callback to receive ticks.
    logging.debug("Ticks: {}".format(ticks))

def on_connect(ws, response):
    # Callback on successful connect.
    # Subscribe to a list of instrument_tokens (RELIANCE and ACC here).
    ws.subscribe([111570695])

    # Set RELIANCE to tick in `full` mode.
    ws.set_mode(ws.MODE_FULL, [111570695])

def on_close(ws, code, reason):
    # On connection close stop the main loop
    # Reconnection will not happen after executing `ws.stop()`
    ws.stop()

# Assign the callbacks.
kws.on_ticks = on_ticks
kws.on_connect = on_connect
kws.on_close = on_close

# Infinite loop on the main thread. Nothing after this will run.
# You have to use the pre-defined callbacks to manage subscriptions.
kws.connect()
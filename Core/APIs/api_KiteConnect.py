import logging
from kiteconnect import KiteConnect

logging.basicConfig(level=logging.WARNING)

kite = KiteConnect(api_key = open("../../Keys/KiteConnect.key", "r").read())
# print(kite.login_url())

# Redirect the user to the login url obtained
# from kite.login_url(), and receive the request_token
# from the registered redirect url after the login flow.
# Once you have the request_token, obtain the access_token
# as follows.

data = kite.generate_session(open("../../Keys/KiteConnect_RequestToken.key", "r").read(), api_secret= open("../../Keys/KiteConnect_Secret.key", "r").read())
kite.set_access_token(data["access_token"])
print(data["access_token"])

with open("../../Keys/KiteConnect_AccessToken.key", "w") as file:
    file.write(data["access_token"])

# Place an order
# try:
#     order_id = kite.place_order(tradingsymbol="INFY",
#                                 exchange=kite.EXCHANGE_NSE,
#                                 transaction_type=kite.TRANSACTION_TYPE_BUY,
#                                 quantity=1,
#                                 variety=kite.VARIETY_AMO,
#                                 order_type=kite.ORDER_TYPE_MARKET,
#                                 product=kite.PRODUCT_CNC,
#                                 validity=kite.VALIDITY_DAY)

#     logging.info("Order placed. ID is: {}".format(order_id))
# except Exception as e:
#     logging.info("Order placement failed: {}".format(e.message))

# Fetch all orders
# kite.orders()

# Get instruments
# kite.instruments()

# Place an mutual fund order
# kite.place_mf_order(
#     tradingsymbol="INF090I01239",
#     transaction_type=kite.TRANSACTION_TYPE_BUY,
#     amount=5000,
#     tag="mytag"
# )

# Cancel a mutual fund order
# kite.cancel_mf_order(order_id="order_id")

# Get mutual fund instruments
# kite.mf_instruments()
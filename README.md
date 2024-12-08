# kite-simulator

## Overview
A CLI-based portfolio management and trading simulator built using C++. It allows users to manage their portfolio, intraday positions, watchlist, and funds while fetching live market data through APIs. This project works with the **KiteConnect API** to fetch current market prices and supports various order types like CNC (Cash and Carry),  MIS (Margin Intraday Square-off) & NRML (Normal Order).

## Features
- **User Management:**
  - Login, registration, and password recovery system with hashed passwords and security questions.
  - Supports multiple users.
  
- **Market Data:**
  - Fetch real-time market data (OHLC) using the **KiteConnect API**.

- **Place Orders:**
  - Users can place CNC, NRML, and MIS orders.
  - Updates the current balance, portfolio/positions, and trade history based on the trade executed.
  - Fetches the lot sizes for each instrument to ensure only valid quantities can be bought.

- **Portfolio Management:**
  - View and manage long-term holdings.
  - Calculate unrealized profit/loss based on live market data.

- **Intraday Positions:**
  - View and manage short-term trades (MIS).
  - Display real-time updates for intraday positions.

- **Watchlist:**
  - Add and remove stocks from a custom watchlist.

- **Funds Management:**
  - View current balance, add funds, and withdraw funds.

- **Trade History:**
  - View all trades executed by a user.

## Dependencies
### C++ Libraries:
- **Standard C++ Libraries**: `iostream`, `fstream`, `vector`, `string`, `iomanip`, `thread`, `chrono`, `sstream`, `algorithm`, `map`, `functional`, `cmath`, `cstdlib`, `ctime`.
- **POSIX Functions**: Used for non-blocking input and terminal manipulation.

### Python:
- **Python 3.x**: Required for API scripts.
- **KiteConnect**: Python SDK for fetching live market data.
- **Twisted**: For WebSocket connections.

### External Tools:
- **GCC/G++**: C++ compiler with C++23 support.
- **Make**: Build automation tool.
- **Anaconda (Python Distribution)**: For running Python scripts.

## Directory Structure
```
├── Core/
│   ├── APIs/
│   │   ├── api_KiteConnectWS_LTP_Once.py
|   |   ├── api_KiteConnectWS_OHLC_Once.py
|   |   ├── apicall_KiteConnect_LTP.cpp
│   │   ├── apicall_KiteConnect_LTP.h
|   |   ├── apicall_KiteConnect_OHLC.cpp
|   |   ├── apicall_KiteConnect_OHLC.h
|   |   ├── fetchInstruments.txt
|   |   ├── filterInstruments.cpp
|   |   ├── instrumentSearch.py
|   |   ├── KiteConnect_filteredInstruments.csv
|   |   ├── KiteConnect_instruments.csv
│   ├── Funds/
│   │   ├── funds.cpp
│   │   ├── funds.h
│   ├── Menu/
│   │   ├── mainmenu.cpp
│   │   ├── mainmenu.h
│   ├── Orders/
│   │   ├── buy-sell.cpp
│   │   ├── buy-sell.h
|   |   ├── getLotSize.cpp
|   |   ├── getLotSize.h
│   ├── Portfolio/
│   │   ├── portfolio.cpp
│   │   ├── portfolio.h
│   ├── Positions/
│   │   ├── intraday.cpp
│   │   ├── intraday.h
|   ├── TradeHistory/
|   |   ├── tradehistory.cpp
|   |   ├── tradehistory.h
│   ├── UserLogin/
│   │   ├── login.cpp
│   │   ├── login.h
|   ├── Watchlist
|   |   ├── watchlist.cpp
|   |   ├── watchlist.h
│   ├── holding.h
|   ├── intradayPosition.h
|   ├── setNonBlockingMode.cpp
│   ├── setNonBlockingMode.h
├── Keys/
|   ├── KiteConnect.key
|   ├── KiteConnect_AccessToken.key
├── main.cpp
├── Makefile
├── README.md
```

## How to Build and Run the Project
1. **Clone the Repository**:
   ```bash
   git clone https://github.com/username/kite-simulator.git
   cd kite-simulator
   ```

2. **Install Dependencies**:
   - Install required Python packages:
     ```bash
     pip install kiteconnect twisted
     ```

3. **Configure API Keys**:
   - Place your KiteConnect API key and Access Token in the `Keys` folder.

4. **Build the Project**:
   - Use the Makefile to compile the project:
     ```bash
     make
     ```

5. **Run the Project**:
   - Run the compiled executable:
     ```bash
     ./kite-simulator
     ```

## Usage Guide
1. **Login/Register**: Start by logging in or registering as a new user.
2. **Main Menu Options**:
   - View Market Data
   - Place Orders (MIS/NRML/CNC).
   - View Portfolio.
   - View Intraday Positions.
   - Manage Funds (Add/Withdraw/View).
   - Manage Watchlist.
   - View Trade History
3. **Exit**: Press `Q` in any submenu to return to the main menu.

## Troubleshooting
- **Undefined Symbols/Error**: Ensure all header files are correctly included and linked.
- **Permission Errors**: Ensure you have read/write permissions for the data files in `Core/`.
- **Python Errors**: Ensure Python scripts have the correct shebang and paths.

## Authors
- **Adit Jain**  
- **Dhwani Balchandani**

## License
This project is licensed under the [MIT License](LICENSE).
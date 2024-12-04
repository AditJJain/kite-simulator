#include "Core/APIs/apicall_KiteConnect_OHLC.h"
#include "Core/Orders/buy-sell.h"
#include "Core/Portfolio/portfolio.h"
#include "Core/Positions/intraday.h"
#include "Core/Watchlist/watchlist.h"
#include "Core/Funds/funds.h"
#include <iostream>
using namespace std;

void runMainMenu(const std::string& username) {
    int choice;
    bool end = false;

    while (!end) {
        cout << "\nChoose your option: " << endl;
        cout << "1 - View Market Data" << endl;
        cout << "2 - Place Order" << endl;
        cout << "3 - View Portfolio" << endl;
        cout << "4 - View Intraday Positions" << endl;
        cout << "5 - Manage Watchlist" << endl;
        cout << "6 - Add/Withdraw Funds" << endl;
        cout << "7 - View Trade History" << endl;
        cout << "8 - Exit" << endl;
        cout << "\nEnter your choice: ";

        // Check if the input is an integer
        if (!(cin >> choice)) {
            // Clear the error flag
            cin.clear();
            // Ignore invalid input up to the newline
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number between 1 and 7." << endl;
            continue;  // Restart the loop for a new input
        }

        switch (choice) {
            case 1:
                viewMarketData(username);
                break;
            case 2:
                runBuySell(username);
                break;
            case 3:
                runPortfolio(username);
                break;
            case 4:
                runPositions(username);
                break;
            case 5:
                runWatchlist(username);
                break;
            case 6:
                runFundsManagement(username);
                break;
            case 7:
                cout << "Viewing Trade History..." << endl;
                break;
            case 8:
                cout << "Exiting program. Goodbye!" << endl;
                end = true;
                break;
            default:
                cout << "Invalid choice. Please enter a number between 1 and 7." << endl;
                break;
        }
    }
}
#include "Core/Portfolio/portfolio.h"
#include "Core/Positions/intraday.h"
#include "Core/Watchlist/watchlist.h"
#include "Core/Funds/funds.h"
#include <iostream>
using namespace std;

// Function to check if the user wants to return to the main menu
bool promptReturnToMenu() {
    char YN;
    while (true) {  // Loop until a valid input is received
        cout << "Would you like to go back to the menu? " << endl;
        cout << "If yes, press Y. If no, press N." << endl;
        cout << "Choice: ";
        cin >> YN;
        YN = toupper(YN);

        if (YN == 'Y') return true;
        if (YN == 'N') return false;

        cout << "Invalid choice. Please enter 'Y' or 'N'." << endl;
    }
}

void runMainMenu() {
    int choice;
    bool end = false;

    while (!end) {
        cout << "\nChoose your option: " << endl;
        cout << "1 - View Market Data" << endl;
        cout << "2 - Place Order" << endl;
        cout << "3 - View Portfolio" << endl;
        cout << "4 - Manage Watchlist" << endl;
        cout << "5 - Add/Withdraw Funds" << endl;
        cout << "6 - View Trade History" << endl;
        cout << "7 - Exit" << endl;

        cout << "Enter your choice: ";

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
                cout << "Viewing Market Data..." << endl;
                end = !promptReturnToMenu();
                break;
            case 2:
                cout << "Placing an Order..." << endl;
                end = !promptReturnToMenu();
                break;
            case 3:
                runPortfolio();
                end = !promptReturnToMenu();
                break;
            case 4:
                cout << "Managing Watchlist..." << endl;
                end = !promptReturnToMenu();
                break;
            case 5:
                cout << "Adding/Withdrawing Funds..." << endl;
                end = !promptReturnToMenu();
                break;
            case 6:
                cout << "Viewing Trade History..." << endl;
                end = !promptReturnToMenu();
                break;
            case 7:
                cout << "Exiting program. Goodbye!" << endl;
                end = true;
                break;
            default:
                cout << "Invalid choice. Please enter a number between 1 and 7." << endl;
                break;
        }
    }
}
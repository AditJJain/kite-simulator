#include <iostream>
#include <fstream>
#include <string>
#include <iomanip> // For setting precision.
#include <chrono>
#include <thread>
using namespace std;

void displayBalance(double balance) {
    cout << fixed << setprecision(2);
    cout << "Your current balance is: ₹" << balance << endl;
}

// Function to load the balance from a file
double loadBalance(const string& filename) {
    ifstream infile(filename);
    double balance = 0.0;

    if (infile.is_open()) {
        infile >> balance;
        infile.close();
    } else {
        // If the file doesn't exist, start with a balance of ₹0.00
        balance = 0.0;
    }
    return balance;
}

// Function to save the balance to a file
void saveBalance(const string& filename, double balance) {
    ofstream outfile(filename);
    if (outfile.is_open()) {
        outfile << balance;
        outfile.close();
    } else {
        cerr << "Error: Unable to save balance to file." << endl;
    }
}

void addFunds(double& balance) {
    double amount = 0.0;
    cout << "Enter the amount to add: ₹";
    cin >> amount;

    if (amount <= 0) {
        cout << "Amount must be greater than zero." << endl;
    } else {
        balance += amount;
        cout << "Successfully added ₹" << amount << " to your account." << endl;
        displayBalance(balance);
    }
}

void withdrawFunds(double& balance) {
    double amount = 0.0;
    cout << "Enter the amount to withdraw: ₹";
    cin >> amount;

    if (amount <= 0) {
        cout << "Amount must be greater than zero." << endl;
    } else if (amount > balance) {
        cout << "Insufficient funds. Your current balance is ₹" << balance << endl;
    } else {
        balance -= amount;
        cout << "Successfully withdrew ₹" << amount << " from your account." << endl;
        displayBalance(balance);
    }
}

void runFundsManagement(const string& username) {
    system("clear");

    string balanceFile = "Core/Funds/" + username + "_balance.csv";
    double balance = loadBalance(balanceFile);

    char choice;
    bool exitMenu = false; // Flag to control the loop

    do {
        cout << "User: " << username << endl;
        cout << "\n=== Funds Management ===" << endl;
        cout << "1. View Balance" << endl;
        cout << "2. Add Funds" << endl;
        cout << "3. Withdraw Funds" << endl;
        cout << "Q. Exit to Main Menu" << endl;
        cout << "\nEnter your choice (1-3 or Q): ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer

        switch (toupper(choice)) {
            case '1':
                displayBalance(balance);
                this_thread::sleep_for(chrono::seconds(5));
                system("clear");
                break;
            case '2':
                addFunds(balance);
                saveBalance(balanceFile, balance);
                this_thread::sleep_for(chrono::seconds(5));
                system("clear");
                break;
            case '3':
                withdrawFunds(balance);
                saveBalance(balanceFile, balance);
                this_thread::sleep_for(chrono::seconds(5));
                system("clear");
                break;
            case 'Q':
                system("clear");
                exitMenu = true; // Set the flag to true to exit the loop
                break;
            default:
                cout << "Invalid choice. Please enter 1, 2, 3, or Q." << endl;
                this_thread::sleep_for(chrono::seconds(2));
                system("clear");
        }
    } while (!exitMenu); // Continue looping until exitMenu is true
}
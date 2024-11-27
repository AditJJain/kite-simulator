#include <iostream>
#include <fstream>
#include <string>
#include <iomanip> // For setting precision.
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

int main() {
    const string balanceFile = "balance.csv";
    double balance = loadBalance(balanceFile);

    int choice = 0;
    do {
        cout << "\n=== Funds Management ===" << endl;
        cout << "1. View Balance" << endl;
        cout << "2. Add Funds" << endl;
        cout << "3. Withdraw Funds" << endl;
        cout << "4. Exit to Main Menu" << endl;
        cout << "Enter your choice (1-4): ";
        cin >> choice;

        switch (choice) {
            case 1:
                displayBalance(balance);
                break;
            case 2:
                addFunds(balance);
                saveBalance(balanceFile, balance);
                break;
            case 3:
                withdrawFunds(balance);
                saveBalance(balanceFile, balance);
                break;
            case 4:
                cout << "Returning to Main Menu..." << endl;
                break;
            default:
                cout << "Invalid choice. Please enter a number between 1 and 4." << endl;
        }
    } while (choice != 4);
}
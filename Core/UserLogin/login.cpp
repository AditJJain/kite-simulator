// Author: Nemonet TYP
// Title: A Login and Registration System Programmed in C++
// Source: https://github.com/The-Young-Programmer/C-CPP-Programming/blob/main/Projects/C%2B%2B%20Projects/Basic/A%20Login%20and%20Registration%20System/login.cpp

#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <functional>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "login.h"
#include "../Menu/mainmenu.h"
using namespace std;

const string dataFilePath = "Core/UserLogin/data.txt";

void login::Login() {
    string count;
    string username, password, id, recordPass, recordSecurity;
    system("clear");
    cout << "Please enter the username and password: " << endl;
    cout << "USERNAME: ";
    cin >> username;
    cout << "PASSWORD: ";
    cin >> password;

    string loginHash = password;
    hash<string> mystdhash;
    int loginHashPassword = mystdhash(loginHash);

    ifstream input(dataFilePath);

    while (input >> id >> recordPass >> recordSecurity)
    {
        if (id == username && stoi(recordPass) == loginHashPassword)
        {
            count = "1";
            system("clear");
        }
    }
    input.close();
    if (count == "1") {
        cout << "User: " << username << endl;
        cout << "Login successful! Welcome to the kite-simulator!";
        system("clear");
        runMainMenu(username);
    }
    else {
        system("clear");
        cout << "Username or password is incorrect. Please try again or register as a new user." << endl;
        Login(); // Call Login() again after incorrect password
    }
}

void login::Registration() {
    string regUser, regPassword, regId, regPass, securityQuestion, regSecure, regCount;
    system("clear");
    cout << "Enter Username: ";
    cin >> regUser;
    cout << "Enter Password: ";
    cin >> regPassword;
    cout << "Security Question: What was your favorite childhood movie?: ";
    cin.ignore();
    getline(cin, securityQuestion);

    string hashing = regPassword;
    hash<string> mystdhash;
    int hashPassword = mystdhash(hashing);

    string secureHashing = securityQuestion;
    hash<string> mystdhash2;
    int securityHash = mystdhash2(secureHashing);

    ifstream input(dataFilePath);
    input.seekg(0, ios::end);

    if (input.tellg() == 0) // If file is empty, write the first user
    {
        ofstream f1(dataFilePath, ios::app);
        f1 << regUser << ' ' << hashPassword << ' ' << securityHash << endl;
        system("clear");
        cout << "Registration successful!" << endl;
        Login(); // Call Login() after registration
        return;
    }
    else { // For subsequent users
        ifstream input(dataFilePath);
        while (input >> regId >> regPass >> regSecure)
        {
            if (regUser == regId)
            {
                string decision;
                cout << "Username already taken." << endl;
                cout << "Enter 1 to enter a new username." << endl;
                cout << "Enter 2 to go back to the menu." << endl;
                cout << "\nEnter choice: " << endl;
                cin >> decision;

                if (decision == "1") {
                    Registration();
                } else if (decision == "2") {
                    system("clear");
                    cout << "Returning to menu\n";
                    return;
                } else {
                    system("clear");
                    cout << "Invalid Entry, returning to menu." << endl;
                    return;
                }
            } else {
                regCount = "1";
            }
        }
        if (regCount == "1")
        {
            ofstream f1(dataFilePath, ios::app);
            f1 << regUser << ' ' << hashPassword << ' ' << securityHash << endl;
            system("clear");
            ofstream balanceFile("Core/Funds/" + regUser + "_balance.csv");
            if (balanceFile.is_open()) {
                balanceFile << "0.00";
                balanceFile.close();
            } else {
                cerr << "Error: Unable to initialize balance file." << endl;
            }
            ofstream("Core/Portfolio/" + regUser + "_portfolio.dat").close();
            ofstream("Core/Positions/" + regUser + "_positions.dat").close();
            ofstream("Core/Watchlist/" + regUser + "_watchlist.txt").close();
            cout << "Data files created." << endl;
            cout << "Registration successful!" << endl;
            Login(); // Call Login() after registration
            return;
        }
    }
}

void login::ForgotPassword() {
    string forgotChoice, count, secondCount;
    system("clear");
    cout << "Press 1 to enter username." << endl;
    cout << "Press 2 to go back to menu." << endl;
    cout << "\nEnter choice: ";
    cin >> forgotChoice;

    if (forgotChoice == "1")
    {
        string user, userSecurity, forgotId, forgotPass, forgotSecurity;
        int newHashPassword, forgotSecHash;
        system("clear");
        cout << "\nEnter USERNAME: ";
        cin >> user;
        cout << "\nAnswer your security question: What was your favorite childhood movie?: ";
        cin.ignore();
        getline(cin, userSecurity);

        // Hash the security answer
        hash<string> mystdhash;
        forgotSecHash = mystdhash(userSecurity);

        // Read user data from file
        ifstream input(dataFilePath);
        ofstream temp("temp.txt");
        bool userFound = false;

        while (input >> forgotId >> forgotPass >> forgotSecurity) {
            if (forgotId == user && stoi(forgotSecurity) == forgotSecHash) {
                userFound = true;
                cout << "Enter your new password: ";
                string newPassword;
                cin >> newPassword;
                newHashPassword = mystdhash(newPassword);
                temp << forgotId << ' ' << newHashPassword << ' ' << forgotSecurity << endl;
            } else {
                temp << forgotId << ' ' << forgotPass << ' ' << forgotSecurity << endl;
            }
        }
        input.close();
        temp.close();
        remove(dataFilePath.c_str());
        rename("temp.txt", dataFilePath.c_str());

        if (userFound) {
            cout << "Password reset successful!" << endl;
            Login(); // Call Login() after password reset
        } else {
            cout << "Username or security answer is incorrect." << endl;
        }
    }
}

void login::DeleteLine(string userDelete) {
    string line;
    ifstream myFile;
    myFile.open(dataFilePath);
    ofstream temp;
    temp.open("temp.txt");
    while (getline(myFile, line))
    {
        if (line.substr(0, userDelete.size()) != userDelete)
        {
            temp << line << endl;
        }
    }
    myFile.close();
    temp.close();
    remove(dataFilePath.c_str());
    rename("temp.txt", dataFilePath.c_str());
}

void runUserLogin() {
    system("clear");
    login userLogin;
    int choice;
    cout << "1. Login\n2. Register\n3. Forgot Password\nEnter your choice: ";
    cin >> choice;
    if (choice == 1) {
        userLogin.Login();
    } else if (choice == 2) {
        userLogin.Registration();
    } else if (choice == 3) {
        userLogin.ForgotPassword();
    } else {
        cout << "Invalid choice. Please try again." << endl;
    }
}
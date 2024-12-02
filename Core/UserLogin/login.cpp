// Autor : Nemonet TYP
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
    cout << "\nPlease enter the username and password: " << endl;
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
    if (count == "1")
    {
        cout << "User: " << username << "\nLogin successful! Welcome to the kite-simulator!";
        string choice = "1";
        while (choice != "3")
        {
            cout << "\n\n_________      Menu      __________" << endl;
            cout << "\n| Press 1 to VIEW PROFILE                      |" << endl;
            cout << "| Press 2 to VIEW KITE SIMULATOR               |" << endl;
            cout << "| Press 3 to LOGOUT                            |" << endl;
            cout << "\nPlease Enter your choice: ";
            cin >> choice;
            cout << endl;

            if (choice == "1")
            {
                system("clear");
                cout << "Profile viewing functionality is not implemented yet." << endl;
            }
            else if (choice == "2") {
                system("clear");
                runMainMenu();
            }
            else if (choice == "3")
            {
                system("clear");
                cout << "Successfully Logged Out." << endl;
            }
            else
            {
                system("clear");
                cout << "Invalid Choice. Please try again.";
            }
        }
    }
    else
    {
        system("clear");
        cout << "\nUsername or password is incorrect, please try again or register\n";
    }
}

void login::Registration() {
    string regUser, regPassword, regId, regPass, securityQuestion, regSecure, regCount;
    system("clear");
    cout << "\nEnter Username: ";
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

    if (input.tellg() == 0)
    {
        ofstream f1(dataFilePath, ios::app);
        f1 << regUser << ' ' << hashPassword << ' ' << securityHash << endl;
        system("clear");
        cout << "\nRegistration successful!\n";
        return;
    }
    else
    {
        ifstream input(dataFilePath);
        while (input >> regId >> regPass >> regSecure)
        {
            if (regUser == regId)
            {
                string decision;
                cout << "\nUsername already taken.\n";
                cout << "Enter 1 to enter a new one\n";
                cout << "Enter 2 to go back to the menu\n";
                cout << "\nEnter choice: ";
                cin >> decision;

                if (decision == "1")
                {
                    Registration();
                }
                else if (decision == "2")
                {
                    system("clear");
                    cout << "Returning to menu\n";
                    return;
                }
                else
                {
                    system("clear");
                    cout << "Invalid Entry, returning to menu." << endl;
                    return;
                }
            }
            else
            {
                regCount = "1";
            }
        }
        if (regCount == "1")
        {
            ofstream f1(dataFilePath, ios::app);
            f1 << regUser << ' ' << hashPassword << ' ' << securityHash << endl;
            system("clear");
            cout << "\nRegistration successful!\n";
            return;
        }
    }
}

void login::ForgotPassword() {
    string forgotChoice, count, secondCount;
    system("clear");
    cout << "\nPress 1 to enter USERNAME\n";
    cout << "Press 2 to go back to MENU\n";
    cout << "\ntEnter choice: ";
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
            cout << "\nPassword reset successful!\n";
        } else {
            cout << "\nUsername or security answer is incorrect.\n";
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
        cout << "Invalid choice.\n";
    }
}
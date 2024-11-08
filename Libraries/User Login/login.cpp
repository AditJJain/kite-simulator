// Autor : Nemonet TYP
// Title: A Login and Registration System Programmed in C++
// PROJECT FOR C/C++ PROGRAMMING TUTORIAL


#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <functional>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "login.h"
using namespace std;

void login::Login()
{
    string count;
    string username, password, id, recordPass, recordSecurity;
    system("clear");
    cout << "\n\t\t\t Please enter the username and password: " << endl;
    cout << "\t\t\t USERNAME: ";
    cin >> username;
    cout << "\t\t\t PASSWORD: ";
    cin >> password;

    string loginHash = password;
    hash<string> mystdhash;
    int loginHashPassword = mystdhash(loginHash);

    ifstream input("data.txt");

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
        cout << username << "\nLogin successful!\n";
        string choice = "1";
        while (choice != "2")
        {
            cout << "\t\t\t_____________________________________________\n\n\n";
            cout << "\t\t\t         Welcome to the NEMO 2023 Login!         \n\n";
            cout << "\t\t\t_______ Currently Logged In: " << username << " ________\n\n";
            cout << "\t\t\t_________           Menu           __________\n\n";
            cout << "\t | Press 1 to VIEW PROFILE                           |" << endl;
            cout << "\t | Press 2 to LOGOUT                                 |" << endl;
            cout << "\n\t\t\t Please Enter your choice: ";
            cin >> choice;
            cout << endl;

            if (choice == "1")
            {
                system("clear");
                cout << "Profile viewing functionality is not implemented yet." << endl;
            }
            else if (choice == "2")
            {
                system("clear");
                cout << "Logging out" << endl;
            }
            else
            {
                system("clear");
                cout << "Choice invalid, try again";
            }
        }
    }
    else
    {
        system("clear");
        cout << "\n Username or password is incorrect, please try again or register\n";
    }
}

void login::Registration()
{
    string regUser, regPassword, regId, regPass, securityQuestion, regSecure, regCount;
    system("clear");
    cout << "\n\t\t\t Enter Username: ";
    cin >> regUser;
    cout << "\t\t\t Enter Password: ";
    cin >> regPassword;
    cout << "\t\t\t Security Question: What was your favorite childhood movie?: ";
    cin.ignore();
    getline(cin, securityQuestion);

    string hashing = regPassword;
    hash<string> mystdhash;
    int hashPassword = mystdhash(hashing);

    string secureHashing = securityQuestion;
    hash<string> mystdhash2;
    int securityHash = mystdhash2(secureHashing);

    ifstream input("data.txt");
    input.seekg(0, ios::end);

    if (input.tellg() == 0)
    {
        ofstream f1("data.txt", ios::app);
        f1 << regUser << ' ' << hashPassword << ' ' << securityHash << endl;
        system("clear");
        cout << "\n\t\t\t Registration successful!\n";
        return;
    }
    else
    {
        ifstream input("data.txt");
        while (input >> regId >> regPass >> regSecure)
        {
            if (regUser == regId)
            {
                string decision;
                cout << "\n\t\tUsername already taken.\n";
                cout << "\t\tEnter 1 to enter a new one\n";
                cout << "\t\tEnter 2 to go back to the menu\n";
                cout << "\n\t\tEnter choice: ";
                cin >> decision;

                if (decision == "1")
                {
                    Registration();
                }
                else if (decision == "2")
                {
                    system("clear");
                    cout << "\tReturning to menu\n";
                    return;
                }
                else
                {
                    system("clear");
                    cout << "\tInvalid Entry, returning to menu." << endl;
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
            ofstream f1("data.txt", ios::app);
            f1 << regUser << ' ' << hashPassword << ' ' << securityHash << endl;
            system("clear");
            cout << "\n\t\t\t Registration successful!\n";
            return;
        }
    }
}

void login::ForgotPassword()
{
    string forgotChoice, count, secondCount;
    system("clear");
    cout << "\n\t\t\tPress 1 to enter USERNAME\n";
    cout << "\t\t\tPress 2 to go back to MENU\n";
    cout << "\n\t\t\tEnter choice: ";
    cin >> forgotChoice;

    if (forgotChoice == "1")
    {
        string user, userSecurity, forgotId, forgotPass, forgotSecurity;
        int newHashPassword, forgotSecHash;
        system("clear");
        cout << "\n\t\tEnter USERNAME: ";
        cin >> user;
        cout << "\n\t\tAnswer your security question: What was your favorite childhood movie?: ";
        cin.ignore();
        getline(cin, userSecurity);

        // Hash the security answer
        hash<string> mystdhash;
        forgotSecHash = mystdhash(userSecurity);

        // Read user data from file
        ifstream input("data.txt");
        ofstream temp("temp.txt");
        bool userFound = false;

        while (input >> forgotId >> forgotPass >> forgotSecurity) {
            if (forgotId == user && stoi(forgotSecurity) == forgotSecHash) {
                userFound = true;
                cout << "\t\t\t Enter your new password: ";
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
        remove("data.txt");
        rename("temp.txt", "data.txt");

        if (userFound) {
            cout << "\n\t\t\t Password reset successful!\n";
        } else {
            cout << "\n\t\t\t Username or security answer is incorrect.\n";
        }
    }
}

void login::DeleteLine(string userDelete)
{
    string line;
    ifstream myFile;
    myFile.open("data.txt");
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
    remove("data.txt");
    rename("temp.txt", "data.txt");
}

int main() {
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
    return 0;
}
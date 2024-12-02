// funds.h
#ifndef FUNDS_H
#define FUNDS_H

#include <string>

void displayBalance(double balance);
double loadBalance(const std::string& filename);
void saveBalance(const std::string& filename, double balance);
void addFunds(double& balance);
void withdrawFunds(double& balance);
int runFundsManagement();

#endif // FUNDS_H
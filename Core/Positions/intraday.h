// intraday.h
#ifndef INTRADAY_H
#define INTRADAY_H

#include <string>
#include <vector>
#include <utility>
#include "Core/intradayPosition.h"

void loadIntradayPositions(const std::string& username, std::vector<IntradayPosition>& positions);
void calculateUnrealizedPL(std::vector<IntradayPosition>& positions);
void displayIntradayPositions(const std::vector<IntradayPosition>& positions);
void setNonBlockingMode(bool enable);
void runPositions(const std::string& username);

#endif // INTRADAY_H
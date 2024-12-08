#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
using namespace std;

int main() {
    ifstream infile("KiteConnect_Instruments.csv");
    if (!infile.is_open()) {
        cerr << "Error opening input file.\n";
        return 1;
    }

    ofstream outfile("KiteConnect_filteredInstruments.csv");
    if (!outfile.is_open()) {
        cerr << "Error opening output file.\n";
        return 1;
    }

    string line;
    while (getline(infile, line)) {
        stringstream ss(line);
        string item;
        vector<string> tokens;
        
        while (getline(ss, item, ',')) {
            tokens.push_back(item);
        }

        if (tokens.size() >= 9) { // Ensure there are enough columns
            outfile << tokens[0] << "," << tokens[2] << "," << tokens[8] << "\n";
        }
    }

    infile.close();
    outfile.close();
}
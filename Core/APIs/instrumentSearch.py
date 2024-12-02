import csv
import sys

def get_instrument_token_from_csv(symbol, csv_file='KiteConnect_filteredInstruments.csv'):
    with open(csv_file, 'r') as file:
        reader = csv.reader(file)
        next(reader)  # Skip the header row
        for row in reader:
            if row[1] == symbol:  # Match tradingsymbol
                return row[0]  # Return instrument_token
    return "Instrument not found"

input = sys.argv[1].strip().upper()
print(f"{get_instrument_token_from_csv(input)}")
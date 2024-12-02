import csv

with open('KiteConnect_Instruments.csv', 'r') as file:
    reader = csv.reader(file)
    with open('filtered_instruments.csv', 'w') as output:
        writer = csv.writer(output)
        writer.writerow(['instrument_token', 'tradingsymbol'])  # Header
        for row in reader:
            writer.writerow([row[0], row[2]])  # First and third columns
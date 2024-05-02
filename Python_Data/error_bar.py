# Author: Brian Gomez Jimenez
# Purpose: Visualize data from a CSV file with error bars using Matplotlib.

import pandas as pd
import matplotlib.pyplot as plt

# Load data from CSV file
data = pd.read_csv('/Users/briangomez/downloads/final_error - Sheet1.csv')

# Extracting data
x = data['x_values']  # Assuming the x-values are in a column named 'x_values'
y = data['y_values']  # Assuming the y-values are in a column named 'y_values'
errors = data['errors']  # Assuming the errors are in a column named 'errors'

# Plot with error bars
plt.errorbar(x, y, yerr=errors, fmt='-o', ecolor='red', capsize=15, capthick=2, elinewidth=5)
plt.xlabel('X Label')
plt.ylabel('Y Label')
plt.title('Line graph with Error Bars')
plt.grid(True)
plt.show()

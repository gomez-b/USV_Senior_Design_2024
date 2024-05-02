# Author: Brian Gomez Jimenez
# Purpose: Visualize depth readings from a CSV file and analyze quartile percentages, min, and max values.

import pandas as pd
import matplotlib.pyplot as plt

# Load data from CSV file
data = pd.read_csv('/Users/briangomez/downloads/whisker_plot_pool_depth_python - Sheet1 (1).csv')

# Convert millimeter readings to feet
data['depth_2.97_ft'] = data['depth_2.97'] / 304.8
data['depth_3.97_ft'] = data['depth_3.97'] / 304.8
data['depth_4.97_ft'] = data['depth_4.97'] / 304.8
data['depth_5.97_ft'] = data['depth_5.97'] / 304.8
data['depth_6.97_ft'] = data['depth_6.97'] / 304.8
data['depth_8.67_ft'] = data['depth_8.67'] / 304.8
data['depth_8.97_ft'] = data['depth_8.97'] / 304.8

# Calculate max_value
max_value = data[['depth_2.97_ft', 'depth_3.97_ft', 'depth_4.97_ft', 'depth_5.97_ft', 'depth_6.97_ft', 'depth_8.67_ft','depth_8.97_ft']].max().max()

# Plot the box plot for all columns in feet
plt.figure(figsize=(10, 6))
box = plt.boxplot([data['depth_2.97_ft'], data['depth_3.97_ft'], data['depth_4.97_ft'], data['depth_5.97_ft'], data['depth_6.97_ft'], data['depth_8.67_ft'],data['depth_8.97_ft']])
plt.title('Box Plot of Depth Readings (in feet)',fontsize = 17, fontweight ='bold')
plt.xlabel('Depth Readings', fontsize = 14, fontweight = 'bold')
plt.ylabel('Depth (feet)', fontsize = 14, fontweight = 'bold')

# Set the y-axis limits
plt.ylim(0, max_value * 1.1)  # Adjust the multiplier as needed to leave some space at the top
plt.grid(True)

# Customize tick spacing and labels
tick_positions = range(1, len(data.columns) + 1)
tick_labels = data.columns.tolist()
plt.xticks(tick_positions, tick_labels, rotation=45, ha='right')  # Adjust rotation and alignment as needed

# Adjust tick spacing
plt.locator_params(axis='x', nbins=len(data.columns))  # Set number of ticks to match the number of columns

plt.show()

# Calculate quartiles
quartiles = data[['depth_2.97_ft', 'depth_3.97_ft', 'depth_4.97_ft', 'depth_5.97_ft', 'depth_6.97_ft', 'depth_8.67_ft','depth_8.97_ft']].quantile([0.25, 0.75])

# Plot Q1 and Q3 percentages
plt.figure(figsize=(10, 6))
plt.plot(quartiles.loc[0.25] / max_value * 100, label='Q1 Percent', marker='o')
plt.plot(quartiles.loc[0.75] / max_value * 100, label='Q3 Percent', marker='o')

plt.title('Quartile Percentages (Q1 and Q3)')
plt.xlabel('Depth')
plt.ylabel('Percentage')
plt.legend()
plt.grid(True)
plt.show()


# Calculate min and max values
min_values = data[['depth_2.97_ft', 'depth_3.97_ft', 'depth_4.97_ft', 'depth_5.97_ft', 'depth_6.97_ft', 'depth_8.67_ft','depth_8.97_ft']].min()
max_values = data[['depth_2.97_ft', 'depth_3.97_ft', 'depth_4.97_ft', 'depth_5.97_ft', 'depth_6.97_ft', 'depth_8.67_ft','depth_8.97_ft']].max()

# Plot min and max values
plt.figure(figsize=(10, 6))
plt.plot(min_values, label='Min', marker='o')
plt.plot(max_values, label='Max', marker='o')
plt.title('Min and Max Values')
plt.xlabel('Depth')
plt.ylabel('Depth (feet)')
plt.legend()
plt.grid(True)
plt.show()

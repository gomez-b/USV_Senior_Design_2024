# Author: Brian Gomez Jimenez
# Purpose: Analyze current readings from a CSV file containing coordinates, current readings, and voltage readings.

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import mplcursors

# Read the CSV file containing the coordinates, current readings, and voltage readings
df = pd.read_csv('/Users/briangomez/downloads/python_estuary - Sheet1 (3).csv')

# Plot the scatter plot for coordinates
ax = df.plot(x="longitude", y="latitude", kind="scatter", color="blue", alpha=1, label="Coordinate")

# Plot the first point separately
first_longitude = df['longitude'].iloc[0]
first_latitude = df['latitude'].iloc[0]
plt.plot(first_longitude, first_latitude, marker='*', color='red', markersize=10, label='Starting Point')

# Initialize prev variable
prev = None

# Iterate through the dataframe and add arrows
for row in df[::-1].itertuples():
    if prev:
        ax.annotate('', xy=(prev.longitude, prev.latitude), xytext=(row.longitude, row.latitude),
                    arrowprops=dict(arrowstyle="->", linewidth=1), color='b')
    prev = row

# Set plot title and labels
ax.set_title('Russian River Estuary 2024 Current Readings (mA)', fontweight='bold')
ax.set_xlabel('Longitude', fontweight='bold')
ax.set_ylabel('Latitude', fontweight='bold')

# Add north arrow
x, y, arrow_length = 0.10, 0.15, 0.085
ax.annotate('N', xy=(x, y), xytext=(x, y-arrow_length),
            arrowprops=dict(facecolor='black', width=2, headwidth=8),
            ha='center', va='center', fontsize=15,
            xycoords=ax.transAxes)

# Add northeast arrows
arrow_length = 0.07  # Adjust the length of the arrow
arrows = [(0.1, 0.9), (0.1, 0.7)]#, (0.9, 0.75)]  # Define the coordinates for each arrow

for x_ne, y_ne in arrows:
    ax.annotate('', xy=(x_ne, y_ne), xytext=(x_ne + arrow_length, y_ne - arrow_length),
                arrowprops=dict(facecolor='blue', width=2, headwidth=8),
                ha='center', va='center', fontsize=15,
                xycoords=ax.transAxes)

# Show the legend
plt.legend()

# Plot the current readings
scatter = plt.scatter(df['longitude'], df['latitude'], c=df['current'], cmap='viridis', label='Current Reading')

# Find and annotate peak current readings
peak_indices = df['current'].nlargest(5).index
for idx in peak_indices:
    plt.annotate(f'Peak: {df["current"][idx]} mA', (df['longitude'][idx], df['latitude'][idx]), textcoords="offset points", xytext=(0,10), ha='center')

# Show the colorbar
plt.colorbar(label='Current Reading (mA)')

# Create a new figure for the line graph for current readings
plt.figure()

# Plot the line graph for current readings
plt.plot(df.index, df['current'], color='green', marker='o', linestyle='-', linewidth=1, markersize=3)

# Set plot title and labels
plt.title('Current Readings', fontweight='bold')
plt.xlabel('Index', fontweight='bold')
plt.ylabel('Current (mA)', fontweight='bold')

# Show the grid
plt.grid(True)

# Add annotations for minimum, maximum, and average current peaks
min_current = df['current'].min()
max_current = df['current'].max()
avg_current = df['current'].mean()

plt.annotate(f'Min: {min_current} mA', xy=(df['current'].idxmin(), min_current), xytext=(df['current'].idxmin(), min_current - 10),
             arrowprops=dict(facecolor='black', shrink=0.05),
             fontsize=20, ha='right')

plt.annotate(f'Max: {max_current} mA', xy=(df['current'].idxmax(), max_current), xytext=(df['current'].idxmax(), max_current + 10),
             arrowprops=dict(facecolor='black', shrink=0.05),
             fontsize=20, ha='right')

plt.annotate(f'Avg: {avg_current:.2f} mA', xy=(df.index.min(), avg_current), xytext=(df.index.min(), avg_current + 10),
             arrowprops=dict(facecolor='black', shrink=0.05),
             fontsize=20, ha='right')

# Show the plot
plt.show()

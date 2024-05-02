# Author: Brian Gomez Jimenez
# Purpose: Visualize depth values from a CSV file containing coordinates and depth values.

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
import folium

# Read the CSV file containing the coordinates and depth values
df = pd.read_csv('/Users/briangomez/downloads/python_estuary - Sheet1 (2).csv')

# Convert depth from millimeters to feet
df['depth'] = df['depth'] / 304.8

# Remove duplicate rows if any
df = df.drop_duplicates()

# Create a custom colormap where blue represents deep and red represents shallow
cmap_custom = mcolors.LinearSegmentedColormap.from_list("", ["blue", "red"])

# Create a Folium map centered at the mean of latitude and longitude
mymap = folium.Map(location=[df['latitude'].mean(), df['longitude'].mean()], zoom_start=15)

# Add a line to the map connecting the coordinates
folium.PolyLine(locations=df[['latitude', 'longitude']].values.tolist(), color='blue').add_to(mymap)

# Save the map as an HTML file
mymap.save('/Users/briangomez/downloads/SpringLake_depth.html')

# Calculate average depth
average_depth = df['depth'].mean()

# Plot the scatter plot with depth values
fig, ax = plt.subplots()

# Plot the first point separately
first_longitude = df['longitude'].iloc[0]
first_latitude = df['latitude'].iloc[0]
plt.plot(first_longitude, first_latitude, marker='*', color='red', markersize=10, label='Starting Point')

# Plot depth values using custom colormap
scatter = ax.scatter(df['longitude'], df['latitude'], c=df['depth'], cmap=cmap_custom, s=50, alpha=0.8,
                     label=f"Depth (Avg: {average_depth:.2f} ft)",vmax=28)

# Initialize prev variable
prev = None

# Iterate through the dataframe and add arrows
for row in df[::-1].itertuples():
    if prev:
        ax.annotate('', xy=(prev.longitude, prev.latitude), xytext=(row.longitude, row.latitude),
                    arrowprops=dict(arrowstyle="->", linewidth=1), color='b')
    prev = row

# Set plot title and labels
ax.set_title('Russian River Estuary 2024', fontweight='bold')
ax.set_xlabel('Longitude', fontweight='bold')
ax.set_ylabel('Latitude', fontweight='bold')

# Add colorbar
cbar = plt.colorbar(scatter, ax=ax, label='Depth (ft)')

# Set colorbar label font weight
cbar.ax.yaxis.label.set_fontweight('bold')

# Get legend handles and labels
handles, labels = ax.get_legend_handles_labels()

# Show legend
ax.legend(handles=handles, labels=labels, loc='upper right')

# Show the plot
plt.show()

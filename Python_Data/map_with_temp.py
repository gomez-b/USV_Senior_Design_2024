# Author: Brian Gomez Jimenez
# Purpose: Analyze data from a CSV file containing latitude, longitude, and temperature values.
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import folium

# Read the CSV file containing the coordinates and temperature values
df = pd.read_csv('/Users/briangomez/downloads/python_estuary_ttemp - Sheet1.csv')

# Remove duplicate rows if any
df = df.drop_duplicates()

# Create a Folium map centered at the mean of latitude and longitude
mymap = folium.Map(location=[df['latitude'].mean(), df['longitude'].mean()], zoom_start=10)

# Add a line to the map connecting the coordinates
folium.PolyLine(locations=df[['latitude', 'longitude']].values.tolist(), color='blue').add_to(mymap)

# Save the map as an HTML file
mymap.save('/Users/briangomez/downloads/estuary_maps.html')

# Calculate average temperature
average_temp = df['temperature'].mean()

# Plot the scatter plot with temperature values
fig, ax = plt.subplots()

# Plot temperature values using 'jet' colormap
scatter = ax.scatter(df['longitude'], df['latitude'], c=df['temperature'], cmap='jet', s=50, alpha=0.8,
                     label=f"Temperature (Avg: {average_temp:.2f} °C)")

# Set plot title and labels
ax.set_title('Temperature', fontweight='bold')
ax.set_xlabel('Longitude', fontweight='bold')
ax.set_ylabel('Latitude', fontweight='bold')

# Add colorbar
cbar = plt.colorbar(scatter, ax=ax, label='Temperature (°C)')

# Set colorbar label font weight
cbar.ax.yaxis.label.set_fontweight('bold')

# Get legend handles and labels
handles, labels = ax.get_legend_handles_labels()

# Show legend
ax.legend(handles=handles, labels=labels, loc='upper right')

# Create a new figure for the line graph for voltage readings
plt.figure()

# Plot the line graph for temperature readings against data points
plt.plot(df.index, df['temperature'], color='blue', marker='.', label='Temperature')
plt.axhline(y=average_temp, color='red', linestyle='--', label=f'Average Temperature: {average_temp:.2f} °C')

# Set plot title and labels
plt.title('Temperature vs. Data Points', fontweight='bold')
plt.xlabel('Data Point', fontweight='bold')
plt.ylabel('Temperature (°C)', fontweight='bold')
plt.grid(True)

# Show the legend
plt.legend()

# Show the plot
plt.tight_layout()
plt.show()

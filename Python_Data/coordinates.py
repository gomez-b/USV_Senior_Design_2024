# Author: Brian Gomez Jimenez
# Program: This program takes in latitude and longitude values from a CSV file and then creates a plot showing all coordinates. 
#          The plot can be viewed on an HTML file to visualize the coordinates on a real map.

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import folium

# Read the CSV file containing the coordinates
df = pd.read_csv('/Users/briangomez/downloads/python_estuary - Sheet1.csv')

# Create a Folium map centered at the mean of latitude and longitude
mymap = folium.Map(location=[df['latitude'].mean(), df['longitude'].mean()], zoom_start=100)

# Add a line to the map connecting the coordinates
folium.PolyLine(locations=df[['latitude', 'longitude']].values.tolist(), color='blue').add_to(mymap)

# Add markers for each coordinate
#for index, row in df.iterrows():
   # popup_text = f"Latitude: {row['latitude']}, Longitude: {row['longitude']}"
   # folium.Marker([row['latitude'], row['longitude']], popup=popup_text, icon=folium.Icon(icon='circle', prefix='fa', color='blue', icon_size=(1, 1))).add_to(mymap)
   
# Save the map as an HTML file
mymap.save('/Users/briangomez/downloads/na.html')

# Plot the scatter plot
ax = df.plot(x="longitude", y="latitude", kind="scatter", color="green", alpha=1, colormap="ocean", label="Coordinate")

# Plot the first point separately
first_longitude = df['longitude'].iloc[0]
first_latitude = df['latitude'].iloc[0]
plt.plot(first_longitude, first_latitude, marker='*', color='red', markersize=20, label='Starting Point')

# Initialize prev variable
prev = None

# Iterate through the dataframe and add arrows
for row in df[::-1].itertuples():
    if prev:
        ax.annotate('', xy=(prev.longitude, prev.latitude), xytext=(row.longitude, row.latitude),
                    arrowprops=dict(arrowstyle="->", linewidth=1), color='b')
    prev = row

# Set plot title and labels
ax.set_title('Coverage Path Planning: The Boustrophedon', fontweight='bold')
ax.set_xlabel('Longitude', fontweight='bold')
ax.set_ylabel('Latitude', fontweight='bold')

# Show the legend
plt.legend()

# Show the plot
plt.show()

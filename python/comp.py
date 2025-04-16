import pandas as pd
import matplotlib.pyplot as plt

#interp_file = "interpolated_with_elevation.csv"
#raw_file = "../data/wavefield.csv"


#interp_df = pd.read_csv(interp_file)
#top_elev_interp = interp_df["elevation"].nlargest(2)
#print("🔹 Top 20 interpolated elevation values (from interpolated_with_elevation.csv):")
#print(top_elev_interp.to_string(index=False))

#print("\n" + "="*80 + "\n")


#raw_df = pd.read_csv(raw_file)
#top_z_raw = raw_df["Points:2"].nlargest(2000)
#print("🔸 Top 20 raw z values (from wavefield.csv):")
#print(top_z_raw.to_string(index=False))


interp_file = "../output/interpolated_wavefield.csv"
df = pd.read_csv(interp_file)
df = df[df["timestep"] == 7]
y_slices = [ -97, -93, - 70, -50, 0]
tol = 5

plt.figure(figsize=(10, 6))
for y_val in y_slices:
    slice_df = df[abs(df["y"] - y_val) < tol]
    slice_df = slice_df.sort_values(by="x")
    plt.plot(slice_df["x"], slice_df["eta"], label=f"y ≈ {y_val}")

plt.title("Elevation vs X for different Y-slices (timestep=6)")
plt.xlabel("X [m]")
plt.ylabel("Elevation [m]")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.show()
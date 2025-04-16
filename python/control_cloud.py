import pandas as pd
import matplotlib.pyplot as plt
import os
import numpy as np

# === Datei-Pfade ===
original_path = "../data/wavefield.csv"
interpolated_path = "../output/interpolated_wavefield.csv"

# === CSVs laden ===
print("📥 Lade Dateien...")
df_original = pd.read_csv(original_path)
df_interp = pd.read_csv(interpolated_path)

# === Spaltennamen vereinheitlichen ===
df_original = df_original.rename(columns={
    'TimeStep': 'timestep',
    'velocity:0': 'vx',
    'velocity:1': 'vy',
    'velocity:2': 'vz',
    'pressure': 'pressure',
    'elevation': 'elevation',
    'Points:0': 'x',
    'Points:1': 'y',
    'Points:2': 'z'
})

# Interpoliertes Grid in lowercase (damit Vx -> vx usw.)
df_interp.columns = [col.lower() for col in df_interp.columns]

# === Nur Timestep 0 vergleichen ===
df_original = df_original[df_original['timestep'] == 42]
df_interp = df_interp[df_interp['timestep'] == 42]

# === Komponenten vergleichen ===
components = ['vx', 'vy', 'vz', 'pressure', 'elevation']

for comp in components:
    print(f"\n🔍 Vergleiche: {comp}")
    
    orig = df_original[comp].dropna()
    interp = df_interp[comp].dropna()
    min_len = min(len(orig), len(interp))
    
    orig = orig[:min_len].values
    interp = interp[:min_len].values
    diff = interp - orig

    # 📊 Histogramm: Überlagerung
    plt.figure(figsize=(8, 4))
    plt.hist(orig, bins=100, alpha=0.5, label="Original", color='steelblue')
    plt.hist(interp, bins=100, alpha=0.5, label="Interpoliert", color='darkorange')
    plt.title(f"{comp.upper()}: Original vs. Interpoliert")
    plt.xlabel(comp)
    plt.ylabel("Häufigkeit")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    hist_path = f"compare_{comp}_hist.png"
    plt.savefig(hist_path)
    print(f"📊 Histogramm gespeichert: {hist_path}")

    # 📊 Differenz-Histogramm
    plt.figure(figsize=(8, 4))
    plt.hist(diff, bins=100, color='purple', edgecolor='k')
    plt.title(f"Differenz: Interpoliert - Original ({comp.upper()})")
    plt.xlabel("Differenz")
    plt.ylabel("Häufigkeit")
    plt.grid(True)
    plt.tight_layout()
    diff_path = f"compare_{comp}_diff.png"
    plt.savefig(diff_path)
    print(f"📊 Differenz-Histogramm gespeichert: {diff_path}")

    # 📈 Statistik
    def summarize(name, values):
        print(f"\n📈 Statistik: {name}")
        print(f"  → min:  {np.min(values):.4f}")
        print(f"  → max:  {np.max(values):.4f}")
        print(f"  → mean: {np.mean(values):.4f}")
        print(f"  → std:  {np.std(values):.4f}")

    summarize(f"Original {comp}", orig)
    summarize(f"Interpoliert {comp}", interp)
    summarize(f"Differenz {comp}", diff)

print("\n✅ Vergleich abgeschlossen.")
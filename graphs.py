import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv("100x3MB.csv")
names = ['bmh', 'bmo', 'dmb', 'kmp', 'kra']

plt.figure(figsize=(10, 6))
plt.title("Time vs. Pattern Size (100x3MB)")
plt.xlabel("Pattern Size")
plt.ylabel("Time")
for name in names:
    plt.plot(df['plen'].to_numpy(), df[name].to_numpy(), label=name)
plt.legend()
plt.savefig("100x3MB.png")
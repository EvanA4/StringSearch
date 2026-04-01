import matplotlib.pyplot as plt
import pandas as pd

# df = pd.read_csv("100x3MB.csv")
# names = ['bmh', 'bmo', 'dmb', 'kmp', 'kra']

# plt.figure(figsize=(10, 6))
# plt.title("Time vs. Pattern Size (100x3MB)")
# plt.xlabel("Pattern Size")
# plt.ylabel("Time")
# for name in names:
#     plt.plot(df['plen'].to_numpy(), df[name].to_numpy(), label=name)
# plt.legend()
# plt.savefig("100x3MB.png")

FN_NAMES = ["bmh", "bmo", "dmb", "kmp", "kra"]
N_TRIALS = 100
TEXT_LEN = 300000
DEFAULT_ALEN = 26
DEFAULT_PLEN = 16
ALPHABET_LEN_BOUNDS = (4, 64)
PATTERN_LEN_BOUNDS = (4, 100)

pretty_tlen = ""
if TEXT_LEN > 1_000_000: pretty_tlen = f"{TEXT_LEN/1_000_000:.1f}MB"
else: pretty_tlen = f"{TEXT_LEN//1_000}KB"

df = pd.read_csv("times.csv")
plen_df = df.loc[df["alen"] == DEFAULT_ALEN].sort_values("plen")
alen_df = df.loc[df["plen"] == DEFAULT_PLEN].sort_values("alen")

# pattern length graph
plt.figure(figsize=(10, 6))
plt.title(f"Time vs. Pattern Length ({N_TRIALS}*{pretty_tlen})")
plt.xlabel("Pattern Length")
plt.ylabel("Time (ms)")
plens = plen_df["plen"].to_list()
for fn_name in FN_NAMES:
    times = [num*1000 for num in plen_df[fn_name].to_list()]
    plt.plot(plens, times, label=fn_name if fn_name != "dmb" else "base")
plt.legend(loc="center left", bbox_to_anchor=(1, .5))
plt.show()
plt.close()

# alphabet length graph
plt.figure(figsize=(10, 6))
plt.title(f"Time vs. Alphabet Length ({N_TRIALS}*{pretty_tlen})")
plt.xlabel("Alphabet Length")
plt.ylabel("Time (ms)")
alens = alen_df["alen"].to_list()
for fn_name in FN_NAMES:
    times = [num*1000 for num in alen_df[fn_name].to_list()]
    plt.plot(alens, times, label=fn_name if fn_name != "dmb" else "base")
plt.legend(loc="center left", bbox_to_anchor=(1, .5))
plt.show()
plt.close()
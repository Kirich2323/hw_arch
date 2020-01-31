import sys

import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter, MaxNLocator

fname = sys.argv[1]

csv_raw_lines = open(fname).read().split("\n")
header = csv_raw_lines[9]
data = csv_raw_lines[10:]

lines = {}
stride_info = set()

for l in data:
    
    splitted = l.split(',')
    if len(splitted) < 2:
        break

    name_parts = splitted[0].replace("\"",'').split('/')
    
    block_size = int(name_parts[1])
    if block_size not in lines:
        lines[block_size] = {}
    
    stride = int(name_parts[2])
    stride_info.add(stride)

    #time = int(float(splitted[-1]) * 10**9)
    time = float(splitted[-1]) * 10**9

    lines[block_size][stride] = time

ax = plt.subplot(111)
ax.set_xlabel("Stride")
ax.set_ylabel("Time (ns)")

def bytes_to_string(arr_of_bytes):
    ans = {}
    for x in arr_of_bytes:
        label = 'b'
        y=x
        if x >= 2**30:
            x //= 2**30
            label = 'Gb'
        elif x >= 2**20:
            x //= 2**20
            label = 'Mb'
        elif x >= 2**10:
            x //= 2** 10
            label = 'Kb'
        ans[y]=str(x)+label
    return ans

tick_labels = bytes_to_string(sorted(stride_info))
tick_labels = [ tick_labels[x] for x in sorted(stride_info) ]

ax.set_xscale('log')
ax.set_xticks(sorted(stride_info))
ax.set_xticklabels(tick_labels)
size_info = bytes_to_string(lines.keys())

for i, block_size in enumerate(lines):
    keys=sorted(lines[block_size].keys())
    plt.plot(keys, [lines[block_size][x] for x in keys], marker='o', label=size_info[block_size])

ax.legend()
plt.show()
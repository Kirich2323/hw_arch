
import sys

import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter, MaxNLocator


fname = sys.argv[1]

raw_lines = open(fname).read().split("\n")
stride_info = list(map(int, raw_lines[0].split()))

lines = {}

for l in raw_lines[1:]:
    ls = l.split()
    if len(ls) < 1:
        break
    size = int(ls[0])
    lines[size] = []
    for i,time in enumerate(ls[1:]):
        lines[size].append((stride_info[i], int(time)))

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

tick_labels = bytes_to_string(stride_info)
tick_labels = [ tick_labels[x] for x in stride_info ]

ax.set_xscale('log')
ax.set_xticks(stride_info)
ax.set_xticklabels(tick_labels)
# def format_fn(tick_val, tick_pos):
#     if int(tick_val) in stride_info:
#         return tick_labels[int(tick_val)]
#     else:
#         return ''

# ax.xaxis.set_major_formatter(FuncFormatter(format_fn))
#ax.xaxis.set_major_locator(MaxNLocator(integer=True))


size_info = bytes_to_string(lines.keys())

for i, line in enumerate(lines):
    #str(line), 
    plt.plot(list(map(lambda x: x[0], lines[line])), list(map(lambda x: x[1], lines[line])), marker='o', label=size_info[line])

ax.legend()
plt.show()
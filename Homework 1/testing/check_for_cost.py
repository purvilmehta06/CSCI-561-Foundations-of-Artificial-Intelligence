import sys

file = open(sys.argv[1], 'r')
if sys.argv[2] == "BFS":
    horizontal = 1
    vertical = 1
else:
    horizontal = 14
    vertical = 10
for line in file:
    paths = line.split(' ')
    if "FAIL" or 'FAIL\n' in paths:
        print(-1)
        continue
    calc_cost = 0
    for i in range(len(paths) - 1):
        x1, y1 = paths[i].split(',')
        x2, y2 = paths[i + 1].split(',')
        if x1 == x2 or y1 == y2:
            calc_cost += horizontal
        else:
            calc_cost += vertical
    print(calc_cost)
file.close()
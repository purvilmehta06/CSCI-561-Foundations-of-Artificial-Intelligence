import numpy as np
import math
from queue import deque
import heapq
import sys
from queue import PriorityQueue

def read_file(filename):

    # file open input.txt
    file = open(filename, 'r')

    # reading all single line input
    algorithm = next(file)[:-1]
    w, h = next(file)[:-1].split()
    startY, startX = next(file)[:-1].split()
    stamina = int(next(file)[:-1])
    totalLodges = int(next(file)[:-1])
    lodges = []

    # converting all input to int
    w, h, startX, startY = int(w), int(h), int(startX), int(startY)

    # iterating through all lodges
    for i in range(totalLodges):
        x, y = next(file)[:-1].split()
        lodges.append([int(x), int(y)])
    
    # creating grid w columns and h rows
    grid = np.zeros((h, w), dtype = int)
    for i in range(h):
        temp = next(file)
        if (temp[-1] == '\n'):
            temp = temp[:-1]
        grid[i] = temp.split()
        for j in range(len(grid[i])):
            grid[i][j] = int(grid[i][j])

    return algorithm, w, h, startX, startY, stamina, totalLodges, lodges, grid

def isValid(curr_ele, new_ele, stamina, momentum = 0):
    if (new_ele < 0):
        # return true if tree has height less than or equal to the current elevation
        return abs(new_ele) <= abs(curr_ele)
    else:
        # return true if next location has elevation less than or equal to the current elevation + stamina
        return new_ele <= abs(curr_ele) + stamina + momentum
        
def bfs(w, h, startX, startY, stamina, lodge, grid, dir):
    
    cost, visited = 0, np.zeros((h, w))
    queue = deque()
    queue.append([startX, startY, str(startY) + ',' + str(startX) + ' '])

    # main loop to explore all possible paths
    while (len(queue)):
        size = len(queue)
        while (size > 0):
            size -= 1
            x, y, currPath = queue.popleft()

            # target reached, return cost and path
            if (y == lodge[0] and x == lodge[1]):
                return cost, currPath[:-1]
            
            # already visited earlier hence continue
            if (visited[x][y] == 1): continue

            # check all 8 directions from the current location
            visited[x][y] = 1
            for i in range(8):
                newX, newY = x + dir[i][0], y + dir[i][1]
                if (newX >= 0 and newX < h and newY >= 0 and newY < w and visited[newX][newY] == 0 
                    and isValid(grid[x][y], grid[newX][newY], stamina)):
                    queue.append([newX, newY, currPath + str(newY) + ',' + str(newX) + ' '])
        cost += 1
    return -1, "FAIL"

def uniform_cost_search(w, h, startX, startY, stamina, lodge, grid, dir):

    visited, queue = np.zeros((h, w)), []

    # creating min priority queue to extract minimum distance node
    heapq.heapify(queue)
    heapq.heappush(queue, [0, startX, startY, str(startY) + ',' + str(startX) + ' '])

    # main loop to explore all possible paths
    while (len(queue)):

        # pop the node with smallest distance
        currCost, x, y, currPath = heapq.heappop(queue)

        # target reached, return cost and path
        if (y == lodge[0] and x == lodge[1]):
            return currCost, currPath[:-1]
        
        # check if we have already explored this node or not
        if (visited[x][y] == 1): continue
        visited[x][y] = 1

        for i in range(8):
            newX, newY = x + dir[i][0], y + dir[i][1]
            if (newX >= 0 and newX < h and newY >= 0 and newY < w and visited[newX][newY] == 0 
                and isValid(grid[x][y], grid[newX][newY], stamina)):
                # cost of moving diagonally is 14 and cost of moving horizontally or vertically is 10
                cost = 14 if i < 4 else 10 
                heapq.heappush(queue, [currCost + cost, newX, newY, currPath + str(newY) + ',' + str(newX) + ' '])

    return -1, "FAIL"

def heuristic(x, y, lodge):
    return int(math.sqrt((x - lodge[1])**2 + (y - lodge[0])**2))

def astar(w, h, startX, startY, stamina, lodge, grid, dir):

    queue = PriorityQueue()
    previous_momentum, prev_cost = [[-1]*w for i in range(h)], [[-1]*w for i in range(h)]

    # creating min priority queue to extract minimum distance node
    queue.put([0, 0, 0, (startX, startY), str(startY) + ',' + str(startX) + ' '])

    # main loop to explore all possible paths
    while not queue.empty():

        # pop the node with smallest distance
        _, curr_cost, momentum, curr_loc, currPath = queue.get()
        x, y = curr_loc[0], curr_loc[1]
        momentum = abs(momentum)

        # target reached, return cost and path
        if (y == lodge[0] and x == lodge[1]):
            return curr_cost, currPath[:-1]
                
        if (previous_momentum[x][y] >= momentum):
            continue
    
        # setting up all params for the current node/state
        previous_momentum[x][y] = momentum
        prev_cost[x][y] = curr_cost
        
        for i in range(8):
            newX, newY = x + dir[i][0], y + dir[i][1]
            if (newX >= 0 and newX < h and newY >= 0 and newY < w 
                and isValid(grid[x][y], grid[newX][newY], stamina, momentum=momentum)):

                # new momentum calculation
                new_momentum = max(0, abs(grid[x][y]) -  abs(grid[newX][newY]))
                if (prev_cost[newX][newY] == -1) or previous_momentum[newX][newY] < new_momentum:

                    # cost calculation
                    movementCost = 14 if i < 4 else 10 
                    elevationCost = max(0, abs(grid[newX][newY]) - abs(grid[x][y]) - momentum)
                    heuristicCost = heuristic(newX, newY, lodge)
                    new_curr_cost = curr_cost + movementCost + elevationCost

                    # putting the new node in the queue
                    queue.put([new_curr_cost + heuristicCost, new_curr_cost, -new_momentum, (newX, newY), 
                               currPath + str(newY) + ',' + str(newX) + ' '])

    return -1, "FAIL"

def main():
    ## main driver method
    algorithm, w, h, startX, startY, stamina, totalLodges, lodges, grid = read_file(sys.argv[1])
    dir = [[1, 1], [1, -1], [-1, 1], [-1, -1], [1, 0], [0, 1], [-1, 0], [0, -1]]
    output = open("output.txt", 'w')

    # main logic
    for i in range(totalLodges):
        if (algorithm == 'BFS'):
            cost, path = bfs(w, h, startX, startY, stamina, lodges[i], grid, dir)
        elif (algorithm == 'UCS'):
            cost, path = uniform_cost_search(w, h, startX, startY, stamina, lodges[i], grid, dir)
        else:
            cost, path = astar(w, h, startX, startY, stamina, lodges[i], grid, dir)

        if (i == totalLodges - 1):
            print(path, end = '', file = output)
        else:
            print(path, end = '\n', file = output)
        
    output.close()

if __name__=="__main__":
    main()
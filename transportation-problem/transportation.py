import numpy as np
import math
import copy
import warnings
warnings.filterwarnings("ignore", category=RuntimeWarning) # surpressing All-NaN axis encountered

n = 3  # amount of rows of table (sources)
m = 4  # amount of columns of table (destinations)


class MethodException(Exception):
    def __str__(self):
        return "The method is not applicable!"


class BalanceProblem(Exception):
    def __str__(self):
        return "The problem is not balanced!"


class Table:
    __n = 3
    __m = 4

    def __init__(self, supply, cost, demand):
        self.supply = supply
        self.cost = cost
        self.demand = demand
        if sum(supply) != sum(demand):
            raise BalanceProblem()
        else:
            for i in range(n):
                if supply[i] < 0:
                    raise MethodException()
                for j in range(m):
                    if cost[i][j] < 0 or demand[j] < 0:
                        raise MethodException()

    def print(self):
        print("\nParameter Table of the Transportation Problem")
        for i in range(self.__n):
            for j in range(self.__m):
                print("{:<10}".format(self.cost[i][j]), end=' ')
            print("| {:<10}".format(self.supply[i]))
        print("-" * 10 * self.__m)
        for j in range(self.__m):
            print("{:<10}".format(self.demand[j]), end=' ')
        print()
    
    def update_cost(self):
        # replaces costs with Nan if the corresponding supply or demand is 0
        for i, s in enumerate(self.supply):
            if s == 0:
                for j in range(len(self.demand)):
                    self.cost[i][j] = math.nan

        for j, d in enumerate(self.demand):
            if d == 0:
                for i in range(len(self.supply)):
                    self.cost[i][j] = math.nan


def north_west_method(_table):
    # retuns starting solution obtained with North-west corner method and vector x0
    table = copy.deepcopy(_table)
    pos_x = 0
    pos_y = 0
    res = 0
    x0 = []
    while pos_x != m and pos_y != n:
        x0.append((pos_x, pos_y, table.cost[pos_y][pos_x]))
        if table.supply[pos_y] >= table.demand[pos_x]:
            res += table.cost[pos_y][pos_x] * table.demand[pos_x]
            table.supply[pos_y] -= table.demand[pos_x]
            table.demand[pos_x] = 0
            pos_x += 1
        else:
            res += table.cost[pos_y][pos_x] * table.supply[pos_y]
            table.demand[pos_x] -= table.supply[pos_y]
            table.supply[pos_y] = 0
            pos_y += 1
    return res, x0


def vogel_method(_table):
    # retuns starting solution obtained with Vogel's method and vector x0
    def get_delta(arr):
        # returns the difference between two smallest values in the array
        first = second = math.inf
        for i in range(0, len(arr)):
            if arr[i] < first:
                second = first
                first = arr[i]
            elif (arr[i] < second):
                second = arr[i]
        res = abs(first - second)
        return res
    
    table = copy.deepcopy(_table)
    res = 0
    x0 = []
    while sum(table.supply) > 0 or sum(table.demand) > 0:
        table_cost_transposed = np.array(table.cost).T.tolist()
        # find rows and colums deltas
        row_difs = list(map(get_delta, table.cost))
        col_difs = list(map(get_delta, table_cost_transposed))
        row_dif = np.nanmax(row_difs)
        col_dif = np.nanmax(col_difs)
         
        # find the cost corresponding to the maximum delta
        if row_dif >= col_dif:
            i = row_difs.index(row_dif)
            j = table.cost[i].index(np.nanmin(table.cost[i]))
        else:
            j = col_difs.index(col_dif)
            i = table_cost_transposed[j].index(np.nanmin(table_cost_transposed[j]))

        # increase the resulting cost and save variable in x0
        val = min(table.supply[i], table.demand[j])
        x0.append((i, j, val))
        res += table.cost[i][j] * val

        # update the table
        table.supply[i] -= val
        table.demand[j] -= val
        table.update_cost()

    return res, x0


def russel_method(_table):
    # retuns starting solution obtained with Russel's method and vector x0
    table = copy.deepcopy(_table)
    res = 0
    x0 = []
    while sum(table.supply) > 0 or sum(table.demand) > 0:
        table_cost_transposed = np.array(table.cost).T.tolist() # get transposed table to itterate through columns
        row_maxs = list(map(np.nanmax, table.cost))
        col_maxs = list(map(np.nanmax, table_cost_transposed))
        min_delta = math.inf
        mi, mj = 0, 0
        # find the most negative delta and the corresponding variable
        for i in range(len(table.supply)):
            for j in range(len(table.demand)):
                delta = table.cost[i][j] - row_maxs[i] - col_maxs[j]
                if delta < 0 and delta < min_delta:
                    min_delta = delta
                    mi, mj = i, j

        # increase the resulting cost and save variable in x0
        val = min(table.supply[mi], table.demand[mj])
        x0.append((mi, mj, val))
        res += table.cost[mi][mj] * val

        # update the table
        table.supply[mi] -= val
        table.demand[mj] -= val
        table.update_cost()

    return res, x0

def print_result(res):
    print(f"result: {res[0]}, basic solution: ", end = '')
    x0 = sorted(res[1])
    print(*(f"x{i}{j} = {e}" for i, j, e in x0), sep = ', ')

if __name__ == '__main__':
    # EXAMPLE
    # 140 180 60
    # 2 3 4 2
    # 8 4 1 4
    # 9 7 3 7
    # 60 70 120 130

    # input
    print("Enter a vector of supply coefficients, 3 numbers devided by space:")
    supply_coef = list(map(int, input().split()))
    print("Enter a matrix of cost coefficients, 3 row by 4 numbers devived by space:")
    cost_matrix = []
    for _ in range(n):
        cost_matrix.append(list(map(int, input().split())))
    print("Enter a vector of coefficients of demand, 4 numbers:")
    demand_coef = list(map(int, input().split()))

    try:
        table = Table(supply_coef, cost_matrix, demand_coef)
        table.print()

        # run the algorithms
        print("\nNorth-west method")
        print_result(north_west_method(table))
        print("\nVogel's method")
        print_result(vogel_method(table))
        print("\nRussel's method")
        print_result(russel_method(table))

    except BalanceProblem as e:
        print(e)
    except MethodException as e:
        print(e)

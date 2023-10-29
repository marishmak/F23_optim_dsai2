import numpy as np
import sys


# all the old simplex functions
def get_with_accuracy(n, accuracy):
    temp = round(n * accuracy)
    return float(temp) / accuracy


def get_matrix():
    global A, b, c, n, m, accuracy
    accuracy = 5
    accuracy = 10 ** accuracy

    matrix = [[0.0] * (m + 1) for _ in range(n + 1)]

    for i in range(m):
        matrix[0][i] = -get_with_accuracy(c[i], accuracy)

    for i in range(1, n + 1):
        for j in range(m):
            matrix[i][j] = get_with_accuracy(A[i - 1][j], accuracy)

    for i in range(1, n + 1):
        for j in range(m, m + 1):
            if i == j - m + 1:
                matrix[i][j] = 1
            else:
                matrix[i][j] = 0

    for i in range(1, n + 1):
        matrix[i][m] = get_with_accuracy(b[i - 1], accuracy)

    return matrix


def simplex(simplex_matrix, accuracy):
    n = len(simplex_matrix[0])
    m = len(simplex_matrix)
    variables = [0] * m

    for i in range(n - m + 1, n):
        variables[i - n + m] = i

    while True:

        min_val = simplex_matrix[0][0]
        min_pos = 0

        for i in range(1, n - 1):
            if simplex_matrix[0][i] < min_val:
                min_val = simplex_matrix[0][i]
                min_pos = i

        if min_val >= 0:
            break

        min_ratio = -1
        min_ratio_val = 0
        min_ratio_pos = -1

        for j in range(1, m):
            if simplex_matrix[j][min_pos] != 0 and (
                    simplex_matrix[j][n - 1] / simplex_matrix[j][min_pos] < min_ratio
                    or min_ratio_pos == -1
            ) and (simplex_matrix[j][n - 1] / simplex_matrix[j][min_pos]) > 0:
                min_ratio = simplex_matrix[j][n - 1] / simplex_matrix[j][min_pos]
                min_ratio_pos = j
                min_ratio_val = simplex_matrix[j][min_pos]

        if min_ratio == -1:
            return {}
        else:
            variables[min_ratio_pos] = min_pos + 1

        for i in range(n):
            simplex_matrix[min_ratio_pos][i] = get_with_accuracy(
                simplex_matrix[min_ratio_pos][i] / min_ratio_val, accuracy
            )

        for b in range(m):
            factor = simplex_matrix[b][min_pos]
            for f in range(n):
                if b != min_ratio_pos:
                    simplex_matrix[b][f] = get_with_accuracy(
                        simplex_matrix[b][f] - factor * simplex_matrix[min_ratio_pos][f],
                        accuracy,
                    )

    ret = {}
    for i in range(m):
        ret[variables[i]] = simplex_matrix[i][n - 1]

    return ret


def interior_point():
    global A, b, c, a
    x = X
    while 1:
        initialX = x
        D = np.diag(x)
        AA = np.dot(A, D)

        cc = np.dot(D, c)
        try:
            P = np.identity(m) - np.dot(np.dot(AA.T, np.linalg.inv(np.dot(AA, AA.T))), AA)
        except np.linalg.LinAlgError as e:
            print("Singular matrix error(try take bigger approximation parameter (ex: 0.001)):", e)
            sys.exit(0)
        cp = np.dot(P, cc)
        if len(cp[cp < 0]) == 0:
            print("Bad initial point")
            sys.exit(0)
        v = abs(np.min(cp[cp < 0]))

        xx = np.ones(m) + (a / v) * cp
        x = np.dot(D, xx)
        if (np.linalg.norm(x - initialX) < eps):
            break
    return x


# input
n = int(input("Enter the number of constraints in the linear programming problem: "))
m = int(input("Enter the number of variables in the linear programming problem: "))
print("Enter vector c, such that c^t*X (X is vector of all variables) is the objective function")
c = np.array(list(map(float, input().split())))

print("Enter matrix A")
A = []
for i in range(n):
    A.append(list(map(float, input().split())))
A = np.array(A)

print("Enter vector b, such that Ax=b")
b = np.array(list(map(float, input().split())))

print("Enter approximation accuracy")
eps = float(input())

# Check what all elements in vector b are non-negative
if not all(b >= 0):
    print("The method is not applicable!")
    sys.exit(0)

if not np.linalg.matrix_rank(A) == min(A.shape):
    print("The method is not applicable!")
    sys.exit(0)
print("Enter initial x")
X = np.array(list(map(float, input().split())))
x = X

# Check for a Solution
if not np.allclose(A.dot(x) - b, np.zeros(b.shape), atol=eps):
    print("The problem does not have solution!")
    sys.exit(0)

# Solution using Simplex
accuracy = 0

simplex_matrix = get_matrix()
ans = simplex(simplex_matrix, accuracy)

# Solution using interior point algorithm

a = 0.5
x1 = interior_point()
a = 0.9
x2 = interior_point()

fmax = float(0)

print("Vector of decision variables(Interior point, a = 0.5)")
x1 = ["{:.6f}".format(item) for item in x1]
print(*x1)

for i in range(m):
    fmax += float(c[i]) * float(x1[i])

print('The maximum of the objective function is', fmax)
print()

print("Vector of decision variables(Interior point, a = 0.9)")
x2 = ["{:.6f}".format(item) for item in x2]
print(*x2)

fmax = 0
for i in range(m):
    fmax += float(c[i]) * float(x2[i])

print('The maximum of the objective function is', fmax)
print()

print("Vector of decision variables(simplex)")
x3 = []
for i in range(1, n + 2):
    if i in ans:
        x3.append(ans[i])
    else:
        x3.append(0)
while (len(x3) != m):
    x3.append(0)
x3 = ["{:.6f}".format(item) for item in x3]

fmax = 0
for i in range(m):
    fmax += float(c[i]) * float(x3[i])

print(*x3)

print('The maximum of the objective function is', fmax)

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

void printMatrix(vector<vector<double>>& matrix, vector<int>& variables) {
    // outputs simplex tableu with 5 decimal places
    int n = matrix.size();
    int m = matrix[0].size();
    for (int i = 0; i < n; i++) {
        if (i != 0)
            cout << "x" << variables[i];
        else
            cout << "z ";
        for (int j = 0; j < m; j++) {
            cout << setw(10) << setprecision(5) << matrix[i][j] << ' ';
        }
        cout << '\n';
    }
}

double getWithAccuracy(double n, long long accuracy) {
    // rounds number to log_10(accuracy) decimals
    long long temp = round(n * accuracy);
    return double(temp) / accuracy;
}

vector<vector<double>> getMatrix(long long& accuracy) {
    bool minMaxFlag;
    cout << "Enter \"0\" for minimization or \"1\" for maximization\n";
    cin >> minMaxFlag;

    int n, m;  // n - number of constraints (number of rows in A); m - number of
    // varibles (number of colums in A)
    cout << "Enter number of constrains in linear programming problem\n";
    cin >> n;
    cout << "Enter number of variables in linear programming problem\n";
    cin >> m;

    vector<double> c(m);
    cout << "Enter vector c, such that c^t*X (X is vector of all variables) is "
        "the objective function\n";

    for (int i = 0; i < m; i++) {
        cin >> c[i];
        c[i] *= (minMaxFlag ? 1 : -1);  // change the objective function
        // according to the problem (max/min)
    }
    cout << "Enter matrix A, such that AX <= b\n";
    vector<vector<double>> a(n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            double x;
            cin >> x;
            a[i].push_back(x);
        }
    }
    cout << "Enter vector b, such that AX <= b\n";
    vector<double> b(n);
    for (int i = 0; i < n; i++) {
        cin >> b[i];
        if (b[i] < 0) {
            cout << "Method is not applicable (all elements of b should be positive)";
            exit(0);
        }
    }

    cout << "Enter the maximum number of decimal places after which everything "
        "will be reset to zero (0 - 10).\n";
    cin >> accuracy;
    accuracy = pow(10, accuracy);

    vector<double> temp(
        n + m + 1);  // a vector filled with zeros for table construction
    vector<vector<double>> matrix(n + 1, temp);
    // filling the table
    for (int i = 0; i < m; i++) {
        matrix[0][i] = -getWithAccuracy(c[i], accuracy);
    }
    for (int i = 1; i < n + 1; i++) {
        for (int j = 0; j < m; j++) {
            matrix[i][j] = getWithAccuracy(a[i - 1][j], accuracy);
        }
    }
    for (int i = 1; i < n + 1; i++) {
        for (int j = m; j < n + m + 1; j++) {
            if (i == j - m + 1) {
                matrix[i][j] = 1;
            }
            else {
                matrix[i][j] = 0;
            }
        }
    }
    for (int i = 1; i < n + 1; i++) {
        matrix[i][n + m] = getWithAccuracy(b[i - 1], accuracy);
    }
    return matrix;
}

map<int, double> simplex(vector<vector<double>> simplexMatrix,
    long long accuracy) {
    cout << "The iterations of the algorithm look like this:\n";
    int n = simplexMatrix[0].size();
    int m = simplexMatrix.size();
    vector<int> variables(m);  // for storing indecies of basic variables
    variables[0] = 0;
    for (int i = n - m + 1; i < n; i++) {
        variables[i - n + m] = i;
    }
    // selecting var to proceed
    while (true) {
        printMatrix(simplexMatrix, variables);
        cout << '\n';
        double minVal =
            simplexMatrix[0][0];  // assume the first element is the minimum
        int minPos = 0;           // position of the minimum value
        // iterate through the array to find the minimum value and its position
        for (int i = 1; i < n - 1; i++) {
            if (simplexMatrix[0][i] < minVal) {
                minVal = simplexMatrix[0][i];
                minPos = i;
            }
        }
        // check if the problem is solved
        if (minVal >= 0) {
            break;
        }
        // finding pivot element
        double minRatio = -1;
        double minRatioVal = 0;
        int minRatioPos = -1;
        for (int j = 1; j < m; j++) {
            if ((simplexMatrix[j][n - 1] / simplexMatrix[j][minPos] <
                minRatio ||
                minRatioPos == -1) &&
                (simplexMatrix[j][n - 1] / simplexMatrix[j][minPos]) > 0) {
                minRatio = simplexMatrix[j][n - 1] / simplexMatrix[j][minPos];
                minRatioPos = j;
                minRatioVal = simplexMatrix[j][minPos];
            }
        }
        if (minRatio == -1) {
            return {};
        }
        else {
            variables[minRatioPos] = minPos + 1;
        }
        // calculating new pivot row
        for (int i = 0; i < n; i++) {
            simplexMatrix[minRatioPos][i] = getWithAccuracy(
                simplexMatrix[minRatioPos][i] / minRatioVal, accuracy);
        }
        // obtaining pivot column
        for (int b = 0; b < m; b++) {
            double factor = simplexMatrix[b][minPos];
            for (int f = 0; f < n; f++) {
                if (b != minRatioPos) {
                    simplexMatrix[b][f] = getWithAccuracy(
                        simplexMatrix[b][f] -
                        factor * simplexMatrix[minRatioPos][f],
                        accuracy);
                }
            }
        }
    }
    map<int, double> ret;
    for (int i = 0; i < m; i++) {
        ret[variables[i]] = simplexMatrix[i][n - 1];
    }
    // return solution and decision variables
    return ret;
}

int main() {
    long long accuracy = 0;
    vector<vector<double>> simplexMatrix = getMatrix(accuracy);
    map<int, double> ans = simplex(simplexMatrix, accuracy);
    cout << "The result is: " << ans[0] << '\n';
    cout << "It can be obtained with the following values of variables: \n";
    for (int i = 1; i < simplexMatrix[0].size() - simplexMatrix.size() + 1;
        i++) {
        cout << "x" << i << " = " << ans[i] << '\n';
    }
    return 0;
}

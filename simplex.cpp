/*
example:
1
3 4
2 1 -3 5
1 2 2 4
2 -1 1 2
4 -2 1 -1
40 8 10
2

---Answer:41---


example 2:
1
3 3
9 10 16
18 15 12
6 4 8
5 3 3
360 132 180
5

---Answer:300---


1
2 4
6 8 5 9
2 1 1 3
1 3 1 2
5 3
2

---Answer:17---

1
3 2
2 12
2 4
1 6
1 3
16 12 12
2
*/
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <map>

using namespace std;

void printMatrix(vector <vector <double>>& matrix, vector<int>& variables) {
    // печатает таблицу симплекс метода с 5 знаками после запятой, выделяя под каждый элемент таблицы по 10 символов
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
    // accuracy - это степень 10. Чтобы обнулить все знаки, например, после второго, нужно исходное число умножить на 10^2 записать значение в целочисленную переменную.
    // Потом, чтобы вернуться к исходной мантиссе, делим число на 10^2
    long long temp = round(n * accuracy);
    return double(temp) / accuracy;
}

vector <vector <double>> getMatrix(long long& accuracy) {
    bool minMaxFlag;
    cout << "Enter \"0\" for minimization or \"1\" for maximization\n";
    cin >> minMaxFlag;

    int n, m;
    // n - количество ограничений (кол-во строк)
    // m - количество переменных (кол-во столбцов)
    cout << "Enter number of restrictions in linear programming problem\n";
    cin >> n;
    cout << "Enter number of variables in linear programming problem\n";
    cin >> m;

    vector <double> c(m);
    cout << "Enter vector c, such that c^t*X (X is vector of all variables)-> ";
    if (minMaxFlag) {
        cout << "max\n";
    }
    else {
        cout << "min\n";
    }
    for (int i = 0; i < m; i++) {
        cin >> c[i];
        c[i] *= (minMaxFlag ? 1 : -1); //изменение целевой функции в соответствии с типом задачи (max/min)
    }
    cout << "Enter matrix A, such that AX <= b\n";
    vector <vector<double>> a(n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            double x;
            cin >> x;
            a[i].push_back(x);
        }
    }
    cout << "Enter vector b, such that AX <= b\n";
    vector <double> b(n);
    for (int i = 0; i < n; i++) {
        cin >> b[i];
    }

    cout << "Enter the maximum number of decimal places after which everything will be reset to zero (0 - 10).\n";
    cin >> accuracy;
    accuracy = pow(10, accuracy); // получение степени 10, необходимой для последующих округлений

    vector <double> temp(n + m + 1); // vector из нулей, которым заполняется каждая строка таблицы
    vector <vector <double>> matrix(n + 1, temp);
    for (int i = 0; i < m; i++) { //заполнение таблицы в соответствии с презентацией
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

map<int, double> simplex(vector<vector<double>> simplexMatrix, long long accuracy) {
    cout << "The iterations of the algorithm look like this:\n";
    int n = simplexMatrix[0].size();
    int m = simplexMatrix.size();
    vector <int> variables(m); // хранит индексы переменных базиса
    variables[0] = 0;
    for (int i = n - m + 1; i < n; i++) {
        variables[i - n + m] = i;
    }
    //selecting var to proceed
    while (true) {
        printMatrix(simplexMatrix, variables);
        cout << '\n';
        double minVal = simplexMatrix[0][0]; // Assume the first element is the minimum
        int minPos = 0;     // Position of the minimum value
        // Iterate through the array to find the minimum value and its position
        for (int i = 1; i < n - 1; i++) {
            if (simplexMatrix[0][i] < minVal) {
                minVal = simplexMatrix[0][i];
                minPos = i;
            }
        }
        //проверяем на то, что нам не нужно дальше делать шаги
        if (minVal >= 0) {
            break;
        }
        //находим pivot element
        double minRatio = -1;
        double minRatioVal = 0;
        int minRatioPos = -1;
        for (int j = 1; j < m; j++) {
            if ((simplexMatrix[j][n - 1] / simplexMatrix[j][minPos] < minRatio || minRatioPos == -1) && (simplexMatrix[j][n - 1] / simplexMatrix[j][minPos]) > 0) {
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
        //делаем pivot row
        for (int i = 0; i < n; i++) {
            simplexMatrix[minRatioPos][i] = getWithAccuracy(simplexMatrix[minRatioPos][i] / minRatioVal, accuracy);
        }
        //здесь мы херачим операции, чтобы сделать pivot column
        for (int b = 0; b < m; b++) {
            double mnozhitel = simplexMatrix[b][minPos];
            for (int f = 0; f < n; f++) {
                if (b != minRatioPos) {
                    simplexMatrix[b][f] = getWithAccuracy(simplexMatrix[b][f] - mnozhitel * simplexMatrix[minRatioPos][f], accuracy);
                }
            }
        }
    }
    map<int, double> ret;
    for (int i = 0; i < m; i++) {
        ret[variables[i]] = simplexMatrix[i][n - 1];
    }
    //возвращаем значение solution, а также значения переменных
    return ret;
}

int main() {
    //гОСПОДА, чтобы это работало вам нужно будет сделать инпут основанный на n и m, там ниже показано, что это,
    // далее тупо заполняете матрицу
    long long accuracy = 0;
    vector<vector<double>> simplexMatrix = getMatrix(accuracy);
    map<int, double> ans = simplex(simplexMatrix, accuracy);
    cout << "The result is: " << ans[0] << '\n';
    cout << "It can be obtained with the following values of variables: \n";
    for (int i = 1; i < simplexMatrix[0].size() - simplexMatrix.size() + 1; i++) {
        cout << "x" << i << " = " << ans[i] << '\n';
    }
    return 0;
}

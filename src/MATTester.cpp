#include "MatTester.h"
using std::cout;
using std::endl;

// Function to perform LDU decomposition
void MATTester::lduDecomposition(vector<vector<double>>& matrix, vector<vector<double>>& L, vector<vector<double>>& D, vector<vector<double>>& U) {
    int n = matrix.size();
    
    // Initialize L, D, U matrices with zeros
    L.resize(n, vector<double>(n, 0.0));
    D.resize(n, vector<double>(n, 0.0));
    U.resize(n, vector<double>(n, 0.0));
    
    // Perform LDU decomposition
    for (int i = 0; i < n; i++) {
        // Compute U matrix
        for (int k = i; k < n; k++) {
            double sum = 0.0;
            for (int j = 0; j < i; j++) {
                sum += L[i][j] * U[j][k];
            }
            U[i][k] = matrix[i][k] - sum;
        }
        
        // Compute L matrix
        for (int k = i; k < n; k++) {
            if (i == k) {
                L[i][i] = 1.0;
            } else {
                double sum = 0.0;
                for (int j = 0; j < i; j++) {
                    sum += L[k][j] * U[j][i];
                }
                L[k][i] = (matrix[k][i] - sum) / U[i][i];
            }
        }
        
        // Compute D matrix
        D[i][i] = U[i][i];
        U[i][i] = 1.0;
    }
}

void MATTester::luDecomposition(const std::vector<std::vector<double>>& A, std::vector<std::vector<double>>& L, std::vector<std::vector<double>>& U) {
    int n = A.size();

    // Initialize L and U matrices with zeros
    L.resize(n, std::vector<double>(n, 0.0));
    U.resize(n, std::vector<double>(n, 0.0));

    // Perform LU decomposition
    for (int i = 0; i < n; i++) {
        // Compute upper triangular matrix U
        for (int k = i; k < n; k++) {
            double sum = 0.0;
            for (int j = 0; j < i; j++) {
                sum += L[i][j] * U[j][k];
            }
            U[i][k] = A[i][k] - sum;
        }

        // Compute lower triangular matrix L
        for (int k = i; k < n; k++) {
            if (i == k) {
                L[i][i] = 1.0;
            } else {
                double sum = 0.0;
                for (int j = 0; j < i; j++) {
                    sum += L[k][j] * U[j][i];
                }
                L[k][i] = (A[k][i] - sum) / U[i][i];
            }
        }
    }
}


// Function to print a matrix
void printMatrix(const vector<vector<double>>& matrix) {
    int n = matrix.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
}

void MATTester::testldu(){
    vector<vector<double>> matrix = { {2, -1, 0}, {-1, 2, -1}, {0, -1, 2} };
    vector<vector<double>> L, D, U;

    lduDecomposition(matrix, L, D, U);

    cout << "L matrix:" << endl;
    printMatrix(L);

    cout << "D matrix:" << endl;
    printMatrix(D);

    cout << "U matrix:" << endl;
    printMatrix(U);

    return;
}

void MATTester::testLUDecomposition(){
   std::vector<std::vector<double>> A = { {2, -1, 3}, {4, 2, 1}, {-6, 3, -2} };
    std::vector<std::vector<double>> L, U;

    luDecomposition(A, L, U);

    std::cout << "Matrix A:" << std::endl;
    printMatrix(A);

    std::cout << "Matrix L:" << std::endl;
    printMatrix(L);

    std::cout << "Matrix U:" << std::endl;
    printMatrix(U);

    //std::cout << "Matrix U:" << std::endl;
    //printMatrix(U);

}
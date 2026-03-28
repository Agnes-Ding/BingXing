#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <cmath>

using namespace std;
using namespace std::chrono;

// --- 矩阵-向量内积算法 ---

// 平凡算法：逐列访问 (Cache 不友好)
void matrix_vector_col(int n, const vector<vector<double>>& B, const vector<double>& a, vector<double>& sum) {
    for (int i = 0; i < n; i++) {
        sum[i] = 0.0;
        for (int j = 0; j < n; j++) {
            sum[i] += B[j][i] * a[j];
        }
    }
}

// Cache 优化算法：逐行访问 (Cache 友好)
void matrix_vector_row(int n, const vector<vector<double>>& B, const vector<double>& a, vector<double>& sum) {
    for (int i = 0; i < n; i++) sum[i] = 0.0;
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < n; i++) {
            sum[i] += B[j][i] * a[j];
        }
    }
}

// --- n 个数求和算法 ---

// 平凡算法：单路累加
double sum_normal(int n, const vector<double>& a) {
    double total = 0.0;
    for (int i = 0; i < n; i++) total += a[i];
    return total;
}

// 超标量优化：两路链式累加
double sum_superscalar(int n, const vector<double>& a) {
    double sum1 = 0.0, sum2 = 0.0;
    for (int i = 0; i < n; i += 2) {
        sum1 += a[i];
        sum2 += a[i + 1];
    }
    return sum1 + sum2;
}

int main() {
    cout << fixed << setprecision(3);

    // --- 实验 1: 矩阵规模对比测试 ---
    cout << "### 实验 1: 矩阵-向量内积性能测试" << endl;
    cout << "| 规模 N | 平凡耗时 (ms) | 优化耗时 (ms) | 加速比 |" << endl;
    cout << "| :--- | :--- | :--- | :--- |" << endl;

    vector<int> matrix_sizes = {100, 500, 1000, 5000, 10000};
    for (int n : matrix_sizes) {
        vector<vector<double>> B(n, vector<double>(n, 1.1));
        vector<double> a(n, 2.2), res(n, 0.0);

        // 矩阵运算视规模决定重复次数，避免跑太久
        int repeat = (n <= 1000) ? 100 : 10;

        auto s1 = high_resolution_clock::now();
        for (int r = 0; r < repeat; r++) matrix_vector_col(n, B, a, res);
        auto e1 = high_resolution_clock::now();
        double t1 = duration_cast<microseconds>(e1 - s1).count() / 1000.0 / repeat;

        auto s2 = high_resolution_clock::now();
        for (int r = 0; r < repeat; r++) matrix_vector_row(n, B, a, res);
        auto e2 = high_resolution_clock::now();
        double t2 = duration_cast<microseconds>(e2 - s2).count() / 1000.0 / repeat;

        cout << "| " << n << " | " << t1 << " | " << t2 << " | " << t1 / t2 << "x |" << endl;
    }

    // --- 实验 2: 求和规模对比测试 ---
    cout << "\n### 实验 2: n 个数求和性能测试" << endl;
    cout << "| 规模 n | 平凡耗时 (us) | 优化耗时 (us) | 加速比 |" << endl;
    cout << "| :--- | :--- | :--- | :--- |" << endl;

    vector<int> sum_sizes = {10000, 100000, 1000000, 10000000};
    for (int n : sum_sizes) {
        vector<double> data(n, 1.0);
        int repeat = 1000;

        auto s1 = high_resolution_clock::now();
        for (int r = 0; r < repeat; r++) sum_normal(n, data);
        auto e1 = high_resolution_clock::now();
        double t1 = duration_cast<nanoseconds>(e1 - s1).count() / 1000.0 / repeat;

        auto s2 = high_resolution_clock::now();
        for (int r = 0; r < repeat; r++) sum_superscalar(n, data);
        auto e2 = high_resolution_clock::now();
        double t2 = duration_cast<nanoseconds>(e2 - s2).count() / 1000.0 / repeat;

        cout << "| 10^" << (int)log10(n) << " | " << t1 << " | " << t2 << " | " << t1 / t2 << "x |" << endl;
    }

    return 0;
}
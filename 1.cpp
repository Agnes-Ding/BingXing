#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace std::chrono;


const int MAX_N = 10000;      // 矩阵规模 n*n
const int SUM_N = 10000000;   // 求和规模 n
const int REPEAT = 100;       // 重复次数，用于消除计时误差

//矩阵-向量内积

// 1. 平凡算法：逐列访问
void matrix_vector_col(int n, const vector<vector<double>>& B, const vector<double>& a, vector<double>& sum) {
    for (int i = 0; i < n; i++) {
        sum[i] = 0.0;
        for (int j = 0; j < n; j++) {
           
            sum[i] += B[j][i] * a[j];
        }
    }
}

// 2. Cache优化算法：逐行访问
void matrix_vector_row(int n, const vector<vector<double>>& B, const vector<double>& a, vector<double>& sum) {
    for (int i = 0; i < n; i++) sum[i] = 0.0;
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < n; i++) {
            // 连续访问 B[j][i]，极大地提高了 Cache 命中率
            sum[i] += B[j][i] * a[j];
        }
    }
}

// n个数求和

// 1. 平凡算法：单路链式累加
double sum_normal(int n, const vector<double>& a) {
    double total = 0.0;
    for (int i = 0; i < n; i++) {
        total += a[i];
    }
    return total;
}

// 2. 超标量优化算法：两路链式累加 (利用指令级并行)
double sum_superscalar(int n, const vector<double>& a) {
    double sum1 = 0.0, sum2 = 0.0;
    for (int i = 0; i < n; i += 2) {
        sum1 += a[i];
        sum2 += a[i + 1]; // sum1和sum2的更新互不依赖，CPU可并行执行
    }
    return sum1 + sum2;
}

//测试与计时主程序

int main() {
    // 数据初始化
    cout << "正在初始化测试数据..." << endl;
    vector<vector<double>> B(MAX_N, vector<double>(MAX_N));
    vector<double> a(MAX_N), res(MAX_N, 0.0);
    for (int i = 0; i < MAX_N; i++) {
        a[i] = i;
        for (int j = 0; j < MAX_N; j++) B[i][j] = i + j;
    }

    vector<double> data_sum(SUM_N);
    for (int i = 0; i < SUM_N; i++) data_sum[i] = i;

    cout << fixed << setprecision(6);
    cout << "--------------------------------------------------" << endl;
    cout << "实验任务 1: 矩阵-向量内积 (规模 n = " << MAX_N << ")" << endl;

    // 测试平凡算法
    auto start = high_resolution_clock::now();
    for (int r = 0; r < 10; r++) matrix_vector_col(MAX_N, B, a, res); // 矩阵运算较慢，重复10次
    auto end = high_resolution_clock::now();
    cout << "平凡算法平均耗时: " << duration_cast<milliseconds>(end - start).count() / 10.0 << " ms" << endl;

    // 测试优化算法
    start = high_resolution_clock::now();
    for (int r = 0; r < 10; r++) matrix_vector_row(MAX_N, B, a, res);
    end = high_resolution_clock::now();
    cout << "优化算法平均耗时: " << duration_cast<milliseconds>(end - start).count() / 10.0 << " ms" << endl;

    cout << "\n实验任务 2: n 个数求和 (规模 n = " << SUM_N << ")" << endl;

    // 测试平凡算法
    start = high_resolution_clock::now();
    for (int r = 0; r < REPEAT; r++) sum_normal(SUM_N, data_sum);
    end = high_resolution_clock::now();
    cout << "平凡算法平均耗时: " << duration_cast<microseconds>(end - start).count() / (double)REPEAT << " us" << endl;

    // 测试超标量优化
    start = high_resolution_clock::now();
    for (int r = 0; r < REPEAT; r++) sum_superscalar(SUM_N, data_sum);
    end = high_resolution_clock::now();
    cout << "超标量优化平均耗时: " << duration_cast<microseconds>(end - start).count() / (double)REPEAT << " us" << endl;

    return 0;
}
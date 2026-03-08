#include <iostream>
#include <chrono>
#include <vector>

using namespace std;
using namespace chrono;

int main()
{
    unsigned int rowsA = 2, colsA = 3;
    unsigned int rowsB = 3, colsB = 2;

    // 행렬 A (2x3), 순차적으로 값 넣기
    vector<int> A(rowsA * colsA);
    for (unsigned int i = 0; i < A.size(); ++i) A[i] = i + 1;

    // 행렬 B (3x2), 순차적으로 값 넣기
    vector<int> B(rowsB * colsB);
    for (unsigned int i = 0; i < B.size(); ++i) B[i] = i + 1;

    // 결과 행렬 C (2x2)
    vector<int> C(rowsA * colsB, 0);


	for (unsigned int y = 0; y < dimsA.y; ++y)
		for (unsigned int x = 0; x < dimsB.x; x++) {
			for (unsigned int i = 0; i < dimsA.x; ++i)
				h_C[y * dimsB.x + x] += h_A[i + y * dimsA.x] * h_B[i * dimsB.x + x];
		}
}
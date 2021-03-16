#pragma once
#ifndef FFT1_H_
#define FFT1_H_
#include <iostream>
#define  MAX_MATRIX_SIZE 4194304             // 2048 * 2048
#define  PI 3.141592653
#define  MAX_VECTOR_LENGTH 10000

typedef struct CComplex
{
	double rl;
	double im;
}CComplex;

class CFft1
{
public:
	CFft1(void);
	~CFft1(void);

public:
	bool fft(CComplex const inVec[], int const len, CComplex outVec[]);   // 基于蝶形算法的快速傅里叶变换
	bool ifft(CComplex const inVec[], int const len, CComplex outVec[]);

	bool is_power_of_two(int num);
	int  get_computation_layers(int num);        // calculate the layers of computation needed for FFT
};

#endif

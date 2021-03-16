#pragma once
#ifndef FILTER_H_
#define FILTER_H_
#include<iostream>
#include<Fft1.h>
#define  SARANGE 4   //一次滑动平均的数据量
#define SIZE 8 //采样点数
#define CHANGE 4 //滤波点数
#define FRETHRESHOLD 0.1 //频谱判断的阈值

class Saver//平滑平均值类
{
public:
	Saver() { outVec[SIZE] = { 0 }; finVec[SIZE] = { 0 }; }
	~Saver() {}
public:
	float SaFunc(float ear);//每次输入特征值ear,返回SARANGE个值平均后所得的值
	float filterFunc(float ear); //滤波处理函数，输入特征值，返回滤波处理后的值
   	int freBlink(float ear); //使用频谱图判断眨眼。注意，里面也用了static inVec数组与static fi！！不能与filterFunc共用！
private:
	static float funcRange[SARANGE]; //储存一次处理数据的数组
	static CComplex inVec[SIZE]; //储存输入ear的数组
	CComplex outVec[SIZE]; //储存FFT算法处理后的数组
	CComplex finVec[SIZE]; //逆运算后储存
	static int fi;
};

float Saver::funcRange[SARANGE] = { 0 };
int Saver::fi = 0;
CComplex Saver::inVec[SIZE] = { 0 };

float Saver::SaFunc(float ear)//滑动平均值算法
{
	float result = 0;//平均结果

	for (int i = 0; i < SARANGE - 1; i++)
	{
		funcRange[i] = funcRange[i + 1];
	}
	funcRange[SARANGE - 1] = ear;
	result = funcRange[0] * 0.1 + funcRange[1] * 0.2 + funcRange[2] * 0.3 + funcRange[3] * 0.4;
	return result;
}

float Saver::filterFunc(float ear)
{
	float Sear = SaFunc(ear); //均值滤波
	int j = 0;
	CFft1 t;

	if (fi < SIZE)//第一组SIZE取满，全部取值不画图片
	{
		inVec[fi].rl = Sear;
		fi++;
		return 0;
	}
	else//当第一组取满后
	{
		t.fft(inVec, SIZE, outVec);//FFT算法实现
		for (j = SIZE / 2 - CHANGE; j <= SIZE / 2 + CHANGE; j++)//高频的振幅置0滤波
		{
			outVec[j].rl = 0;
			outVec[j].im = 0;
		}
		t.ifft(outVec, SIZE, finVec);//FFT逆运算

		for (j = 0; j < SIZE - 1; j++) //将新的Sear值存入inVec中用于下一次FFT
		{
			inVec[j].rl = inVec[j + 1].rl;
		}
		inVec[SIZE - 1].rl = Sear;

		return pow(finVec[SIZE - 2].rl * 5, 2); //平方变换
	}
}

int Saver::freBlink(float ear)
{
	Saver f;
	CFft1 t;
	float finRecord[SIZE] = { 0 };
	float dis = 0; //频谱两端与中间值的距离
	static int s_closeMark = 0;//标志
	static float s_lastDis = 0;//上一次的area值
	float Sear = f.SaFunc(ear); //平滑均值处理ear

	if (fi < SIZE)
	{
		inVec[fi].rl = Sear;
		fi++;
		return 0;
	}
	else
	{
		t.fft(inVec, SIZE, outVec);
		for (int i = 0; i < SIZE; i++) //将复数化成模
		{
			finRecord[i] = sqrt(pow(outVec[i].rl, 2) + pow(outVec[i].im, 2));
		}
		for (int j = 0; j < SIZE - 1; j++)
		{
			inVec[j].rl = inVec[j + 1].rl;
		}
		inVec[SIZE - 1].rl = Sear;

		dis = (finRecord[1] + finRecord[SIZE - 1]) / 2 - finRecord[SIZE/2];
	}

	if (dis > FRETHRESHOLD + 0.02 && dis > s_lastDis && s_closeMark == 0)
	{
		s_closeMark = 1;
		s_lastDis = dis;
	}
	else if (dis < FRETHRESHOLD && dis < s_lastDis && s_closeMark == 1)
	{
		s_lastDis = dis;
		s_closeMark = 0;
		return 1;
	}
	else
	{
		s_lastDis = dis;
		return 0;
	}
	return 0;
}
#endif // !FILTER_H_


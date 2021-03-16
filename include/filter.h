#pragma once
#ifndef FILTER_H_
#define FILTER_H_
#include<iostream>
#include<Fft1.h>
#define  SARANGE 4   //һ�λ���ƽ����������
#define SIZE 8 //��������
#define CHANGE 4 //�˲�����
#define FRETHRESHOLD 0.1 //Ƶ���жϵ���ֵ

class Saver//ƽ��ƽ��ֵ��
{
public:
	Saver() { outVec[SIZE] = { 0 }; finVec[SIZE] = { 0 }; }
	~Saver() {}
public:
	float SaFunc(float ear);//ÿ����������ֵear,����SARANGE��ֵƽ�������õ�ֵ
	float filterFunc(float ear); //�˲�����������������ֵ�������˲�������ֵ
   	int freBlink(float ear); //ʹ��Ƶ��ͼ�ж�գ�ۡ�ע�⣬����Ҳ����static inVec������static fi����������filterFunc���ã�
private:
	static float funcRange[SARANGE]; //����һ�δ������ݵ�����
	static CComplex inVec[SIZE]; //��������ear������
	CComplex outVec[SIZE]; //����FFT�㷨����������
	CComplex finVec[SIZE]; //������󴢴�
	static int fi;
};

float Saver::funcRange[SARANGE] = { 0 };
int Saver::fi = 0;
CComplex Saver::inVec[SIZE] = { 0 };

float Saver::SaFunc(float ear)//����ƽ��ֵ�㷨
{
	float result = 0;//ƽ�����

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
	float Sear = SaFunc(ear); //��ֵ�˲�
	int j = 0;
	CFft1 t;

	if (fi < SIZE)//��һ��SIZEȡ����ȫ��ȡֵ����ͼƬ
	{
		inVec[fi].rl = Sear;
		fi++;
		return 0;
	}
	else//����һ��ȡ����
	{
		t.fft(inVec, SIZE, outVec);//FFT�㷨ʵ��
		for (j = SIZE / 2 - CHANGE; j <= SIZE / 2 + CHANGE; j++)//��Ƶ�������0�˲�
		{
			outVec[j].rl = 0;
			outVec[j].im = 0;
		}
		t.ifft(outVec, SIZE, finVec);//FFT������

		for (j = 0; j < SIZE - 1; j++) //���µ�Searֵ����inVec��������һ��FFT
		{
			inVec[j].rl = inVec[j + 1].rl;
		}
		inVec[SIZE - 1].rl = Sear;

		return pow(finVec[SIZE - 2].rl * 5, 2); //ƽ���任
	}
}

int Saver::freBlink(float ear)
{
	Saver f;
	CFft1 t;
	float finRecord[SIZE] = { 0 };
	float dis = 0; //Ƶ���������м�ֵ�ľ���
	static int s_closeMark = 0;//��־
	static float s_lastDis = 0;//��һ�ε�areaֵ
	float Sear = f.SaFunc(ear); //ƽ����ֵ����ear

	if (fi < SIZE)
	{
		inVec[fi].rl = Sear;
		fi++;
		return 0;
	}
	else
	{
		t.fft(inVec, SIZE, outVec);
		for (int i = 0; i < SIZE; i++) //����������ģ
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


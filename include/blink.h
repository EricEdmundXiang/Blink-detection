#pragma once
#ifndef BLINK_H_;
#define BLINK_H_;
#define VARNUM 5//һ�η����ж�ʹ�õ�������
//�����ж�գ�۵ĺ�����ͷ�ļ�
//����һ����̬��ֵ+����Dynamic threshold + variance
class Dtv
{
public:
	Dtv(){}//���캯��
	static float remember[VARNUM];
	int DtvFunc(float ear);//Dynamic + variance
	~Dtv() {}//��������
};

float Dtv::remember[VARNUM] = { 0 };

int Dtv::DtvFunc(float ear)
{

}
#endif // !BLINK_H_;


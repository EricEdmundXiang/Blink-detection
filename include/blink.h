#pragma once
#ifndef BLINK_H_;
#define BLINK_H_;
#define VARNUM 5//一次方差判断使用的数据量
//用来判断眨眼的函数的头文件
//方法一：动态阈值+方差Dynamic threshold + variance
class Dtv
{
public:
	Dtv(){}//构造函数
	static float remember[VARNUM];
	int DtvFunc(float ear);//Dynamic + variance
	~Dtv() {}//析构函数
};

float Dtv::remember[VARNUM] = { 0 };

int Dtv::DtvFunc(float ear)
{

}
#endif // !BLINK_H_;


//使用眼睛面积判断的类
//阈值AREATHRESHOLD 350
//最大值AREAMAX 800
#ifndef AREA_H_
#define AREA_H_
#include<iostream>
#include "opencv2/opencv.hpp"
using namespace cv;
using namespace std;
/*class Area
{
private:
	float sumArea;//眼睛的面积
	static int s_closeMark;//标志
	static float s_lastArea;//上一次的area值
public:
	const int AREATHRESHOLD;
	const int AREMAX;
	Area(void) :AREATHRESHOLD(300), AREMAX(800)
	{
		sumArea = 0;
	}
	float method_area(Point *eye); //计算面积，输入Point数组
	int if_blink(float area);//计算是否眨眼，输入area特征值
};

int Area::s_closeMark = 0;
float Area::s_lastArea = 0;

float Area::method_area(Point *eye)
{
	int i;
	sumArea = 0;
	for (i = 1; i <= 4; i++)//return abs((x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1))/2.0
	{
		sumArea += (eye[i + 1].x - eye[1].x)*(eye[i + 2].y - eye[1].y) - (eye[i + 1].y - eye[1].y)*(eye[i + 2].x - eye[1].x);
	}
	//	cout << sumArea << endl;
	return sumArea;
}

int Area::if_blink(float area)
{
	if (area < AREATHRESHOLD && area < s_lastArea && s_closeMark == 0)
	{
		s_closeMark = 1;
		s_lastArea = area;
	}
	else if (area > AREATHRESHOLD && area > s_lastArea && s_closeMark == 1)
	{
		s_lastArea = area;
		s_closeMark = 0;
		return 1;
	}
	else
	{
		s_lastArea = area;
		return 0;
	}
	return 0;
}*/

class Ear
{
private:
//	static float last_ear;
//	static int close_mark;
//	static int blow_mark;
//	static int open_mark;
//	static int stable_mark;
//	static int ftp;
public:
//	const float BLINK_THRESHOLD;
//	const float MAX_EAR;
	Ear(void) //:BLINK_THRESHOLD(0.3), MAX_EAR(0.8)
	{}
	float methond_EAR(Point *eye);
//	int if_blink(float ear);
};

//float Ear::last_ear = 0;
//int Ear::close_mark = 0;
//int Ear::blow_mark = 0;
//int Ear::open_mark = 0;
//int Ear::stable_mark = 0;
//int Ear::ftp = 0;

float Ear::methond_EAR(Point *eye)
{
	float a1 = sqrt(pow(eye[2].x - eye[6].x, 2) + pow(eye[2].y - eye[6].y, 2));
	float a2 = sqrt(pow(eye[3].x - eye[5].x, 2) + pow(eye[3].y - eye[5].y, 2));
	float a = (a1 + a2) / 2;
	float b = sqrt(pow(eye[1].x - eye[4].x, 2) + pow(eye[1].y - eye[4].y, 2));
	float ear1 = a / b;

	a1= sqrt(pow(eye[8].x - eye[12].x, 2) + pow(eye[8].y - eye[12].y, 2));
	a2 = sqrt(pow(eye[9].x - eye[11].x, 2) + pow(eye[9].y - eye[11].y, 2));
	a = (a1 + a2) / 2;
	b = sqrt(pow(eye[7].x - eye[10].x, 2) + pow(eye[7].y - eye[10].y, 2));
	float ear2 = a / b;

	return (ear1 + ear2) / 2;
}

/*int Ear::if_blink(float ear)
{
	if (ear < BLINK_THRESHOLD)//在阈值以下
	{
		if (1 == close_mark)blow_mark = 1;//阈值以下且有下降沿
		else blow_mark = 0;
	}
	else
	{
		if (close_mark == 0)//下降沿
		{
			if (ear <= last_ear) close_mark = 1;
			else close_mark = 0;
		}
		else if ((close_mark == 1 && blow_mark == 1))//在阈值以上且有下降和上升过程
		{
			if (ear > last_ear)
			{
				stable_mark = 1;
			}

			else
			{

				last_ear = ear;
//				cout << "frame=" << ftp << endl;
				close_mark = blow_mark = open_mark = ftp = stable_mark = 0;
				return 1;
			}
		}
	}
//	if (close_mark == 1 || stable_mark == 1)ftp++;
//	else ftp = 0;
	last_ear = ear;
	return 0;
}*/

#endif

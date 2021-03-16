#pragma once
#ifndef _ANGLE_H_
#define _ANGLE_H_
#include "opencv2/opencv.hpp"
#include <iostream>
#define MOVINGAVERAGE_NUM 4       //均值滤波中数据个数
#define THRESHOLD_NUM 30           //动态阈值中数据个数
using namespace cv;
using namespace std;
class ANGLE
{
public:
	long int blink_times;
	float angle;
	ANGLE();
	float cal_ANGLE(Point *remember);
	bool if_blink(int angle_threshold);
	float change_threshold(Point *remember);
	~ANGLE();

private:
	bool close_mark;
	bool blow_mark;
	bool open_mark;
	bool stable_mark;
	int ftp;
	static float ANGLE_threshold;
};
float ANGLE::ANGLE_threshold = 35;

ANGLE::ANGLE()
{
	close_mark = 0;
	blow_mark = 0;
	open_mark = 0;
	stable_mark = 0;
	ftp = 0;

	blink_times = 0;
	angle = 0;
}
float ANGLE::cal_ANGLE(Point *remember)
{
	int position = 0;
	int init_flag = 0;
	int pulse_loop, pulse_temp;
	int pulse_sum = 0;
	int pulse;
	static int pulse_queue[MOVINGAVERAGE_NUM] = { 0 };
	pulse_temp = (float)(180 / 3.1415926) * acos(((remember[2].x - remember[1].x) * (remember[5].x - remember[1].x) + (remember[2].y - remember[1].y) * (remember[5].y - remember[1].y)) / (sqrt(pow(remember[2].x - remember[1].x, 2) + pow(remember[2].y - remember[1].y, 2)) * sqrt(pow(remember[5].x - remember[1].x, 2) + pow(remember[5].y - remember[1].y, 2))));
   	
		if (init_flag == 0)
		{
			for (pulse_loop = 0; pulse_loop < MOVINGAVERAGE_NUM; pulse_loop++)
			{
				pulse_queue[pulse_loop] = pulse_temp;
			}
			init_flag = 1;
		}
		else
		{
			pulse_queue[position] = pulse_temp;
			if (position == (MOVINGAVERAGE_NUM - 1))
			{
				position = 0;
			}
			else
			{
				position++;
			}
		}
		for (pulse_loop = 0; pulse_loop < MOVINGAVERAGE_NUM; pulse_loop++)
		{
			pulse_sum += pulse_queue[pulse_loop]*(1.6 - pulse_loop*0.4);
		}

		pulse = (float)pulse_sum / MOVINGAVERAGE_NUM;
		return pulse;
}

bool ANGLE::if_blink(int angle_threshold)
{
	static float last_angle;
	static int ftp;

	if (angle < angle_threshold)//在阈值以下
	{
		if (1 == close_mark)blow_mark = 1;//阈值以下且有下降沿
		else blow_mark = 0;
	}
	else
	{
		if (close_mark == 0)//下降沿
		{
			if (angle <= last_angle) close_mark = 1;
			else close_mark = 0;
		}
		else if ((close_mark == 1 && blow_mark == 1))//在阈值以上且有下降和上升过程
		{
			if (angle > last_angle)
			{
				stable_mark = 1;
			}
			else
			{

				last_angle = angle;
				cout << "frame=" << ftp << endl;
				close_mark = blow_mark = open_mark = ftp = stable_mark = 0;
				return 1;
			}
		}
	}
	if (close_mark == 1 || stable_mark == 1)ftp++;
	else ftp = 0;
	last_angle = angle;
	return 0;

}
ANGLE::~ANGLE()
{
}
float ANGLE::change_threshold(Point *remember)
{
	int i;
	int high = 0, low = 0;
	float a[65535];
	for (i = 0; i < 65535; i++)
	{
		a[i] = (float)(180 / 3.1415926) * acos(((remember[2].x - remember[1].x) * (remember[5].x - remember[1].x) + (remember[2].y - remember[1].y) * (remember[5].y - remember[1].y)) / (sqrt(pow(remember[2].x - remember[1].x, 2) + pow(remember[2].y - remember[1].y, 2)) * sqrt(pow(remember[5].x - remember[1].x, 2) + pow(remember[5].y - remember[1].y, 2))));
	}
	for (i = 0; i < 65535-1; i++)
	{
		if (a[high] < a[i+1])
		{
			high = i+1;
		}
		if (a[low] > a[i+1])
		{
			low = i+1;
		}
	}
	return (a[high] + a[low]) / 2;
}
#endif
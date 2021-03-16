#include <vector>
#include <iostream>
#include <fstream>


#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "include/area.h"
#include "include/angle_class.h"

#include "ldmarkmodel.h"

using namespace std;
using namespace cv;
Mat draw_chart(float area,float ear,float angle);
#define RECOREDSIZE 251
#define BLINK_THRESHOLD 0.3
#define MAX_EAR 0.8
#define AREA_THRESHOLD 300
#define MAX_AREA 800
#define ANGLE_threshold 35
#define MAX_ANGLE 90
int main()
{
	/*********************
    std::vector<ImageLabel> mImageLabels;
    if(!load_ImageLabels("mImageLabels-test.bin", mImageLabels)){
        mImageLabels.clear();
        ReadLabelsFromFile(mImageLabels, "labels_ibug_300W_test.xml");
        save_ImageLabels(mImageLabels, "mImageLabels-test.bin");
    }
    std::cout << "测试数据一共有: " <<  mImageLabels.size() << std::endl;
	*******************/

    ldmarkmodel modelt;
	Area Carea;//面积类
	Ear Cear;//纵横比类
	ANGLE Cangle;//角度类
	float ear, area, angle;
    std::string modelFilePath = "F:\\testProj\\sdm_test\\test\\roboman-landmark-model.bin";
    while(!load_ldmarkmodel(modelFilePath, modelt)){
        std::cout << "文件打开错误，请重新输入文件路径." << std::endl;
        std::cin >> modelFilePath;
    }

    cv::VideoCapture mCamera(0);
    if(!mCamera.isOpened()){
        std::cout << "Camera opening failed..." << std::endl;
        system("pause");
        return 0;
    }
    cv::Mat Image;
	cv::Mat blink= draw_chart(0,0,0);
    cv::Mat current_shape;
	for (;;) {
		mCamera >> Image;
		modelt.track(Image, current_shape);
		cv::Vec3d eav;
		modelt.EstimateHeadPose(current_shape, eav);
		modelt.drawPose(Image, current_shape, 50);
		Point remember[7];
		int num_re = 0;
		int numLandmarks = current_shape.cols / 2;
		for (int j = 0; j < numLandmarks; j++) {
			int x = current_shape.at<float>(j);
			int y = current_shape.at<float>(j + numLandmarks);
			std::stringstream ss;
			ss << j;
			if (j >= 42 && j <= 47)
			{
				num_re++;
				remember[num_re].x = x;
				remember[num_re].y = y;
			}
			//cv::putText(Image, ss.str(), cv::Point(x, y), 0.5, 0.5, cv::Scalar(0, 0, 255));
			cv::circle(Image, cv::Point(x, y), 2, cv::Scalar(0, 0, 255), -1);
		}
		std::stringstream ss;
		area = Carea.method_area(remember);
		ear = Cear.methond_EAR(remember);
		cout << ear << endl;
		angle = Cangle.cal_ANGLE(remember);
		blink = draw_chart(area,ear,angle); //画坐标和EAR,area点
//		int tot = 0, tot1 = 0;
//		if (Carea.if_blink(area) == 1)//计算眨眼次数并打印
//		{
//			tot++;
//			cout<<"  blink="<<tot<<endl;	
//		}
//		if (Cear.if_blink(ear) == 1)
//		{
//			tot1++;
//		}
//			ss << "  blink=" << tot1;
//			cv::putText(Image, ss.str(), cv::Point(10,10 ), 1, 1, cv::Scalar(0, 0, 255));
//			cv::putText(blink, ss.str(), cv::Point(50, 20), 1, 1, cv::Scalar(0,255 ,0));
//			ss.str("");
//			ss << "  blink=" << tot;
//			cv::putText(Image, ss.str(), cv::Point(10, 10), 1, 1, cv::Scalar(0, 0, 255));
//			cv::putText(blink, ss.str(), cv::Point(50, 200), 1, 1, cv::Scalar(0, 255, 0));
//			ss.str("");
		cv::imshow("Camera", Image);
		cv::imshow("blink", blink);
		if(27 == cv::waitKey(3))
		{
            mCamera.release();
            cv::destroyAllWindows();
            break;
        }
	}

    system("pause");
    return 0;
}

Mat draw_chart(float area,float ear,float angle)
{
	static float AreaRecord[RECOREDSIZE],EarRecord[RECOREDSIZE],AngleRecord[RECOREDSIZE];
	static int record_num = 0;
	int i;
	std::stringstream ss;
	Mat s = Mat::zeros(600, 650, CV_8UC3); //生成全0矩阵600*650
	if (record_num <= RECOREDSIZE)
	{
		AreaRecord[record_num] = area;
		EarRecord[record_num] = ear;
		AngleRecord[record_num] = angle;
		record_num++;
	}//一次记录251个EAR
	else
	{
		for (i = 0; i < RECOREDSIZE - 1; i++)
		{
			AreaRecord[i] = AreaRecord[i + 1];
			EarRecord[i] = EarRecord[i + 1];
			AngleRecord[i] = AngleRecord[i + 1];
		}
		AreaRecord[RECOREDSIZE - 1] = area;
		EarRecord[RECOREDSIZE - 1] = ear;
		AngleRecord[RECOREDSIZE - 1] = angle;
	}//实现图像滚动，record[250]记录新的EAR,area
	line(s, Point(50,10), Point(50, 170), Scalar(255, 255, 255),2);//坐标竖轴ear
	line(s, Point(50, 170), Point(600, 170), Scalar(255, 255, 255), 2);//坐标横轴ear
	line(s, Point(50, 200), Point(50, 360), Scalar(255,255,255), 2);//竖轴area
	line(s, Point(50, 360), Point(600, 360), Scalar(255,255,255), 2);//横轴area
	line(s, Point(50, 390), Point(50, 550), Scalar(255, 255, 255), 2);
	line(s, Point(50, 550), Point(600, 550), Scalar(255, 255, 255), 2);
	line(s, Point(50, 170-150* BLINK_THRESHOLD/MAX_EAR), Point(600, 170 - 150 * BLINK_THRESHOLD / MAX_EAR), Scalar(0, 0, 255), 1);//原点轴ear
	line(s, Point(50, 360 - 150 * AREA_THRESHOLD / MAX_AREA), Point(600, 360 - 150 * AREA_THRESHOLD / MAX_AREA), Scalar(0, 0, 255),1 );//原点轴area
	line(s, Point(50, 550 - 150 * ANGLE_threshold / MAX_ANGLE), Point(600, 550 - 150 * ANGLE_threshold / MAX_ANGLE), Scalar(0, 0, 255), 1);//原点轴angle
	ss << BLINK_THRESHOLD;
	cv::putText(s, ss.str(), cv::Point(10, 170 - 150 * BLINK_THRESHOLD / MAX_EAR), 1, 1, cv::Scalar(0, 255, 0));//竖轴0.3
	cv::putText(s,"0.8", cv::Point(10, 20), 1, 1, cv::Scalar(0, 255, 0));//竖轴0.8
	ss.str("");
	ss << AREA_THRESHOLD;
	cv::putText(s, ss.str(), cv::Point(10, 360 - 150 * BLINK_THRESHOLD / MAX_EAR), 1, 1, cv::Scalar(0, 255, 0));//竖轴240
	cv::putText(s, "800", cv::Point(10, 200), 1, 1, cv::Scalar(0, 255, 0));//竖轴800
	ss.str("");
	ss << ANGLE_threshold;
	cv::putText(s, ss.str(), cv::Point(10, 550 - 150 * ANGLE_threshold / MAX_ANGLE), 1, 1, cv::Scalar(0, 255, 0));//竖轴35
	cv::putText(s, "90", cv::Point(10, 380), 1, 1, cv::Scalar(0, 255, 0));//竖轴90
	for (i = 0; i < RECOREDSIZE; i++)
	{
		circle(s, Point(i*2+51, 170 - 150 * EarRecord[i] / MAX_EAR), 1, Scalar(0, 255, 0));
		circle(s, Point(i * 2 + 51, 360 - 150 * AreaRecord[i] / MAX_AREA), 1, Scalar(0, 255, 0));
		circle(s, Point(i * 2 + 51, 550 - 150 * AngleRecord[i] / MAX_AREA), 1, Scalar(0, 255, 0));
		if (i != RECOREDSIZE - 1)
		{
			line(s, Point(i * 2 + 51, 170 - 150 * EarRecord[i] / MAX_EAR), Point(i * 2 + 51 + 2, 170 - 150 * EarRecord[i + 1] / MAX_EAR), Scalar(0, 255, 0), 1);
			line(s, Point(i * 2 + 51, 360 - 150 * AreaRecord[i] / MAX_AREA), Point(i * 2 + 51 + 2, 360 - 150 * AreaRecord[i + 1] / MAX_AREA), Scalar(0, 255, 0), 1);
			line(s, Point(i * 2 + 51, 550 - 150 * AngleRecord[i] / MAX_ANGLE), Point(i * 2 + 51 + 2, 550 - 150 * AngleRecord[i + 1] / MAX_ANGLE), Scalar(0, 255, 0), 1);
		}
	}
	ss.str("");
	ss << "  ear="<<ear;
	cv::putText(s, ss.str(), cv::Point(200, 20), 1, 1, cv::Scalar(0, 255, 0));
	ss.str("");
	ss << "  area=" << area;
	cv::putText(s, ss.str(), cv::Point(200, 200), 1, 1, cv::Scalar(0, 255, 0));
	ss.str("");
	ss << "  angle=" << angle;
	cv::putText(s, ss.str(), cv::Point(200, 380), 1, 1, cv::Scalar(0, 255, 0));
	return s;
}




















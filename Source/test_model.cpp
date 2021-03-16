#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "include/area.h"
#include "include/Fft1.h"
#include "include/filter.h"
#include "ldmarkmodel.h"

//const int SIZE = 16;
//const int CHANGE = 2;
const int PAINTSIZE = 256; //画图点
using namespace std;
using namespace cv;

//float x, y, w, h;

Mat drawChar(float ear);

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

/****************载入模型****************/
    ldmarkmodel modelt;
	Ear cEar; //申明横纵比的类
	Point remember[13];//记录左右眼的特征点
	Mat programm;//显示图表
	Saver s;
	static int blink;
	float ear = 0;//记录横纵比
	int rem_num;
    std::string modelFilePath = "F:\\testProj\\sdm_test\\test\\roboman-landmark-model.bin";
    while(!load_ldmarkmodel(modelFilePath, modelt)){
        std::cout << "文件打开错误，请重新输入文件路径." << std::endl;
        std::cin >> modelFilePath;
    }

/*******************打开摄像头*******************/
    cv::VideoCapture mCamera("xiang_move.mp4");

 /*   if(!mCamera.isOpened()){
        std::cout << "Camera opening failed..." << std::endl;
        system("pause");
        return 0;
    }*/
    cv::Mat Image;
	cv::Mat current_shape;
    for(;;){
        mCamera >> Image;//摄像头获取的图像mCamere载入Image

		transpose(Image, Image);
		flip(Image, Image, 0);//逆时针旋转90度

        modelt.track(Image, current_shape);
        cv::Vec3d eav;
        modelt.EstimateHeadPose(current_shape, eav);
        modelt.drawPose(Image, current_shape, 50);
        int numLandmarks = current_shape.cols/2;
		rem_num = 0;
        for(int j=0; j<numLandmarks; j++)
		{
            int x = current_shape.at<float>(j);
            int y = current_shape.at<float>(j + numLandmarks);
            std::stringstream ss;
            ss << j;   //j是特征点序号
			if (j >= 36 && j <= 47)
			{
				rem_num++;
				remember[rem_num].x = x;
				remember[rem_num].y = y;
			}
		if (j >= 36 && j <= 68) //36-41左眼，42-47右眼，48-67嘴巴
			{
//				cv::putText(Image, ss.str(), cv::Point(x, y), 0.5, 0.5, cv::Scalar(0, 0, 255));//打特征点序号
				cv::circle(Image, cv::Point(x, y), 2, cv::Scalar(0, 0, 255), -1);//打特征点
			}
        }
//		rectangle(Image, Rect(x, y, w, h), Scalar(0, 255, 0), 2);
		ear = cEar.methond_EAR(remember);
		if (s.freBlink(ear) == 1)
		{
			blink++;
			cout << "blink: " << blink << endl;
		}
		cv::imshow("char", drawChar(ear));
        cv::imshow("Camera", Image);
        if(27 == cv::waitKey(5)){
            mCamera.release();
            cv::destroyAllWindows();
            break;
        }
    }

    system("pause");
    return 0;
}

Mat drawChar(float ear)
{
	Mat scr = Mat::zeros(600, 1000, CV_8UC3);
//	Saver f; //FFT变换类
//	float Fear = f.filterFunc(ear); //FFT变换后的特征值
//	static float record[PAINTSIZE] = { 0 }; //储存显示画图点数
//	static int firstNum = 0; //一次记录256个点

//	if (firstNum < PAINTSIZE)
//	{
//		record[firstNum] = Fear;
//		firstNum++;
//	}
//	else
//	{
//		for (int i = 0; i < PAINTSIZE - 1; i++)
//		{
//			record[i] = record[i + 1];
//		}
//		record[PAINTSIZE - 1] = Fear;
//	}
	Saver f;
	float Sear = f.SaFunc(ear);
	static CComplex inVec[SIZE] = { 0 };
	CComplex outVec[SIZE] = { 0 };
	float finRecord[SIZE] = { 0 };
	static int i = 0;
	CFft1 t;
	int j,paintJ;
	
	if (i < SIZE)//第一组SIZE取满，全部取值不画图片
	{
		inVec[i].rl = Sear;
		i++;
		return scr;
	}
	else//当第一组取满
	{
		t.fft(inVec, SIZE, outVec);//FFT算法实现

		for (j = 0; j < SIZE; j++)//将得到的复数转化为模
		{
			finRecord[j] = sqrt(pow(outVec[j].rl, 2) + pow(outVec[j].im, 2));
		}

		for (j = 0; j < SIZE - 1; j++)
		{
			inVec[j].rl = inVec[j + 1].rl;
		}
		inVec[SIZE - 1].rl = Sear;
	}

	line(scr, Point(50, 50), Point(50, 550), Scalar(255, 255, 255), 2);
	line(scr, Point(50, 550), Point(900, 550), Scalar(255, 255, 255), 2);
	for (j = 1,paintJ = 0; j < SIZE; j++)
	{
		line(scr, Point(paintJ+100, 550), Point(paintJ+100, 550 - finRecord[j] * 500), Scalar(0, 255, 0), 1);
		if(j != SIZE - 1)//直流分量不取
		line(scr, Point(paintJ + 100, 550 - finRecord[j] * 500), Point(paintJ + 150, 550 - finRecord[j + 1] * 500), Scalar(0, 0, 255), 2);
		paintJ += 50;
	}
	cout << (finRecord[1] + finRecord[SIZE - 1]) / 2 - finRecord[SIZE/2]<<endl;
//	line(scr, Point(50, 10), Point(50, 170), Scalar(255, 255, 255), 2);//坐标竖轴ear
//	line(scr, Point(50, 170), Point(600, 170), Scalar(255, 255, 255), 2);//坐标横轴ear
//	line(scr, Point(50, 170 - 150 * 0.3 / 0.8), Point(600, 170 - 150 * 0.3 / 0.8), Scalar(0, 0, 255), 1);//原点轴ear
//	for (int i = 0; i < PAINTSIZE; i++)
//	{
//		if (i != PAINTSIZE - 1)
//		{
//			line(scr, Point(i * 2 + 50, 170 - 150 * record[i] / 10), Point(i * 2 + 50 + 2, 170 - 150 * record[i+1] / 10), Scalar(0, 255, 0), 1);
//		}
//	}
	return scr;
}
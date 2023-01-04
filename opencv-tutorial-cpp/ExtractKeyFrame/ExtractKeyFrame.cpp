#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//CV_BGR2GRAY未声明的标识符的解决办法
#include <opencv2\imgproc\types_c.h>

#include "Timer.h"

using namespace std;
using namespace cv;

// 根据情况调节阈值
#define Threshold_Value 0.4

#define VideoPath "../Resource/xgplayer-demo-720p.mp4"
#define OutputImgPath "../x64/Debug/keyframes/"

int extractKeyFrame()
{
	long currentFrame = 1;
	float p;
	VideoCapture cap;
	//这里放置需要提取关键字的视频
	cap.open(VideoPath);

	if (!cap.isOpened())//如果视频不能正常打开则返回
	{
		// 如果没有将opencv_videoio_ffmpeg430_64.dll放到exe同级目录，则打开VideoCapture失败
		cout << "cannot open video!" << endl;
		return 0;
	}
	Mat frame_key;
	bool b = cap.read(frame_key);

	if (frame_key.empty())
		cout << "frame_key is empty!" << endl;

	Mat grayKeyFrame;
	cvtColor(frame_key, grayKeyFrame, CV_BGR2GRAY);	// 19ms

	stringstream str;
	str << OutputImgPath << currentFrame << ".jpg";
	cout << str.str() << endl;
	b = imwrite(str.str(), frame_key);
	Mat frame;
	Mat previousImage, currentImage, resultImage;

	Timer t;
	while (1)
	{
		Mat frame;
		cap >> frame;
		if (frame.empty())
		{
			cout << "frame is empty!" << endl;
			break;
		}

		Mat srcImage_base = frame_key;
		Mat srcImage_test1 = frame;

		Timer t1;

		//将图像从BGR色彩空间转换到 HSV色彩空间
		Timer t11;
		Timer t112;
		cvtColor(srcImage_test1, currentImage, CV_BGR2GRAY);	// 19ms
		//cout << "t112: " << t112.delta<Timer::ms>() << endl;
		//cout << "t11: " << t11.delta<Timer::ms>() << endl;

		Timer t12;
		absdiff(currentImage, grayKeyFrame, resultImage);  //帧差法，相减	8ms
		//cout << "t12: " << t12.delta<Timer::ms>() << endl;

		Timer t13;
		threshold(resultImage, resultImage, 10, 255.0, CV_THRESH_BINARY); //二值化，像素值相差大于20则置为255，其余为0	2ms
		//cout << "t13: " << t13.delta<Timer::ms>() << endl;

		//cout << "t1: " << t1.delta<Timer::ms>() << endl; // 51ms
		Timer t2;

		float counter = 0;
		float num = 0;
		// 统计两帧相减后图像素
		for (int i = 0; i < resultImage.rows; i++)
		{
			uchar *data = resultImage.ptr<uchar>(i); //获取每一行的指针
			for (int j = 0; j < resultImage.cols; j++)
			{
				num = num + 1;
				if (data[j] == 255) //访问到像素值
				{
					counter = counter + 1;
				}
			}
		}
		//cout << "t2: " << t2.delta<Timer::ms>() << endl; // 5ms

		// counter  num  p 分别为  有变化的像素点数  总像素点数目  比例
		p = counter / num;
		//printf(">>>>>>counter>>>>num>>>>p: %f  %f  %f  \n", counter, num, p);
		if (p > Threshold_Value) //达到阈值的像素数达到一定的数量则保存该图像
		{
			frame_key = frame;

			cvtColor(frame_key, grayKeyFrame, CV_BGR2GRAY);	// 19ms

			++currentFrame;
			stringstream str;
			str << OutputImgPath << currentFrame << ".jpg";
			b = imwrite(str.str(), frame_key);

			imshow("显示关键帧", frame);
			waitKey(20);
		}
		else
		{
			cout << "p = " << p << endl;
		}

	}
	cout << "duration: " << t.stop_delta<Timer::ms>() << "ms" << endl;
	return 0;
}
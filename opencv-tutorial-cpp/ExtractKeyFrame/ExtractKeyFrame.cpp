#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//CV_BGR2GRAYδ�����ı�ʶ���Ľ���취
#include <opencv2\imgproc\types_c.h>

#include "Timer.h"

using namespace std;
using namespace cv;

// �������������ֵ
#define Threshold_Value 0.4

#define VideoPath "../Resource/xgplayer-demo-720p.mp4"
#define OutputImgPath "../x64/Debug/keyframes/"

int extractKeyFrame()
{
	long currentFrame = 1;
	float p;
	VideoCapture cap;
	//���������Ҫ��ȡ�ؼ��ֵ���Ƶ
	cap.open(VideoPath);

	if (!cap.isOpened())//�����Ƶ�����������򷵻�
	{
		// ���û�н�opencv_videoio_ffmpeg430_64.dll�ŵ�exeͬ��Ŀ¼�����VideoCaptureʧ��
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

		//��ͼ���BGRɫ�ʿռ�ת���� HSVɫ�ʿռ�
		Timer t11;
		Timer t112;
		cvtColor(srcImage_test1, currentImage, CV_BGR2GRAY);	// 19ms
		//cout << "t112: " << t112.delta<Timer::ms>() << endl;
		//cout << "t11: " << t11.delta<Timer::ms>() << endl;

		Timer t12;
		absdiff(currentImage, grayKeyFrame, resultImage);  //֡������	8ms
		//cout << "t12: " << t12.delta<Timer::ms>() << endl;

		Timer t13;
		threshold(resultImage, resultImage, 10, 255.0, CV_THRESH_BINARY); //��ֵ��������ֵ������20����Ϊ255������Ϊ0	2ms
		//cout << "t13: " << t13.delta<Timer::ms>() << endl;

		//cout << "t1: " << t1.delta<Timer::ms>() << endl; // 51ms
		Timer t2;

		float counter = 0;
		float num = 0;
		// ͳ����֡�����ͼ����
		for (int i = 0; i < resultImage.rows; i++)
		{
			uchar *data = resultImage.ptr<uchar>(i); //��ȡÿһ�е�ָ��
			for (int j = 0; j < resultImage.cols; j++)
			{
				num = num + 1;
				if (data[j] == 255) //���ʵ�����ֵ
				{
					counter = counter + 1;
				}
			}
		}
		//cout << "t2: " << t2.delta<Timer::ms>() << endl; // 5ms

		// counter  num  p �ֱ�Ϊ  �б仯�����ص���  �����ص���Ŀ  ����
		p = counter / num;
		//printf(">>>>>>counter>>>>num>>>>p: %f  %f  %f  \n", counter, num, p);
		if (p > Threshold_Value) //�ﵽ��ֵ���������ﵽһ���������򱣴��ͼ��
		{
			frame_key = frame;

			cvtColor(frame_key, grayKeyFrame, CV_BGR2GRAY);	// 19ms

			++currentFrame;
			stringstream str;
			str << OutputImgPath << currentFrame << ".jpg";
			b = imwrite(str.str(), frame_key);

			imshow("��ʾ�ؼ�֡", frame);
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
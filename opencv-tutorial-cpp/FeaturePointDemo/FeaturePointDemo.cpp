#include "Timer.h"

#include <iostream>
#include <math.h>

//CV_BGR2GRAY未声明的标识符的解决办法
#include <opencv2/imgproc/types_c.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgcodecs/legacy/constants_c.h>
#include <opencv2/core/base.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;


#define PhysicsStepNum			14
#define PhysicsKeyFrameNum		31
//#define Physics_Key_Frame_

#define ChemistryStepNum		17
#define ChemistryKeyFrameNum	38

//	Threshold for the keypoint detector.Only features, whose hessian is larger than hessianThreshold are retained by the detector.
//	Therefore, the larger the value, the less keypoints you will get.A good default value could be from 300 to 500, depending from the image contrast.
#define SurfMinHessian	400



//Mat mergeRows(Mat A, Mat B)
//{
//	//CV_ASSERT(A.cols == B.cols&&A.type() == B.type());
//	assert(A.cols == B.cols && A.type() == B.type());
//	int totalRows = A.rows + B.rows;
//
//	Mat mergedDescriptors(totalRows, A.cols, A.type());
//	Mat submat = mergedDescriptors.rowRange(0, A.rows);
//	A.copyTo(submat);
//	submat = mergedDescriptors.rowRange(A.rows, totalRows);
//	B.copyTo(submat);
//	return mergedDescriptors;
//}

//Mat mergeCols(Mat A, Mat B)
//{
//	//CV_ASSERT(A.rows == B.rows && A.type() == B.type());
//	//assert(A.rows == B.rows && A.type() == B.type());
//	int totalCols = A.cols + B.cols;
//
//	Mat mergedDescriptors(A.rows, totalCols, A.type());
//	Mat submat = mergedDescriptors.colRange(0, A.cols);
//	A.copyTo(submat);
//	submat = mergedDescriptors.colRange(A.cols, totalCols);
//	B.copyTo(submat);
//	return mergedDescriptors;
//}

Mat mergeCols(Mat img_left, Mat img_right)
{
	Size size(img_left.cols + img_right.cols, MAX(img_left.rows, img_right.rows));

	Mat img_merge;
	Mat outImg_left, outImg_right;

	img_merge.create(size, CV_MAKETYPE(img_left.depth(), 3));
	img_merge = Scalar::all(0);
	outImg_left = img_merge(Rect(0, 0, img_left.cols, img_left.rows));
	outImg_right = img_merge(Rect(img_left.cols, 0, img_right.cols, img_right.rows));

	if (img_left.type() == CV_8U)
	{
		cvtColor(img_left, outImg_left, CV_GRAY2BGR);
	}
	else
	{
		img_left.copyTo(outImg_left);
	}

	if (img_right.type() == CV_8U)
	{
		cvtColor(img_right, outImg_right, CV_GRAY2BGR);
	}
	else
	{
		img_right.copyTo(outImg_right);
	}
	return img_merge;
}

//绘制特征点
int extractFeatureDemo1() 
{
	//Mat src = imread("../Resource/物理实验关键步骤图片/测量固体体重-物理实验_1.jpg", IMREAD_GRAYSCALE);
	Mat src = imread("../Resource/关键步骤图片/fp2.jpg", IMREAD_COLOR);
	if (src.empty()) 
	{
		printf("could not load image...\n");
		return -1;
	}
	//namedWindow("input image", CV_WINDOW_AUTOSIZE);
	imshow("输入图片", src);

	// SURF特征点检测
	int minHessian = 100;
	Ptr<SURF> detector = SURF::create(minHessian);//创建一个surf类对象并初始化
	vector<KeyPoint> keypoints;
	detector->detect(src, keypoints, Mat());//找出关键点

	// 绘制关键点
	Mat keypoint_img;
	drawKeypoints(src, keypoints, keypoint_img, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	imshow("绘制特征点后的图片", keypoint_img);

	waitKey(0);
	//system("pause");
	return 0;
}

int extractFeatureDemo2()
{
	Timer t;
	vector<Mat> stepMats;
	vector<Mat> keyFrameMats;
	for (int i = 0; i < PhysicsStepNum; ++i)
	{
		string imageDir = "../Resource/关键步骤图片/物理/";
		string path = imageDir + "测量固体体重-物理实验_" + to_string(i + 1) + ".jpg";

		//Timer t;
		Mat src = imread(path, IMREAD_COLOR);
		//cout << "imread duration: " << t.delta<Timer::ms>() << "ms" << endl;    //52ms
		if (src.empty())
		{
			printf("could not load image...\n");
			return -1;
		}
		stepMats.emplace_back(src);
	}
	for (int j = 0; j < PhysicsKeyFrameNum; ++j)
	{
		string imageDir = "../Resource/关键帧/物理/";
		string path = imageDir + to_string(j + 1) + ".jpg";

		Mat right = imread(path, IMREAD_COLOR);
		if (right.empty())
		{
			printf("could not load key frame image...\n");
			return -1;
		}
		keyFrameMats.emplace_back(right);
	}

	string outPath;
	for (int i = 0; i < PhysicsStepNum; ++i)
	{
		// SURF特征点检测
		Ptr<SURF> detector = SURF::create(SurfMinHessian);//创建一个surf类对象并初始化
		vector<KeyPoint> keypoints;
		detector->detect(stepMats[i], keypoints, Mat());//找出关键点

		// 绘制关键点
		Mat keypoint_img_left;
		drawKeypoints(stepMats[i], keypoints, keypoint_img_left, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

		for (int j = 0; j < PhysicsKeyFrameNum; ++j)
		{
			// SURF特征点检测
			Ptr<SURF> detector_right = SURF::create(SurfMinHessian);//创建一个surf类对象并初始化
			vector<KeyPoint> keypoints_right;
			detector_right->detect(keyFrameMats[j], keypoints_right, Mat());//找出关键点

			// 绘制关键点
			Mat keypoint_img_right;
			drawKeypoints(keyFrameMats[j], keypoints_right, keypoint_img_right, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

			//合并两张图片
			Mat mergeMat = mergeCols(keypoint_img_left, keypoint_img_right);

			outPath = "../Resource/特征图/物理/";
			outPath += to_string(i + 1) + "_" + to_string(j + 1) + ".jpg";
			bool b = imwrite(outPath, mergeMat);
		}

	}

	cout << "extractFeatureDemo2 time: " << t.delta<Timer::s>() << "s" << endl;
	return 0;
}

int extractFeatureDemo3()
{
	Timer t;
    string outPath;
    vector<Mat> stepMats;
    vector<Mat> keyFrameMats;
	vector<Mat> lMats;
	vector<Mat> rMats;

    for (int i = 0; i < ChemistryStepNum; ++i)
    {
        string imageDir = "../Resource/关键步骤图片/化学/";
        string path = imageDir + "探究金属的某些化学性质-化学实验_" + to_string(i + 1) + ".jpg";

        //Timer t;
        Mat src = imread(path, IMREAD_COLOR);
        //cout << "imread duration: " << t.delta<Timer::ms>() << "ms" << endl;    //52ms
        if (src.empty())
        {
			printf("could not load image...\n");
			return -1;
		}
		//缩小到720x404
		resize(src, src, Size(720, 404));

		stepMats.emplace_back(src);

		// SURF特征点检测
		Ptr<SURF> detector = SURF::create(SurfMinHessian);//创建一个surf类对象并初始化
		vector<KeyPoint> keypoints;
		detector->detect(stepMats[i], keypoints, Mat());//找出关键点

		// 绘制关键点
		Mat keypoint_img_left;
		drawKeypoints(stepMats[i], keypoints, keypoint_img_left, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
		lMats.emplace_back(keypoint_img_left);

    }
    for (int j = 0; j < ChemistryKeyFrameNum; ++j)
    {
        string imageDir = "../Resource/关键帧/化学/";
        string path = imageDir + to_string(j + 1) + ".jpg";

        Mat right = imread(path, IMREAD_COLOR);
        if (right.empty())
        {
            printf("could not load key frame image...\n");
            return -1;
        }
		resize(right, right, Size(720, 404));

        keyFrameMats.emplace_back(right);

		// SURF特征点检测
		Ptr<SURF> detector_right = SURF::create(SurfMinHessian);//创建一个surf类对象并初始化
		vector<KeyPoint> keypoints_right;
		Timer t2;
		detector_right->detect(keyFrameMats[j], keypoints_right, Mat());//找出关键点
		//cout << "detect duration: " << t2.delta<Timer::ms>() << "ms" << endl;   //802ms

		// 绘制关键点
		Mat keypoint_img_right;
		Timer t3;
		drawKeypoints(keyFrameMats[j], keypoints_right, keypoint_img_right, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
		//cout << "drawKeypoints duration: " << t3.delta<Timer::ms>() << "ms" << endl;    //99ms
		rMats.emplace_back(keypoint_img_right);
    }

    for (int i = 0; i < ChemistryStepNum; ++i)
    {
		Mat left = lMats[i];
        for (int j = 0; j < ChemistryKeyFrameNum; ++j)
        {
			Mat right = rMats[j];

            //合并两张图片
			Mat mergeMat = mergeCols(left, right);
            outPath = "../Resource/特征图/化学/";
            outPath += to_string(i + 1) + "_" + to_string(j + 1) + ".jpg";
            bool b = imwrite(outPath, mergeMat);
        }

    }
	cout << "extractFeatureDemo3 time: " << t.delta<Timer::s>() << "s" << endl;
    return 0;
}

//暴力匹配
int bfMatch()
{

	Mat src = imread("../Resource/物理实验关键步骤图片/测量固体体重-物理实验_1.jpg");
	Mat temp = imread("../Resource/物理实验关键步骤图片/测量固体体重-物理实验_2.jpg");
	if (src.empty() || temp.empty()) {
		printf("could not load image...\n");
		return -1;
	}
	namedWindow("input image", CV_WINDOW_AUTOSIZE);
	imshow("input image", src);

	// SURF特征点检测
	int minHessian = 400;
	Ptr<SURF> detector = SURF::create(minHessian, 4, 3, true, true);//创建一个surf类检测器对象并初始化
	vector<KeyPoint> keypoints1, keypoints2;
	Mat src_vector, temp_vector;//用来存放特征点的描述向量

	//detector->detect(src, keypoints1, Mat());//找出关键点
	//detector->detect(temp, keypoints2, Mat());//找出关键点

	//找到特征点并计算特征描述子(向量)
	detector->detectAndCompute(src, Mat(), keypoints1, src_vector);//输入图像，输入掩码，输入特征点，输出Mat，存放所有特征点的描述向量
	detector->detectAndCompute(temp, Mat(), keypoints2, temp_vector);//这个Mat行数为特征点的个数，列数为每个特征向量的尺寸，SURF是64（维）


	//匹配
	BFMatcher matcher(NORM_L2);         //实例化一个暴力匹配器(括号里可以选择匹配方法)

	vector<DMatch> matches;    //DMatch是用来描述匹配好的一对特征点的类，包含这两个点之间的匹配信息
							   //比如左图有个特征m，它和右图的特征点n最匹配，这个DMatch就记录它俩最匹配，并且还记录m和n的
							   //特征向量的距离和其他信息，这个距离在后面用来做筛选    

	matcher.match(src_vector, temp_vector, matches);             //匹配，数据来源是特征向量，结果存放在DMatch类型里面  

	//匹配点筛选
	//sort函数对数据进行升序排列
	//筛选匹配点，根据match里面特征对的距离从小到大排序    
	//筛选出最优的30个匹配点（可以不使用，会画出所有特征点）

	sort(matches.begin(), matches.end());
	vector< DMatch > good_matches;
	int ptsPairs = std::min(30, (int)(matches.size() * 0.15));//匹配点数量不大于50
	//int ptsPairs = 2;
	//cout << ptsPairs << endl;
	for (int i = 0; i < ptsPairs; i++)
	{
		good_matches.push_back(matches[i]);//距离最小的50个压入新的DMatch
	}

    for (size_t i = 0; i < good_matches.size(); ++i)
    {
        cout << "BF匹配，第" << i + 1 << "个匹配对欧式距离：" << good_matches[i].distance << endl;
    }

	Mat MatchesImage;                                //drawMatches这个函数直接画出摆在一起的图
	drawMatches(src, keypoints1, temp, keypoints2, good_matches, MatchesImage, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);  //绘制匹配点  
	imshow("BFMatcher Image", MatchesImage);

	waitKey(0);
	return 0;

}

//FLANN匹配
int flannMatch()
{

	//Mat src = imread("../Resource/物理实验关键步骤图片/测量固体体重-物理实验_1.jpg", IMREAD_GRAYSCALE);
	Mat src = imread("../Resource/物理实验关键步骤图片/测量固体体重-物理实验_1.jpg", IMREAD_COLOR);
	Mat temp = imread("../Resource/物理实验关键步骤图片/测量固体体重-物理实验_2.jpg", IMREAD_COLOR);
	if (src.empty() || temp.empty()) {
		printf("could not load image...\n");
		return -1;
	}
	namedWindow("input image", CV_WINDOW_AUTOSIZE);
	imshow("input image", src);

	// SURF特征点检测
	int minHessian = 400;
	/*
	Threshold for the keypoint detector.Only features, whose hessian is larger than hessianThreshold are retained by the detector.
	Therefore, the larger the value, the less keypoints you will get.A good default value could be from 300 to 500, depending from the image contrast.
	*/
	Ptr<SURF> detector = SURF::create(minHessian, 4, 3, true, true);//创建一个surf类检测器对象并初始化
	vector<KeyPoint> keypoints1, keypoints2;
	Mat src_vector, temp_vector;//用来存放特征点的描述向量

	//detector->detect(src, keypoints1, Mat());//找出关键点
	//detector->detect(temp, keypoints2, Mat());//找出关键点

	//找到特征点并计算特征描述子(向量)
	detector->detectAndCompute(src, Mat(), keypoints1, src_vector);//输入图像，输入掩码，输入特征点，输出Mat，存放所有特征点的描述向量
	detector->detectAndCompute(temp, Mat(), keypoints2, temp_vector);//这个Mat行数为特征点的个数，列数为每个特征向量的尺寸，SURF是64（维）


	//匹配
	FlannBasedMatcher matcher;         //实例化一个FLANN匹配器(括号里可以选择匹配方法)

	vector<DMatch> matches;    //DMatch是用来描述匹配好的一对特征点的类，包含这两个点之间的匹配信息
							   //比如左图有个特征m，它和右图的特征点n最匹配，这个DMatch就记录它俩最匹配，并且还记录m和n的
							   //特征向量的距离和其他信息，这个距离在后面用来做筛选    

	matcher.match(src_vector, temp_vector, matches);             //匹配，数据来源是特征向量，结果存放在DMatch类型里面  

	//求最小最大距离
	double minDistance = 1000;//反向逼近
	double maxDistance = 0;
	for (int i = 0; i < src_vector.rows; i++) {
		double distance = matches[i].distance;
		if (distance > maxDistance) {
			maxDistance = distance;
		}
		if (distance < minDistance) {
			minDistance = distance;
		}
	}
	printf("max distance : %f\n", maxDistance);
	printf("min distance : %f\n", minDistance);

	//筛选较好的匹配点
	//vector< DMatch > good_matches;
	//for (int i = 0; i < src_vector.rows; i++) {
	//	double distance = matches[i].distance;
	//	if (distance < max(minDistance * 2, 0.02)) {
	//		good_matches.push_back(matches[i]);//距离小于范围的压入新的DMatch
	//	}
	//}

	//good_matches = matches;

	//sort函数对数据进行升序排列
	//筛选匹配点，根据match里面特征对的距离从小到大排序
	//筛选出最优的50个匹配点（可以不使用，会画出所有特征点）

	sort(matches.begin(), matches.end());
	vector< DMatch > good_matches;
	int ptsPairs = std::min(50, (int)(matches.size() * 0.15));//匹配点数量不大于50
	cout << ptsPairs << endl;
	for (int i = 0; i < ptsPairs; i++)
	{
		good_matches.push_back(matches[i]);//距离最小的50个压入新的DMatch
	}
	

	Mat MatchesImage;                                //drawMatches这个函数直接画出摆在一起的图
	drawMatches(src, keypoints1, temp, keypoints2, good_matches, MatchesImage, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);  //绘制匹配点  
	imshow("FLANN Image", MatchesImage);
	imwrite("../Resource/tt.jpg", MatchesImage);

	waitKey(0);
	return 0;
}

int flannMatch_p()
{
	Timer tt;
	// SURF特征点检测
	Ptr<SURF> detector = SURF::create(SurfMinHessian, 4, 3, true, true);//创建一个surf类检测器对象并初始化

	vector<Mat> stepMats;
	vector<Mat> keyFrameMats;
	vector<vector<KeyPoint>> lKeyPointsVec;
	vector<vector<KeyPoint>> rKeyPointsVecs;
	vector<Mat> lMats;
	vector<Mat> rMats;
	for (int i = 0; i < PhysicsStepNum; ++i)
	{
		string stepPath = "../Resource/关键步骤图片/物理/测量固体体重-物理实验_";
		stepPath += to_string(i + 1) + ".jpg";
		Mat src = imread(stepPath, IMREAD_COLOR);
		if (src.empty()) {
			printf("could not load image...\n");
			return -1;
		}
		stepMats.emplace_back(src);

		vector<KeyPoint> keypoints1;
		Mat src_vector;//用来存放特征点的描述向量
		//找到特征点并计算特征描述子(向量)
		detector->detectAndCompute(stepMats[i], Mat(), keypoints1, src_vector);//输入图像，输入掩码，输入特征点，输出Mat，存放所有特征点的描述向量
		lKeyPointsVec.emplace_back(keypoints1);
		lMats.emplace_back(src_vector);
	}
	for (int i = 0; i < PhysicsKeyFrameNum; ++i)
	{
		string keyFramePath = "../Resource/关键帧/物理/";
		keyFramePath += to_string(i + 1) + ".jpg";
		Mat dst = imread(keyFramePath, IMREAD_COLOR);
		if (dst.empty()) {
			printf("could not load image...\n");
			return -1;
		}
		keyFrameMats.emplace_back(dst);

		vector<KeyPoint> keypoints2;
		Mat temp_vector;//用来存放特征点的描述向量
		//Timer t;
		detector->detectAndCompute(keyFrameMats[i], Mat(), keypoints2, temp_vector);//这个Mat行数为特征点的个数，列数为每个特征向量的尺寸，SURF是64（维）
		//cout << "detectAndCompute duration: " << t.delta<Timer::ms>() << "ms" << endl;  //1083ms
		rKeyPointsVecs.emplace_back(keypoints2);
		rMats.emplace_back(temp_vector);

	}
	assert(stepMats.size() == PhysicsStepNum && keyFrameMats.size() == PhysicsKeyFrameNum);

	for (int i = 0; i < PhysicsStepNum; ++i)
	{
		Mat src_vector = lMats[i];
		vector<KeyPoint> keypoints1 = lKeyPointsVec[i];
		for (int j = 0; j < PhysicsKeyFrameNum; ++j)
		{
			Timer t4;
			Mat temp_vector = rMats[j];
			vector<KeyPoint> keypoints2 = rKeyPointsVecs[j];

			FlannBasedMatcher matcher;         //实例化一个FLANN匹配器(括号里可以选择匹配方法)
			vector<DMatch> matches;    //DMatch是用来描述匹配好的一对特征点的类，包含这两个点之间的匹配信息
			//Timer t2;
			matcher.match(src_vector, temp_vector, matches);             //匹配，数据来源是特征向量，结果存放在DMatch类型里面  
			//cout << "match duration: " << t2.delta<Timer::ms>() << "ms" << endl;    //692ms

			Mat MatchesImage;                                //drawMatches这个函数直接画出摆在一起的图
			//Timer t3;
			drawMatches(stepMats[i], keypoints1, keyFrameMats[j], keypoints2, matches, MatchesImage, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);  //绘制匹配点  
			//cout << "drawMatches duration: " << t3.delta<Timer::ms>() << "ms" << endl;  //389ms
			string outPath = "../Resource/特征点连线图/物理/";
			outPath += to_string(i + 1) + "_" + to_string(j + 1) + ".jpg";
			imwrite(outPath, MatchesImage);
			cout << "化学画一张连线图耗时：" << t4.delta<Timer::ms>() << "毫秒" << endl;
		}
	}

	cout << "flannMatch_p time: " << tt.delta<Timer::s>() << "s" << endl;
	return 0;
}


int flannMatch_c()
{
	Timer tt;
	// SURF特征点检测
    Ptr<SURF> detector = SURF::create(SurfMinHessian, 4, 3, true, true);//创建一个surf类检测器对象并初始化

    vector<Mat> stepMats;
    vector<Mat> keyFrameMats;
	vector<vector<KeyPoint>> lKeyPointsVec;
	vector<vector<KeyPoint>> rKeyPointsVecs;
	vector<Mat> lMats;
	vector<Mat> rMats;
    for (int i = 0; i < ChemistryStepNum; ++i)
    {
        string stepPath = "../Resource/关键步骤图片/化学/探究金属的某些化学性质-化学实验_";
        stepPath += to_string(i + 1) + ".jpg";
        Mat src = imread(stepPath, IMREAD_COLOR);
        if (src.empty() ) {
            printf("could not load image...\n");
            return -1;
        }
		resize(src, src, Size(720, 404));

        stepMats.emplace_back(src);

		vector<KeyPoint> keypoints1;
		Mat src_vector;//用来存放特征点的描述向量
		//找到特征点并计算特征描述子(向量)
		detector->detectAndCompute(stepMats[i], Mat(), keypoints1, src_vector);//输入图像，输入掩码，输入特征点，输出Mat，存放所有特征点的描述向量
		lKeyPointsVec.emplace_back(keypoints1);
		lMats.emplace_back(src_vector);
    }
    for (int i = 0; i < ChemistryKeyFrameNum; ++i)
    {
        string keyFramePath = "../Resource/关键帧/化学/";
        keyFramePath += to_string(i + 1) + ".jpg";
        Mat dst = imread(keyFramePath, IMREAD_COLOR);
        if (dst.empty()) {
            printf("could not load image...\n");
            return -1;
        }
		resize(dst, dst, Size(720, 404));

        keyFrameMats.emplace_back(dst);

		vector<KeyPoint> keypoints2;
		Mat temp_vector;//用来存放特征点的描述向量
		Timer t;
		detector->detectAndCompute(keyFrameMats[i], Mat(), keypoints2, temp_vector);//这个Mat行数为特征点的个数，列数为每个特征向量的尺寸，SURF是64（维）
		cout << "detectAndCompute duration: " << t.delta<Timer::ms>() << "ms" << endl;	//500ms
		rKeyPointsVecs.emplace_back(keypoints2);
		rMats.emplace_back(temp_vector);
    }
    assert(stepMats.size() == ChemistryStepNum && keyFrameMats.size() == ChemistryKeyFrameNum);

    for (int i = 0; i < ChemistryStepNum; ++i)
    {
		Mat src_vector = lMats[i];
		vector<KeyPoint> keypoints1 = lKeyPointsVec[i];
        for (int j = 0; j < ChemistryKeyFrameNum; ++j)
        {
			Timer t4;
			Mat temp_vector = rMats[j];
			vector<KeyPoint> keypoints2 = rKeyPointsVecs[j];

            FlannBasedMatcher matcher;         //实例化一个FLANN匹配器(括号里可以选择匹配方法)
            vector<DMatch> matches;    //DMatch是用来描述匹配好的一对特征点的类，包含这两个点之间的匹配信息
            //Timer t2;
            matcher.match(src_vector, temp_vector, matches);             //匹配，数据来源是特征向量，结果存放在DMatch类型里面  
            //cout << "match duration: " << t2.delta<Timer::ms>() << "ms" << endl;	//385ms

            Mat MatchesImage;                                //drawMatches这个函数直接画出摆在一起的图
            //Timer t3;
            drawMatches(stepMats[i], keypoints1, keyFrameMats[j], keypoints2, matches, MatchesImage, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);  //绘制匹配点  
            //cout << "drawMatches duration: " << t3.delta<Timer::ms>() << "ms" << endl;	//211ms
            string outPath = "../Resource/特征点连线图/化学/";
            outPath += to_string(i + 1) + "_" + to_string(j + 1) + ".jpg";
            imwrite(outPath, MatchesImage);
			//cout << "化学画一张连线图耗时：" << t4.delta<Timer::ms>() << "毫秒" << endl;	//338ms
        }
    }
	cout << "flannMatch_c time: " << tt.delta<Timer::s>() << "s" << endl;
	return 0;
}

int objectSearch()
{
	Mat src = imread("67.jpg");
	Mat temp = imread("73.jpg");
	if (src.empty() || temp.empty()) {
		printf("could not load image...\n");
		return -1;
	}
	namedWindow("input image", CV_WINDOW_AUTOSIZE);
	imshow("input image", src);

	// SURF特征点检测
	int minHessian = 400;
	Ptr<SURF> detector = SURF::create(minHessian, 4, 3, true, true);//创建一个surf类检测器对象并初始化
	vector<KeyPoint> keypoints1, keypoints2;
	Mat src_vector, temp_vector;//用来存放特征点的描述向量

	//detector->detect(src, keypoints1, Mat());//找出关键点
	//detector->detect(temp, keypoints2, Mat());//找出关键点

	//找到特征点并计算特征描述子(向量)
	detector->detectAndCompute(src, Mat(), keypoints1, src_vector);//输入图像，输入掩码，输入特征点，输出Mat，存放所有特征点的描述向量
	detector->detectAndCompute(temp, Mat(), keypoints2, temp_vector);//这个Mat行数为特征点的个数，列数为每个特征向量的尺寸，SURF是64（维）


	//匹配
	FlannBasedMatcher matcher;         //实例化一个FLANN匹配器(括号里可以选择匹配方法)

	vector<DMatch> matches;    //DMatch是用来描述匹配好的一对特征点的类，包含这两个点之间的匹配信息
							   //比如左图有个特征m，它和右图的特征点n最匹配，这个DMatch就记录它俩最匹配，并且还记录m和n的
							   //特征向量的距离和其他信息，这个距离在后面用来做筛选    

	matcher.match(src_vector, temp_vector, matches);             //匹配，数据来源是特征向量，结果存放在DMatch类型里面  

	//求最小最大距离
	double minDistance = 1000;//反向逼近
	double maxDistance = 0;
	for (int i = 0; i < src_vector.rows; i++) {
		double distance = matches[i].distance;
		if (distance > maxDistance) {
			maxDistance = distance;
		}
		if (distance < minDistance) {
			minDistance = distance;
		}
	}
	printf("max distance : %f\n", maxDistance);
	printf("min distance : %f\n", minDistance);

	//筛选较好的匹配点
	vector< DMatch > good_matches;
	for (int i = 0; i < src_vector.rows; i++) {
		double distance = matches[i].distance;
		if (distance < max(minDistance * 3, 0.02)) {
			good_matches.push_back(matches[i]);//距离小于范围的压入新的DMatch
		}
	}


	Mat MatchesImage;                                //drawMatches这个函数直接画出摆在一起的图
	drawMatches(src, keypoints1, temp, keypoints2, good_matches, MatchesImage, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);  //绘制匹配点  


	vector<Point2f> obj;
	vector<Point2f> objInScene;
	for (size_t t = 0; t < good_matches.size(); t++) {
		obj.push_back(keypoints1[good_matches[t].queryIdx].pt);//返回对象在模板图特征点坐标
		objInScene.push_back(keypoints2[good_matches[t].trainIdx].pt);//返回对象在背景查找图的坐标
	}

	Mat H = findHomography(obj, objInScene, RANSAC);//计算透视变换矩阵

	vector<Point2f> obj_corner(4);
	vector<Point2f> scene_corner(4);
	obj_corner[0] = Point(0, 0);
	obj_corner[1] = Point(src.cols, 0);
	obj_corner[2] = Point(src.cols, src.rows);
	obj_corner[3] = Point(0, src.rows);

	perspectiveTransform(obj_corner, scene_corner, H);//透视变换

	//画出边框线
	line(MatchesImage, scene_corner[0] + Point2f(src.cols, 0), scene_corner[1] + Point2f(src.cols, 0), Scalar(0, 0, 255), 2, 8, 0);
	line(MatchesImage, scene_corner[1] + Point2f(src.cols, 0), scene_corner[2] + Point2f(src.cols, 0), Scalar(0, 0, 255), 2, 8, 0);
	line(MatchesImage, scene_corner[2] + Point2f(src.cols, 0), scene_corner[3] + Point2f(src.cols, 0), Scalar(0, 0, 255), 2, 8, 0);
	line(MatchesImage, scene_corner[3] + Point2f(src.cols, 0), scene_corner[0] + Point2f(src.cols, 0), Scalar(0, 0, 255), 2, 8, 0);
	imshow("FLANN Image", MatchesImage);

	line(temp, scene_corner[0], scene_corner[1], Scalar(0, 0, 255), 2, 8, 0);
	line(temp, scene_corner[1], scene_corner[2], Scalar(0, 0, 255), 2, 8, 0);
	line(temp, scene_corner[2], scene_corner[3], Scalar(0, 0, 255), 2, 8, 0);
	line(temp, scene_corner[3], scene_corner[0], Scalar(0, 0, 255), 2, 8, 0);
	imshow("temp Image", temp);

	waitKey(0);
	return 0;
}

int orb()
{
	//-- 读取图像
	Mat img_1 = imread("../Resource/物理实验关键步骤图片/测量固体体重-物理实验_1.jpg", CV_LOAD_IMAGE_COLOR);
	Mat img_2 = imread("../Resource/物理实验关键步骤图片/测量固体体重-物理实验_2.jpg", CV_LOAD_IMAGE_COLOR);
    if (img_1.empty() || img_2.empty()) {
        printf("could not load image...\n");
        return -1;
    }

	//-- 初始化
	std::vector<KeyPoint> keypoints_1, keypoints_2;
	Mat descriptors_1, descriptors_2;
	Ptr<FeatureDetector> detector = ORB::create();
	Ptr<DescriptorExtractor> descriptor = ORB::create();
	// Ptr<FeatureDetector> detector = FeatureDetector::create(detector_name);
	// Ptr<DescriptorExtractor> descriptor = DescriptorExtractor::create(descriptor_name);
	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce-Hamming");

	//-- 第一步:检测 Oriented FAST 角点位置
	detector->detect(img_1, keypoints_1);
	detector->detect(img_2, keypoints_2);

	//-- 第二步:根据角点位置计算 BRIEF 描述子
	descriptor->compute(img_1, keypoints_1, descriptors_1);
	descriptor->compute(img_2, keypoints_2, descriptors_2);

	Mat outimg1;
	drawKeypoints(img_1, keypoints_1, outimg1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	imshow("ORB特征点", outimg1);

	//-- 第三步:对两幅图像中的BRIEF描述子进行匹配，使用 Hamming 距离
	vector<DMatch> matches;
	matcher->match(descriptors_1, descriptors_2, matches);

	//-- 第四步:匹配点对筛选
	double min_dist = 10000, max_dist = 0;

	//找出所有匹配之间的最小距离和最大距离, 即是最相似的和最不相似的两组点之间的距离
	for (int i = 0; i < descriptors_1.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	cout << "最小距离：" << min_dist << endl;
	cout << "最大距离：" << max_dist << endl;


	// 仅供娱乐的写法
	min_dist = min_element(matches.begin(), matches.end(), [](const DMatch& m1, const DMatch& m2) {return m1.distance < m2.distance; })->distance;
	max_dist = max_element(matches.begin(), matches.end(), [](const DMatch& m1, const DMatch& m2) {return m1.distance < m2.distance; })->distance;

	printf("-- Max dist : %f \n", max_dist);
	printf("-- Min dist : %f \n", min_dist);

	//当描述子之间的距离大于两倍的最小距离时,即认为匹配有误.但有时候最小距离会非常小,设置一个经验值30作为下限.
	std::vector< DMatch > good_matches;
	for (int i = 0; i < descriptors_1.rows; i++)
	{
		if (matches[i].distance <= max(2 * min_dist, 30.0))
		{
			good_matches.push_back(matches[i]);
		}
	}

	//-- 第五步:绘制匹配结果
	Mat img_match;
	Mat img_goodmatch;
	drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, img_match);
	drawMatches(img_1, keypoints_1, img_2, keypoints_2, good_matches, img_goodmatch);
	imshow("所有匹配点对", img_match);
	imshow("优化后匹配点对", img_goodmatch);

    cout << "总匹配对数：" << matches.size() << endl;
    cout << "优化后的匹配对数：" << good_matches.size() << endl;
    float dist_sum = 0;
    for (size_t i = 0; i < good_matches.size(); ++i)
    {
        cout << "第" << i + 1 << "个匹配对的欧氏距离：" << good_matches[i].distance << endl;
        dist_sum += good_matches[i].distance;
    }

    cout << "欧式距离总和：" << dist_sum << "， 平均值：" << dist_sum / good_matches.size() << endl;

	waitKey(0);
	return 0;
}

int mergeImage_c()
{
	vector<Mat> stepMats;
	vector<Mat> keyFrameMats;
	for (int i = 0; i < 4; ++i)
	{
		string imageDir = "../Resource/关键步骤图片/化学/";
		string path = imageDir + "探究金属的某些化学性质-化学实验_" + to_string(i + 1) + ".jpg";

		Timer t;
		Mat src = imread(path, IMREAD_COLOR);
		cout << "imread duration: " << t.delta<Timer::ms>() << "ms" << endl;    //52ms
		if (src.empty())
		{
			printf("could not load image...\n");
			return -1;
		}
		stepMats.emplace_back(src);
	}
	for (int j = 0; j < 4; ++j)
	{
		string imageDir = "../Resource/关键帧/化学/";
		string path = imageDir + to_string(j + 1) + ".jpg";

		Mat right = imread(path, IMREAD_COLOR);
		if (right.empty())
		{
			printf("could not load key frame image...\n");
			return -1;
		}
		keyFrameMats.emplace_back(right);
	}

	//合并两张图片
//CV_ASSERT(keypoint_img_left.cols == keypoint_img_right.cols && keypoint_img_left.type() == keypoint_img_right.type());

	for (int i = 0; i < 4; ++i)
	{
		Mat keypoint_img_left = stepMats[i];
		for (int j = 0; j < 4; ++j)
		{
			Mat keypoint_img_right = keyFrameMats[j];

			int totalCols = keypoint_img_left.cols + keypoint_img_right.cols;
			Mat mergedDescriptors(keypoint_img_left.rows, totalCols, keypoint_img_left.type());
			Mat submat = mergedDescriptors.colRange(0, keypoint_img_left.cols);
			keypoint_img_left.copyTo(submat);
			submat = mergedDescriptors.colRange(keypoint_img_left.cols, totalCols);
			keypoint_img_right.copyTo(submat);

			string outPath = "../Resource/原图合并/化学/";
			outPath += to_string(i + 1) + "_" + to_string(j + 1) + ".jpg";
			bool b = imwrite(outPath, mergedDescriptors);
		}
	}
	return 0;
}
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

//	Threshold for the keypoint detector.Only features, whose hessian is larger than hessianThreshold are retained by the detector.
//	Therefore, the larger the value, the less keypoints you will get.A good default value could be from 300 to 500, depending from the image contrast.
#define SurfMinHessian	400


// 提取特征点并绘制
int extractFeatureDemo() 
{
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

//暴力匹配
int bfMatch()
{
	Mat src = imread("../Resource/特征匹配/3.jpg");
	Mat temp = imread("../Resource/特征匹配/4.jpg");
	if (src.empty() || temp.empty()) {
		printf("could not load image...\n");
		return -1;
	}

	// SURF特征点检测
	int minHessian = 400;
	Ptr<SURF> detector = SURF::create(minHessian, 4, 3, true, true);//创建一个surf类检测器对象并初始化
	vector<KeyPoint> keypoints1, keypoints2;
	Mat src_vector, temp_vector;//用来存放特征点的描述向量

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
#if 1
	//ptsPairs = matches.size();
	// 为了增加演示效果，这里增加了匹配点数
	ptsPairs = 100;
#endif
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
	namedWindow("BFMatcher Image", CV_WND_PROP_FULLSCREEN);
	imshow("BFMatcher Image", MatchesImage);
	//bool b = imwrite("./match.jpg", MatchesImage);

	waitKey(0);
	return 0;
}

//FLANN匹配
int flannMatch()
{
	Mat src = imread("../Resource/特征匹配/3.jpg");
	Mat temp = imread("../Resource/特征匹配/4.jpg");
	if (src.empty() || temp.empty()) {
		printf("could not load image...\n");
		return -1;
	}

	// SURF特征点检测
	int minHessian = 400;
	/*
	Threshold for the keypoint detector.Only features, whose hessian is larger than hessianThreshold are retained by the detector.
	Therefore, the larger the value, the less keypoints you will get.A good default value could be from 300 to 500, depending from the image contrast.
	*/
	Ptr<SURF> detector = SURF::create(minHessian, 4, 3, true, true);//创建一个surf类检测器对象并初始化
	vector<KeyPoint> keypoints1, keypoints2;
	Mat src_vector, temp_vector;//用来存放特征点的描述向量

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
	namedWindow("FLANN Image", CV_WND_PROP_FULLSCREEN);
	imshow("FLANN Image", MatchesImage);

	waitKey(0);
	return 0;
}

// ORB匹配
int orb()
{
	//-- 读取图像
	Mat img_1 = imread("../Resource/特征匹配/3.jpg");
	Mat img_2 = imread("../Resource/特征匹配/4.jpg");
    if (img_1.empty() || img_2.empty()) {
        printf("could not load image...\n");
        return -1;
    }

	//-- 初始化
	std::vector<KeyPoint> keypoints_1, keypoints_2;
	Mat descriptors_1, descriptors_2;
	Ptr<FeatureDetector> detector = ORB::create();
	Ptr<DescriptorExtractor> descriptor = ORB::create();
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
	//namedWindow("ORB所有匹配点对", CV_WND_PROP_FULLSCREEN);
	imshow("ORB所有匹配点对", img_match);
	//namedWindow("ORB优化后匹配点对", CV_WND_PROP_FULLSCREEN);
	imshow("ORB优化后匹配点对", img_goodmatch);

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

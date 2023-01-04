#include "Timer.h"

#include <iostream>
#include <math.h>

//CV_BGR2GRAYδ�����ı�ʶ���Ľ���취
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

//����������
int extractFeatureDemo1() 
{
	//Mat src = imread("../Resource/����ʵ��ؼ�����ͼƬ/������������-����ʵ��_1.jpg", IMREAD_GRAYSCALE);
	Mat src = imread("../Resource/�ؼ�����ͼƬ/fp2.jpg", IMREAD_COLOR);
	if (src.empty()) 
	{
		printf("could not load image...\n");
		return -1;
	}
	//namedWindow("input image", CV_WINDOW_AUTOSIZE);
	imshow("����ͼƬ", src);

	// SURF��������
	int minHessian = 100;
	Ptr<SURF> detector = SURF::create(minHessian);//����һ��surf����󲢳�ʼ��
	vector<KeyPoint> keypoints;
	detector->detect(src, keypoints, Mat());//�ҳ��ؼ���

	// ���ƹؼ���
	Mat keypoint_img;
	drawKeypoints(src, keypoints, keypoint_img, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	imshow("������������ͼƬ", keypoint_img);

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
		string imageDir = "../Resource/�ؼ�����ͼƬ/����/";
		string path = imageDir + "������������-����ʵ��_" + to_string(i + 1) + ".jpg";

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
		string imageDir = "../Resource/�ؼ�֡/����/";
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
		// SURF��������
		Ptr<SURF> detector = SURF::create(SurfMinHessian);//����һ��surf����󲢳�ʼ��
		vector<KeyPoint> keypoints;
		detector->detect(stepMats[i], keypoints, Mat());//�ҳ��ؼ���

		// ���ƹؼ���
		Mat keypoint_img_left;
		drawKeypoints(stepMats[i], keypoints, keypoint_img_left, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

		for (int j = 0; j < PhysicsKeyFrameNum; ++j)
		{
			// SURF��������
			Ptr<SURF> detector_right = SURF::create(SurfMinHessian);//����һ��surf����󲢳�ʼ��
			vector<KeyPoint> keypoints_right;
			detector_right->detect(keyFrameMats[j], keypoints_right, Mat());//�ҳ��ؼ���

			// ���ƹؼ���
			Mat keypoint_img_right;
			drawKeypoints(keyFrameMats[j], keypoints_right, keypoint_img_right, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

			//�ϲ�����ͼƬ
			Mat mergeMat = mergeCols(keypoint_img_left, keypoint_img_right);

			outPath = "../Resource/����ͼ/����/";
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
        string imageDir = "../Resource/�ؼ�����ͼƬ/��ѧ/";
        string path = imageDir + "̽��������ĳЩ��ѧ����-��ѧʵ��_" + to_string(i + 1) + ".jpg";

        //Timer t;
        Mat src = imread(path, IMREAD_COLOR);
        //cout << "imread duration: " << t.delta<Timer::ms>() << "ms" << endl;    //52ms
        if (src.empty())
        {
			printf("could not load image...\n");
			return -1;
		}
		//��С��720x404
		resize(src, src, Size(720, 404));

		stepMats.emplace_back(src);

		// SURF��������
		Ptr<SURF> detector = SURF::create(SurfMinHessian);//����һ��surf����󲢳�ʼ��
		vector<KeyPoint> keypoints;
		detector->detect(stepMats[i], keypoints, Mat());//�ҳ��ؼ���

		// ���ƹؼ���
		Mat keypoint_img_left;
		drawKeypoints(stepMats[i], keypoints, keypoint_img_left, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
		lMats.emplace_back(keypoint_img_left);

    }
    for (int j = 0; j < ChemistryKeyFrameNum; ++j)
    {
        string imageDir = "../Resource/�ؼ�֡/��ѧ/";
        string path = imageDir + to_string(j + 1) + ".jpg";

        Mat right = imread(path, IMREAD_COLOR);
        if (right.empty())
        {
            printf("could not load key frame image...\n");
            return -1;
        }
		resize(right, right, Size(720, 404));

        keyFrameMats.emplace_back(right);

		// SURF��������
		Ptr<SURF> detector_right = SURF::create(SurfMinHessian);//����һ��surf����󲢳�ʼ��
		vector<KeyPoint> keypoints_right;
		Timer t2;
		detector_right->detect(keyFrameMats[j], keypoints_right, Mat());//�ҳ��ؼ���
		//cout << "detect duration: " << t2.delta<Timer::ms>() << "ms" << endl;   //802ms

		// ���ƹؼ���
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

            //�ϲ�����ͼƬ
			Mat mergeMat = mergeCols(left, right);
            outPath = "../Resource/����ͼ/��ѧ/";
            outPath += to_string(i + 1) + "_" + to_string(j + 1) + ".jpg";
            bool b = imwrite(outPath, mergeMat);
        }

    }
	cout << "extractFeatureDemo3 time: " << t.delta<Timer::s>() << "s" << endl;
    return 0;
}

//����ƥ��
int bfMatch()
{

	Mat src = imread("../Resource/����ʵ��ؼ�����ͼƬ/������������-����ʵ��_1.jpg");
	Mat temp = imread("../Resource/����ʵ��ؼ�����ͼƬ/������������-����ʵ��_2.jpg");
	if (src.empty() || temp.empty()) {
		printf("could not load image...\n");
		return -1;
	}
	namedWindow("input image", CV_WINDOW_AUTOSIZE);
	imshow("input image", src);

	// SURF��������
	int minHessian = 400;
	Ptr<SURF> detector = SURF::create(minHessian, 4, 3, true, true);//����һ��surf���������󲢳�ʼ��
	vector<KeyPoint> keypoints1, keypoints2;
	Mat src_vector, temp_vector;//����������������������

	//detector->detect(src, keypoints1, Mat());//�ҳ��ؼ���
	//detector->detect(temp, keypoints2, Mat());//�ҳ��ؼ���

	//�ҵ������㲢��������������(����)
	detector->detectAndCompute(src, Mat(), keypoints1, src_vector);//����ͼ���������룬���������㣬���Mat������������������������
	detector->detectAndCompute(temp, Mat(), keypoints2, temp_vector);//���Mat����Ϊ������ĸ���������Ϊÿ�����������ĳߴ磬SURF��64��ά��


	//ƥ��
	BFMatcher matcher(NORM_L2);         //ʵ����һ������ƥ����(���������ѡ��ƥ�䷽��)

	vector<DMatch> matches;    //DMatch����������ƥ��õ�һ����������࣬������������֮���ƥ����Ϣ
							   //������ͼ�и�����m��������ͼ��������n��ƥ�䣬���DMatch�ͼ�¼������ƥ�䣬���һ���¼m��n��
							   //���������ľ����������Ϣ����������ں���������ɸѡ    

	matcher.match(src_vector, temp_vector, matches);             //ƥ�䣬������Դ��������������������DMatch��������  

	//ƥ���ɸѡ
	//sort���������ݽ�����������
	//ɸѡƥ��㣬����match���������Եľ����С��������    
	//ɸѡ�����ŵ�30��ƥ��㣨���Բ�ʹ�ã��ử�����������㣩

	sort(matches.begin(), matches.end());
	vector< DMatch > good_matches;
	int ptsPairs = std::min(30, (int)(matches.size() * 0.15));//ƥ�������������50
	//int ptsPairs = 2;
	//cout << ptsPairs << endl;
	for (int i = 0; i < ptsPairs; i++)
	{
		good_matches.push_back(matches[i]);//������С��50��ѹ���µ�DMatch
	}

    for (size_t i = 0; i < good_matches.size(); ++i)
    {
        cout << "BFƥ�䣬��" << i + 1 << "��ƥ���ŷʽ���룺" << good_matches[i].distance << endl;
    }

	Mat MatchesImage;                                //drawMatches�������ֱ�ӻ�������һ���ͼ
	drawMatches(src, keypoints1, temp, keypoints2, good_matches, MatchesImage, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);  //����ƥ���  
	imshow("BFMatcher Image", MatchesImage);

	waitKey(0);
	return 0;

}

//FLANNƥ��
int flannMatch()
{

	//Mat src = imread("../Resource/����ʵ��ؼ�����ͼƬ/������������-����ʵ��_1.jpg", IMREAD_GRAYSCALE);
	Mat src = imread("../Resource/����ʵ��ؼ�����ͼƬ/������������-����ʵ��_1.jpg", IMREAD_COLOR);
	Mat temp = imread("../Resource/����ʵ��ؼ�����ͼƬ/������������-����ʵ��_2.jpg", IMREAD_COLOR);
	if (src.empty() || temp.empty()) {
		printf("could not load image...\n");
		return -1;
	}
	namedWindow("input image", CV_WINDOW_AUTOSIZE);
	imshow("input image", src);

	// SURF��������
	int minHessian = 400;
	/*
	Threshold for the keypoint detector.Only features, whose hessian is larger than hessianThreshold are retained by the detector.
	Therefore, the larger the value, the less keypoints you will get.A good default value could be from 300 to 500, depending from the image contrast.
	*/
	Ptr<SURF> detector = SURF::create(minHessian, 4, 3, true, true);//����һ��surf���������󲢳�ʼ��
	vector<KeyPoint> keypoints1, keypoints2;
	Mat src_vector, temp_vector;//����������������������

	//detector->detect(src, keypoints1, Mat());//�ҳ��ؼ���
	//detector->detect(temp, keypoints2, Mat());//�ҳ��ؼ���

	//�ҵ������㲢��������������(����)
	detector->detectAndCompute(src, Mat(), keypoints1, src_vector);//����ͼ���������룬���������㣬���Mat������������������������
	detector->detectAndCompute(temp, Mat(), keypoints2, temp_vector);//���Mat����Ϊ������ĸ���������Ϊÿ�����������ĳߴ磬SURF��64��ά��


	//ƥ��
	FlannBasedMatcher matcher;         //ʵ����һ��FLANNƥ����(���������ѡ��ƥ�䷽��)

	vector<DMatch> matches;    //DMatch����������ƥ��õ�һ����������࣬������������֮���ƥ����Ϣ
							   //������ͼ�и�����m��������ͼ��������n��ƥ�䣬���DMatch�ͼ�¼������ƥ�䣬���һ���¼m��n��
							   //���������ľ����������Ϣ����������ں���������ɸѡ    

	matcher.match(src_vector, temp_vector, matches);             //ƥ�䣬������Դ��������������������DMatch��������  

	//����С������
	double minDistance = 1000;//����ƽ�
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

	//ɸѡ�Ϻõ�ƥ���
	//vector< DMatch > good_matches;
	//for (int i = 0; i < src_vector.rows; i++) {
	//	double distance = matches[i].distance;
	//	if (distance < max(minDistance * 2, 0.02)) {
	//		good_matches.push_back(matches[i]);//����С�ڷ�Χ��ѹ���µ�DMatch
	//	}
	//}

	//good_matches = matches;

	//sort���������ݽ�����������
	//ɸѡƥ��㣬����match���������Եľ����С��������
	//ɸѡ�����ŵ�50��ƥ��㣨���Բ�ʹ�ã��ử�����������㣩

	sort(matches.begin(), matches.end());
	vector< DMatch > good_matches;
	int ptsPairs = std::min(50, (int)(matches.size() * 0.15));//ƥ�������������50
	cout << ptsPairs << endl;
	for (int i = 0; i < ptsPairs; i++)
	{
		good_matches.push_back(matches[i]);//������С��50��ѹ���µ�DMatch
	}
	

	Mat MatchesImage;                                //drawMatches�������ֱ�ӻ�������һ���ͼ
	drawMatches(src, keypoints1, temp, keypoints2, good_matches, MatchesImage, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);  //����ƥ���  
	imshow("FLANN Image", MatchesImage);
	imwrite("../Resource/tt.jpg", MatchesImage);

	waitKey(0);
	return 0;
}

int flannMatch_p()
{
	Timer tt;
	// SURF��������
	Ptr<SURF> detector = SURF::create(SurfMinHessian, 4, 3, true, true);//����һ��surf���������󲢳�ʼ��

	vector<Mat> stepMats;
	vector<Mat> keyFrameMats;
	vector<vector<KeyPoint>> lKeyPointsVec;
	vector<vector<KeyPoint>> rKeyPointsVecs;
	vector<Mat> lMats;
	vector<Mat> rMats;
	for (int i = 0; i < PhysicsStepNum; ++i)
	{
		string stepPath = "../Resource/�ؼ�����ͼƬ/����/������������-����ʵ��_";
		stepPath += to_string(i + 1) + ".jpg";
		Mat src = imread(stepPath, IMREAD_COLOR);
		if (src.empty()) {
			printf("could not load image...\n");
			return -1;
		}
		stepMats.emplace_back(src);

		vector<KeyPoint> keypoints1;
		Mat src_vector;//����������������������
		//�ҵ������㲢��������������(����)
		detector->detectAndCompute(stepMats[i], Mat(), keypoints1, src_vector);//����ͼ���������룬���������㣬���Mat������������������������
		lKeyPointsVec.emplace_back(keypoints1);
		lMats.emplace_back(src_vector);
	}
	for (int i = 0; i < PhysicsKeyFrameNum; ++i)
	{
		string keyFramePath = "../Resource/�ؼ�֡/����/";
		keyFramePath += to_string(i + 1) + ".jpg";
		Mat dst = imread(keyFramePath, IMREAD_COLOR);
		if (dst.empty()) {
			printf("could not load image...\n");
			return -1;
		}
		keyFrameMats.emplace_back(dst);

		vector<KeyPoint> keypoints2;
		Mat temp_vector;//����������������������
		//Timer t;
		detector->detectAndCompute(keyFrameMats[i], Mat(), keypoints2, temp_vector);//���Mat����Ϊ������ĸ���������Ϊÿ�����������ĳߴ磬SURF��64��ά��
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

			FlannBasedMatcher matcher;         //ʵ����һ��FLANNƥ����(���������ѡ��ƥ�䷽��)
			vector<DMatch> matches;    //DMatch����������ƥ��õ�һ����������࣬������������֮���ƥ����Ϣ
			//Timer t2;
			matcher.match(src_vector, temp_vector, matches);             //ƥ�䣬������Դ��������������������DMatch��������  
			//cout << "match duration: " << t2.delta<Timer::ms>() << "ms" << endl;    //692ms

			Mat MatchesImage;                                //drawMatches�������ֱ�ӻ�������һ���ͼ
			//Timer t3;
			drawMatches(stepMats[i], keypoints1, keyFrameMats[j], keypoints2, matches, MatchesImage, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);  //����ƥ���  
			//cout << "drawMatches duration: " << t3.delta<Timer::ms>() << "ms" << endl;  //389ms
			string outPath = "../Resource/����������ͼ/����/";
			outPath += to_string(i + 1) + "_" + to_string(j + 1) + ".jpg";
			imwrite(outPath, MatchesImage);
			cout << "��ѧ��һ������ͼ��ʱ��" << t4.delta<Timer::ms>() << "����" << endl;
		}
	}

	cout << "flannMatch_p time: " << tt.delta<Timer::s>() << "s" << endl;
	return 0;
}


int flannMatch_c()
{
	Timer tt;
	// SURF��������
    Ptr<SURF> detector = SURF::create(SurfMinHessian, 4, 3, true, true);//����һ��surf���������󲢳�ʼ��

    vector<Mat> stepMats;
    vector<Mat> keyFrameMats;
	vector<vector<KeyPoint>> lKeyPointsVec;
	vector<vector<KeyPoint>> rKeyPointsVecs;
	vector<Mat> lMats;
	vector<Mat> rMats;
    for (int i = 0; i < ChemistryStepNum; ++i)
    {
        string stepPath = "../Resource/�ؼ�����ͼƬ/��ѧ/̽��������ĳЩ��ѧ����-��ѧʵ��_";
        stepPath += to_string(i + 1) + ".jpg";
        Mat src = imread(stepPath, IMREAD_COLOR);
        if (src.empty() ) {
            printf("could not load image...\n");
            return -1;
        }
		resize(src, src, Size(720, 404));

        stepMats.emplace_back(src);

		vector<KeyPoint> keypoints1;
		Mat src_vector;//����������������������
		//�ҵ������㲢��������������(����)
		detector->detectAndCompute(stepMats[i], Mat(), keypoints1, src_vector);//����ͼ���������룬���������㣬���Mat������������������������
		lKeyPointsVec.emplace_back(keypoints1);
		lMats.emplace_back(src_vector);
    }
    for (int i = 0; i < ChemistryKeyFrameNum; ++i)
    {
        string keyFramePath = "../Resource/�ؼ�֡/��ѧ/";
        keyFramePath += to_string(i + 1) + ".jpg";
        Mat dst = imread(keyFramePath, IMREAD_COLOR);
        if (dst.empty()) {
            printf("could not load image...\n");
            return -1;
        }
		resize(dst, dst, Size(720, 404));

        keyFrameMats.emplace_back(dst);

		vector<KeyPoint> keypoints2;
		Mat temp_vector;//����������������������
		Timer t;
		detector->detectAndCompute(keyFrameMats[i], Mat(), keypoints2, temp_vector);//���Mat����Ϊ������ĸ���������Ϊÿ�����������ĳߴ磬SURF��64��ά��
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

            FlannBasedMatcher matcher;         //ʵ����һ��FLANNƥ����(���������ѡ��ƥ�䷽��)
            vector<DMatch> matches;    //DMatch����������ƥ��õ�һ����������࣬������������֮���ƥ����Ϣ
            //Timer t2;
            matcher.match(src_vector, temp_vector, matches);             //ƥ�䣬������Դ��������������������DMatch��������  
            //cout << "match duration: " << t2.delta<Timer::ms>() << "ms" << endl;	//385ms

            Mat MatchesImage;                                //drawMatches�������ֱ�ӻ�������һ���ͼ
            //Timer t3;
            drawMatches(stepMats[i], keypoints1, keyFrameMats[j], keypoints2, matches, MatchesImage, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);  //����ƥ���  
            //cout << "drawMatches duration: " << t3.delta<Timer::ms>() << "ms" << endl;	//211ms
            string outPath = "../Resource/����������ͼ/��ѧ/";
            outPath += to_string(i + 1) + "_" + to_string(j + 1) + ".jpg";
            imwrite(outPath, MatchesImage);
			//cout << "��ѧ��һ������ͼ��ʱ��" << t4.delta<Timer::ms>() << "����" << endl;	//338ms
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

	// SURF��������
	int minHessian = 400;
	Ptr<SURF> detector = SURF::create(minHessian, 4, 3, true, true);//����һ��surf���������󲢳�ʼ��
	vector<KeyPoint> keypoints1, keypoints2;
	Mat src_vector, temp_vector;//����������������������

	//detector->detect(src, keypoints1, Mat());//�ҳ��ؼ���
	//detector->detect(temp, keypoints2, Mat());//�ҳ��ؼ���

	//�ҵ������㲢��������������(����)
	detector->detectAndCompute(src, Mat(), keypoints1, src_vector);//����ͼ���������룬���������㣬���Mat������������������������
	detector->detectAndCompute(temp, Mat(), keypoints2, temp_vector);//���Mat����Ϊ������ĸ���������Ϊÿ�����������ĳߴ磬SURF��64��ά��


	//ƥ��
	FlannBasedMatcher matcher;         //ʵ����һ��FLANNƥ����(���������ѡ��ƥ�䷽��)

	vector<DMatch> matches;    //DMatch����������ƥ��õ�һ����������࣬������������֮���ƥ����Ϣ
							   //������ͼ�и�����m��������ͼ��������n��ƥ�䣬���DMatch�ͼ�¼������ƥ�䣬���һ���¼m��n��
							   //���������ľ����������Ϣ����������ں���������ɸѡ    

	matcher.match(src_vector, temp_vector, matches);             //ƥ�䣬������Դ��������������������DMatch��������  

	//����С������
	double minDistance = 1000;//����ƽ�
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

	//ɸѡ�Ϻõ�ƥ���
	vector< DMatch > good_matches;
	for (int i = 0; i < src_vector.rows; i++) {
		double distance = matches[i].distance;
		if (distance < max(minDistance * 3, 0.02)) {
			good_matches.push_back(matches[i]);//����С�ڷ�Χ��ѹ���µ�DMatch
		}
	}


	Mat MatchesImage;                                //drawMatches�������ֱ�ӻ�������һ���ͼ
	drawMatches(src, keypoints1, temp, keypoints2, good_matches, MatchesImage, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);  //����ƥ���  


	vector<Point2f> obj;
	vector<Point2f> objInScene;
	for (size_t t = 0; t < good_matches.size(); t++) {
		obj.push_back(keypoints1[good_matches[t].queryIdx].pt);//���ض�����ģ��ͼ����������
		objInScene.push_back(keypoints2[good_matches[t].trainIdx].pt);//���ض����ڱ�������ͼ������
	}

	Mat H = findHomography(obj, objInScene, RANSAC);//����͸�ӱ任����

	vector<Point2f> obj_corner(4);
	vector<Point2f> scene_corner(4);
	obj_corner[0] = Point(0, 0);
	obj_corner[1] = Point(src.cols, 0);
	obj_corner[2] = Point(src.cols, src.rows);
	obj_corner[3] = Point(0, src.rows);

	perspectiveTransform(obj_corner, scene_corner, H);//͸�ӱ任

	//�����߿���
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
	//-- ��ȡͼ��
	Mat img_1 = imread("../Resource/����ʵ��ؼ�����ͼƬ/������������-����ʵ��_1.jpg", CV_LOAD_IMAGE_COLOR);
	Mat img_2 = imread("../Resource/����ʵ��ؼ�����ͼƬ/������������-����ʵ��_2.jpg", CV_LOAD_IMAGE_COLOR);
    if (img_1.empty() || img_2.empty()) {
        printf("could not load image...\n");
        return -1;
    }

	//-- ��ʼ��
	std::vector<KeyPoint> keypoints_1, keypoints_2;
	Mat descriptors_1, descriptors_2;
	Ptr<FeatureDetector> detector = ORB::create();
	Ptr<DescriptorExtractor> descriptor = ORB::create();
	// Ptr<FeatureDetector> detector = FeatureDetector::create(detector_name);
	// Ptr<DescriptorExtractor> descriptor = DescriptorExtractor::create(descriptor_name);
	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce-Hamming");

	//-- ��һ��:��� Oriented FAST �ǵ�λ��
	detector->detect(img_1, keypoints_1);
	detector->detect(img_2, keypoints_2);

	//-- �ڶ���:���ݽǵ�λ�ü��� BRIEF ������
	descriptor->compute(img_1, keypoints_1, descriptors_1);
	descriptor->compute(img_2, keypoints_2, descriptors_2);

	Mat outimg1;
	drawKeypoints(img_1, keypoints_1, outimg1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	imshow("ORB������", outimg1);

	//-- ������:������ͼ���е�BRIEF�����ӽ���ƥ�䣬ʹ�� Hamming ����
	vector<DMatch> matches;
	matcher->match(descriptors_1, descriptors_2, matches);

	//-- ���Ĳ�:ƥ����ɸѡ
	double min_dist = 10000, max_dist = 0;

	//�ҳ�����ƥ��֮�����С�����������, ���������Ƶĺ�����Ƶ������֮��ľ���
	for (int i = 0; i < descriptors_1.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	cout << "��С���룺" << min_dist << endl;
	cout << "�����룺" << max_dist << endl;


	// �������ֵ�д��
	min_dist = min_element(matches.begin(), matches.end(), [](const DMatch& m1, const DMatch& m2) {return m1.distance < m2.distance; })->distance;
	max_dist = max_element(matches.begin(), matches.end(), [](const DMatch& m1, const DMatch& m2) {return m1.distance < m2.distance; })->distance;

	printf("-- Max dist : %f \n", max_dist);
	printf("-- Min dist : %f \n", min_dist);

	//��������֮��ľ��������������С����ʱ,����Ϊƥ������.����ʱ����С�����ǳ�С,����һ������ֵ30��Ϊ����.
	std::vector< DMatch > good_matches;
	for (int i = 0; i < descriptors_1.rows; i++)
	{
		if (matches[i].distance <= max(2 * min_dist, 30.0))
		{
			good_matches.push_back(matches[i]);
		}
	}

	//-- ���岽:����ƥ����
	Mat img_match;
	Mat img_goodmatch;
	drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, img_match);
	drawMatches(img_1, keypoints_1, img_2, keypoints_2, good_matches, img_goodmatch);
	imshow("����ƥ����", img_match);
	imshow("�Ż���ƥ����", img_goodmatch);

    cout << "��ƥ�������" << matches.size() << endl;
    cout << "�Ż����ƥ�������" << good_matches.size() << endl;
    float dist_sum = 0;
    for (size_t i = 0; i < good_matches.size(); ++i)
    {
        cout << "��" << i + 1 << "��ƥ��Ե�ŷ�Ͼ��룺" << good_matches[i].distance << endl;
        dist_sum += good_matches[i].distance;
    }

    cout << "ŷʽ�����ܺͣ�" << dist_sum << "�� ƽ��ֵ��" << dist_sum / good_matches.size() << endl;

	waitKey(0);
	return 0;
}

int mergeImage_c()
{
	vector<Mat> stepMats;
	vector<Mat> keyFrameMats;
	for (int i = 0; i < 4; ++i)
	{
		string imageDir = "../Resource/�ؼ�����ͼƬ/��ѧ/";
		string path = imageDir + "̽��������ĳЩ��ѧ����-��ѧʵ��_" + to_string(i + 1) + ".jpg";

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
		string imageDir = "../Resource/�ؼ�֡/��ѧ/";
		string path = imageDir + to_string(j + 1) + ".jpg";

		Mat right = imread(path, IMREAD_COLOR);
		if (right.empty())
		{
			printf("could not load key frame image...\n");
			return -1;
		}
		keyFrameMats.emplace_back(right);
	}

	//�ϲ�����ͼƬ
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

			string outPath = "../Resource/ԭͼ�ϲ�/��ѧ/";
			outPath += to_string(i + 1) + "_" + to_string(j + 1) + ".jpg";
			bool b = imwrite(outPath, mergedDescriptors);
		}
	}
	return 0;
}
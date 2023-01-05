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

//	Threshold for the keypoint detector.Only features, whose hessian is larger than hessianThreshold are retained by the detector.
//	Therefore, the larger the value, the less keypoints you will get.A good default value could be from 300 to 500, depending from the image contrast.
#define SurfMinHessian	400


// ��ȡ�����㲢����
int extractFeatureDemo() 
{
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

//����ƥ��
int bfMatch()
{
	Mat src = imread("../Resource/����ƥ��/3.jpg");
	Mat temp = imread("../Resource/����ƥ��/4.jpg");
	if (src.empty() || temp.empty()) {
		printf("could not load image...\n");
		return -1;
	}

	// SURF��������
	int minHessian = 400;
	Ptr<SURF> detector = SURF::create(minHessian, 4, 3, true, true);//����һ��surf���������󲢳�ʼ��
	vector<KeyPoint> keypoints1, keypoints2;
	Mat src_vector, temp_vector;//����������������������

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
#if 1
	//ptsPairs = matches.size();
	// Ϊ��������ʾЧ��������������ƥ�����
	ptsPairs = 100;
#endif
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
	namedWindow("BFMatcher Image", CV_WND_PROP_FULLSCREEN);
	imshow("BFMatcher Image", MatchesImage);
	//bool b = imwrite("./match.jpg", MatchesImage);

	waitKey(0);
	return 0;
}

//FLANNƥ��
int flannMatch()
{
	Mat src = imread("../Resource/����ƥ��/3.jpg");
	Mat temp = imread("../Resource/����ƥ��/4.jpg");
	if (src.empty() || temp.empty()) {
		printf("could not load image...\n");
		return -1;
	}

	// SURF��������
	int minHessian = 400;
	/*
	Threshold for the keypoint detector.Only features, whose hessian is larger than hessianThreshold are retained by the detector.
	Therefore, the larger the value, the less keypoints you will get.A good default value could be from 300 to 500, depending from the image contrast.
	*/
	Ptr<SURF> detector = SURF::create(minHessian, 4, 3, true, true);//����һ��surf���������󲢳�ʼ��
	vector<KeyPoint> keypoints1, keypoints2;
	Mat src_vector, temp_vector;//����������������������

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
	namedWindow("FLANN Image", CV_WND_PROP_FULLSCREEN);
	imshow("FLANN Image", MatchesImage);

	waitKey(0);
	return 0;
}

// ORBƥ��
int orb()
{
	//-- ��ȡͼ��
	Mat img_1 = imread("../Resource/����ƥ��/3.jpg");
	Mat img_2 = imread("../Resource/����ƥ��/4.jpg");
    if (img_1.empty() || img_2.empty()) {
        printf("could not load image...\n");
        return -1;
    }

	//-- ��ʼ��
	std::vector<KeyPoint> keypoints_1, keypoints_2;
	Mat descriptors_1, descriptors_2;
	Ptr<FeatureDetector> detector = ORB::create();
	Ptr<DescriptorExtractor> descriptor = ORB::create();
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
	//namedWindow("ORB����ƥ����", CV_WND_PROP_FULLSCREEN);
	imshow("ORB����ƥ����", img_match);
	//namedWindow("ORB�Ż���ƥ����", CV_WND_PROP_FULLSCREEN);
	imshow("ORB�Ż���ƥ����", img_goodmatch);

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

//Interface Collection for Motion Detection

//Opencv Library Declarations

#include "opencv2/core/core.hpp"
#include "opencv2/flann/miniflann.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/photo/photo.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/core/core_c.h"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/legacy/legacy.hpp"

#include "opencv2/video/background_segm.hpp"
//Basic C++ library Declarations
#include <iostream>

#include <string>
#include <vector>
#include <numeric>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "../package_tracking/BlobTracking.h"
#include "../package_analysis/VehicleCouting.h"


using namespace std;


class MovingDetection{

private:

	cv::VideoCapture cap;
	cv::Mat background, result;

	cv::Mat img_blob;

	cv::Rect box;
	vector<cv::Rect> boundRect();
	cv::RotatedRect streetcurb;
	cv::RotatedRect streetcurb2;
	int detected[100][2];

public:

	cv::Mat output, mask;
	string file;
	string videoname;
	//Constructor
	MovingDetection();

	//user interface
	void menu();
	void process(cv::Mat &img_input, cv::Mat &img_mask);


	//read in and play a video file
	int readVideo(string address);
	void playVideo();

	//moving tracking functions
	void getBackground(string address);
	//void filterMovingObjects(string address1, string address2);
	void filterMovingObjects(string address1);
	cv::Mat filterTotalBackground(cv::Mat frame);

	cv::BackgroundSubtractorMOG2 bgSubtractor;

	//save the average static background image
	void saveBackgroundImage(string address);

	//Read User Input Functions
	cv::string readString(string message);
	double readDouble(string message);
	int readInt(string message);
	int readOddInt(string message);
};

#include "MovingDetection.h"

/*
Default Constructor
notes: creates the MovingDetection object
*/
MovingDetection::MovingDetection(){
	background = cv::Mat::zeros(1, 1, CV_64F);
	result = cv::Mat::zeros(1, 1, CV_64F);
	cap = 0;
}


//Menu: Function list
void MovingDetection::menu(){

	videoname = "20160401_10fps";

	file = "street7.mkv";
	string file1 = "street7.mkv";
	bool isNewVideo = true;
	bool stillworking = true;
	while (stillworking){

		//prints out the menu options the user can choose from
 /*
		cout << "Select one of the options below\n\n";
		cout << "\t10. Test normal scene\n";
		cout << "\t11. Test burst scene\n\n\n";
		cout << "\t0. Play sample video\n";
		cout << "\t2. Try specific video to process\n";
		cout << "\t3. Get the Accumulated Background and Extract Moving Objects\n";
		cout << "\t4. End Program\n\n";

		//prompts the user for an option number then carries out a command based
		//off of the option number provided
		int option = readInt("Enter Option Number: ");
*/
                int option = 10;
		if (option == 10) {
			if (!file.empty()){
				getBackground(file);
				isNewVideo = false;
				filterMovingObjects(file);
			}
		}
		if (option == 11) {
			if (!file1.empty()){
				getBackground(file1);
				isNewVideo = false;
				filterMovingObjects(file1);
			}
		}

		else if (option == 0){
			readVideo(file);
			playVideo();
		}
		else if (option == 2){
			string temp = readString("Enter video address: ");
			file = "D:/ECE698_Proj/test_video_library/" + temp;
			isNewVideo = true;
		}
		else if (option == 3){
			if (!file.empty()){
				getBackground(file);
				isNewVideo = false;
				filterMovingObjects(file);
			}
			else{
				cout << "read in a video file";
			}
		}
		/*
		else if (option == 3){
		if (!isNewVideo){
		string address = readString("Enter the name for output video:  ");
		filterMovingObjects(file, address);
		}
		else{
		cout << "get static background";
		}
		}
		*/
		else if (option == 4){
			stillworking = false;
		}
	}
}

//Basic method: readVideo
/*
readVideo
inputs: string representing the address of a video file
outputs: none
description: Loads the video file at the addres string if the video
could not be readed print out an error before exiting
*/
int MovingDetection::readVideo(string address){

	//load the image from the address param as either an RGB or greyscale image
	cv::VideoCapture dst(address);

	//Check to see if the image loaded correctly if not return
	if (!dst.isOpened())
	{
		cout << "Error: Video cannot be loaded" << endl;
		return -1;
	}

	//saves loaded video to one of the class variables
	cap = dst;
	return 1;
}

/*
playVideo
inputs: none
outputs: none
description: plays the video file loaded by the readVideo function
*/
void MovingDetection::playVideo(){

	int totalFrameNumber = cap.get(CV_CAP_PROP_FRAME_COUNT);
	double fps = cap.get(CV_CAP_PROP_FPS);
	long width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	long height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	cout << width << " * " << height << ", " << fps << " FPS, " << totalFrameNumber << " frames totally." << endl;

	cv::Mat frame;
	cv::namedWindow("Crossroad", CV_WINDOW_AUTOSIZE);
	while (1){
		// read a new frame from video
		bool bSuccess = cap.read(frame);
		//if not success, break loop
		if (!bSuccess)
		{
			cout << "Cannot read the frame from video file" << endl;
			break;
		}

		//show the frame
		//cv::imshow("Crossroad", frame);
		//wait for 'esc' key press for 1 ms. If 'esc' key is pressed, break loop
		if (cv::waitKey(1) == 27)
		{
			cout << "Press 'ESC' to exit" << endl;
			break;
		}
	}
	cv::destroyAllWindows();
}

/*
readDouble
inputs: string representing a message to be displayed
outputs: a double representing the number the user passes
description: prompts the user using the message from the input string
to enter a number, when the user passes a string representing a number
the function checks to see if the string actually represents a double
before returning it as a double variable if true. if false the function
will repeat and prompt the user for a new number.
*/
double MovingDetection::readDouble(string message){
	char numbers[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.' };
	int conti = 1;
	string input;

	//A while loop that keeps looping until user enters a double number
	while (conti == 1){
		conti = 0;
		input = readString(message);

		//
		for (int i = 0; i < input.size(); ++i){
			int isNum = 0;
			int isDeci = 0;
			for (int j = 0; j < 11; ++j){
				if (input[i] == numbers[j]){
					isNum = 1;
					if (input[i] == numbers[10]){
						++isDeci;
						if (input.size() == 1){
							isNum = 0;
						}
					}
				}
			}

			//
			if (isNum == 0 || isDeci > 1){
				conti = 1;
			}
		}

		//
		if (conti == 1){
			cout << "input was not a number\n";
		}
	}

	//
	double value = atof(input.c_str());
	return value;
}


/*
readInt
inputs: string representing a message to be displayed
outputs: an int representing the number the user passes
description: this function uses the readDouble method to prompt the user
to enter an integer and return the integer the user passes
*/
int MovingDetection::readInt(string message){
	int result = readDouble(message);
	return result;
}


/*
getBackground
inputs: string representing the address of a video file
outputs: none
description:get the static background of a video file
*/
void MovingDetection::getBackground(string address){

	//load the image from the address param as either an RGB or greyscale image
	cv::VideoCapture capSrc(address);
	long totalFrameNumber;
	double fps;
	long frameToStart = 300;

	//Check to see if the image loaded correctly if not return
	if (!capSrc.isOpened()){
		cout << "Error: Video cannot be loaded." << endl;
		return;
	}
	else {
		//std::string dirAddress = "D:\\ECE698_Proj\\test_video_library\\";
		//SetCurrentDirectoryA(dirAddress.c_str());
		//mkdir(videoname.c_str());
		//std::string dirAddress2 = "D:\\ECE698_Proj\\test_video_library\\" + videoname + "\\";
		//SetCurrentDirectoryA(videoname.c_str());

		totalFrameNumber = capSrc.get(CV_CAP_PROP_FRAME_COUNT);
		fps = capSrc.get(CV_CAP_PROP_FPS);
		long width = capSrc.get(CV_CAP_PROP_FRAME_WIDTH);
		long height = capSrc.get(CV_CAP_PROP_FRAME_HEIGHT);
		cout << width << " * " << height << ", " << fps << " FPS, " << totalFrameNumber << " frames totally." << endl;
	}
	cv::Mat frame;
	cv::Mat dst;
	cv::Mat smallFrame;
	double alpha = .01;
	double beta;

//	cv::namedWindow("Accumulated Frame", CV_WINDOW_KEEPRATIO);
//	cv::namedWindow("Original", CV_WINDOW_KEEPRATIO);

	capSrc.read(dst);
	capSrc.set(CV_CAP_PROP_POS_FRAMES, fps);
	double stopwatch = 1;
	cout << "Total length is " << totalFrameNumber / fps << "\nTotal Frame is " << totalFrameNumber << endl;
        int count = 0;
	while (count < 100){
                count++;
		bool bSuccess = capSrc.read(frame);
		if (!bSuccess)
		{
			cout << "Cannot read more frame from video file" << endl;
			break;
		}

		//
		if (alpha < 0.001){
			alpha = 0.001;
		}
		beta = 1 - alpha;

		//C++: void cv::addWeighted(InputArray src1, double alpha, InputArray src2, double beta, double gamma, OutputArray dst, int dtype=-1)
		//dst = src1*alpha + src2*beta + gamma;
		cv::addWeighted(frame, alpha, dst, beta, 0.0, dst);
		alpha = alpha / 1.0001;
		background = dst;

		//Display the size-reduced process
		cv::pyrDown(frame, smallFrame, cv::Size(frame.cols / 2, frame.rows / 2));

//		cv::resizeWindow("Original", 640, 360);
//		cv::moveWindow("Accumulated Frame", 0, 0);
//		cv::moveWindow("Original", 1095, 0);

//		cv::imshow("Accumulated Frame", dst);
//		cv::imshow("Original", smallFrame);

		stopwatch++;

		//For accelerating the accumulation process
		if (cv::waitKey(1) == 27)	{
			//if ((cv::waitKey(1) == 27) || (stopwatch >= totalFrameNumber / fps))	{
			break;
		}
		/*
		if (cv::waitKey(1) == 27)	{
		break;
		}*/
	}
	if (videoname.empty()){
		saveBackgroundImage("background.jpg");
	}
	else{
		saveBackgroundImage(videoname + ".jpg");
	}
	capSrc.release();
	cv::destroyAllWindows();
}



/*
filterTotalBackground
inputs: a video frame
outputs: the thersholded absolute difference between the frame and its
static background
description: finds the foreground objects of a frame image using a static
background image
*/
cv::Mat MovingDetection::filterTotalBackground(cv::Mat frame){

	cv::Mat diff;
	cv::Mat result;

	//if background class variable is empty read in the background image file
	if (background.empty()){
		if (videoname.empty()){
			background = cv::imread("background.jpg", CV_LOAD_IMAGE_UNCHANGED);
		}
		else{
			background = cv::imread(videoname + ".jpg", CV_LOAD_IMAGE_UNCHANGED);
		}
	}

	//take the difference between the frame image and the background: dst = frame - background
	//C++: void absdiff(InputArray src1, InputArray src2, OutputArray dst)
	cv::absdiff(frame, background, diff);

	//Applies a fixed-level cv::threshold to each array element.
	//C++: double cv::threshold(InputArray src, OutputArray dst, double thresh, double maxval, int type)
	cv::threshold(diff, result, 60, 255, CV_THRESH_BINARY);
	return result;
}


/*
saveBackgroundImage
inputs: a string representing the target address
outputs: none
description: saves the image stored in the class variable background to the address the user passes.
*/
void MovingDetection::saveBackgroundImage(string address){

	//C++: bool imwrite(const string& filename, InputArray img, const vector<int>& params=vector<int>() )
	bool bSuccess = cv::imwrite(address, background);
	if (!bSuccess){
		cout << "Error: Failed to save the image" << endl;
	}
}



/*
readString
inputs: string representing a message to be displayed
outputs: a string representing that the user passes
description: prompts the user using the message from the input string
to enter a string
*/
string MovingDetection::readString(string message){
	string input;
	cout << message;
	cin >> input;
	cout << "\n";
	return input;
}



/*
filterMovingObjects2
inputs: A string representing the address of the video file,
outputs: none
description:
*/
//void MovingDetection::filterMovingObjects(string address1, string address2){
void MovingDetection::filterMovingObjects(string address1){

	// Blob Tracking Algorithm
	BlobTracking* blobTracking;
	blobTracking = new BlobTracking;

	// Vehicle Counting Algorithm
	VehicleCouting* vehicleCouting;
	vehicleCouting = new VehicleCouting;

	//
	cv::Mat info;
	cv::Mat frame;
	cv::Mat back;
	cv::Mat back2;

	//
	cv::Mat fore;
	cv::Mat fore2;
	cv::Mat dst;

	cv::Mat smallFrame;
	cv::Mat smallDst;

	//
	vector<vector<cv::Point> > contours;
	cv::Mat smallImg;
	if (videoname.empty()){
		cv::Mat img = cv::imread("background.jpg", CV_LOAD_IMAGE_UNCHANGED);
		if (img.empty()){
			getBackground(address1);
		}
	}
	else{
		cv::Mat img = cv::imread(videoname + ".jpg", CV_LOAD_IMAGE_UNCHANGED);
		if (img.empty()){
			getBackground(address1);
		}
	}

	//load video from string address
	cv::VideoCapture capSrc(address1);
	//get the frame rate of the video
	double fps = capSrc.get(CV_CAP_PROP_FPS);
	//load the first video frame
	capSrc.read(info);
	//get the width of frames of the video
	double dWidth = info.cols;
	//get the height of frames of the video
	double dHeight = info.rows;

	//background subtractor for the frames
	//http://docs.opencv.org/2.4/modules/video/doc/motion_analysis_and_object_tracking.html#backgroundsubtractormog2
	cv::BackgroundSubtractorMOG2 bg = cv::BackgroundSubtractorMOG2();
	bg.set("history", 1000);
	bg.set("nmixtures", 3);
	bg.set("backgroundRatio", 0.7);
	bg.set("detectShadows", false);

	//background subtractor for the filterTotalBackground results
	cv::BackgroundSubtractorMOG2 bg2 = cv::BackgroundSubtractorMOG2();
	bg2.set("history", 1000);
	bg2.set("nmixtures", 3);
	bg2.set("backgroundRatio", 0.7);
	bg2.set("detectShadows", false);


	//traverse through the video file
	while (1){
                clock_t begin = clock();
		//read in the next frame in the video
		bool bSuccess = capSrc.read(frame);
		//if not success, break loop
		if (!bSuccess)
		{
			cout << "Cannot read the frame from video file" << endl;
			break;
		}

		//find the moving objects in results of the filterTotalBackground function
		cv::Mat total = filterTotalBackground(frame);

		//Computes a foreground mask.
		//C++: void cv::BackgroundSubtractorMOG2::operator()(InputArray image, OutputArray fgmask, double learningRate=-1)
		bg.operator()(total, fore);


		//Computes a background image.
		//C++: void BackgroundSubtractor::getBackgroundImage(OutputArray backgroundImage) const¶
		bg.getBackgroundImage(back);

		//cv::pyrDown(fore,smallFore,cv::Size(fore.cols/2, fore.rows/2));
		//cv::imshow("Fore", smallFore);


		//find the moving objects in the frame and cv::erode the image
		bg2.operator()(frame, fore2);
		bg2.getBackgroundImage(back2);
		cv::erode(fore2, fore2, cv::Mat());

		//cv::Mat smallfore2;
		//cv::pyrDown(fore2, smallfore2, cv::Size(fore2.cols / 2, fore2.rows / 2));
		//cv::imshow("Fore2", smallfore2);


		//combine the two images and cv::blur  and cv::threshold the results
		dst = fore & fore2;
		cv::blur(dst, dst, cv::Size(5, 5));
		cv::blur(dst, dst, cv::Size(5, 5));
		cv::blur(dst, dst, cv::Size(5, 5));
		cv::threshold(dst, dst, 50, 255, cv::THRESH_BINARY);

		cv::pyrDown(dst, smallDst, cv::Size(dst.cols / 2, dst.rows / 2));

		// Perform blob tracking
		blobTracking->process(frame, dst, img_blob);
		// Perform vehicle counting
		vehicleCouting->setInput(img_blob);
		vehicleCouting->setTracks(blobTracking->getTracks());
		vehicleCouting->process();

		/*
		//find all the object contours in the image
		findContours(dst, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);



		//Approxicv::Mate contours to polygons and get bounding rects
		vector<vector<cv::Point>> contours_poly(contours.size());
		vector<cv::Rect> boundRect(contours.size());
		for (int i = 0; i < contours.size(); i++){
		cv::approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(cv::Mat(contours_poly[i]));
		//cv::Rect intersect = boundRect[i] & streetcurb;

		}

		// Draw polygonal contour and bonding rects
		for (int i = 0; i < contours.size(); i++){
		//Choose the color pf object detection box
		//Scalar(B,G,R[,alpha])
		cv::Scalar color = cv::Scalar(255, 0, 0); //Blue
		drawContours(frame, contours_poly, i, color, 1, 8, vector<cv::Vec4i>(), 0, cv::Point());
		rectangle(frame, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
		box = cv::Rect(boundRect[i].tl(), boundRect[i].br());
		}
		*/


                clock_t end = clock();
                double elapsed_time = (end - begin) / (CLOCKS_PER_SEC/1000);
                cout << elapsed_time << endl;

//		cv::imshow("Foreground Mask", smallDst);
//		cv::moveWindow("Foreground Mask", 1095, 0);
		if (cv::waitKey(1) >= 0){
			break;
		}
	}

	//
	delete vehicleCouting;
	delete blobTracking;

	capSrc.release();
	cv::destroyAllWindows();
}

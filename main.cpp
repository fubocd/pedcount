//OpenCV 2.4.13
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

#include "opencv2/gpu/gpu.hpp"                                                                                       
#include "opencv2/video/background_segm.hpp"
#include "opencv2/legacy/legacy.hpp"

//
#include <iostream>
#include <opencv/cv.h>

#include "package_bgs/MovingDetection.h"
//#include "package_tracking/BlobTracking.h"
//#include "package_analysis/VehicleCouting.h"


using namespace std;


int main(int argc, const char** argv){
	MovingDetection video = MovingDetection();
	video.menu();
	return 0;
}

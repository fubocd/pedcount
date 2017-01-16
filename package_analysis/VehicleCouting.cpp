#include "VehicleCouting.h"


namespace FAV1
{
	IplImage* img_input1 = 0;
	IplImage* img_input2 = 0;
	int roi_x0 = 0;
	int roi_y0 = 0;
	int roi_x1 = 0;
	int roi_y1 = 0;
	int numOfRec = 0;
	int startDraw = 0;
	bool roi_defined = false;
	bool use_roi = true;
	void VehicleCouting_on_mouse(int evt, int x, int y, int flag, void* param)
	{
		if (!use_roi)
			return;

		if (evt == CV_EVENT_LBUTTONDOWN)
		{
			if (!startDraw)
			{
				roi_x0 = x;
				roi_y0 = y;
				startDraw = 1;
			}
			else
			{
				roi_x1 = x;
				roi_y1 = y;
				startDraw = 0;
				roi_defined = true;
			}
		}

		if (evt == CV_EVENT_MOUSEMOVE && startDraw)
		{
			//redraw ROI selection
			img_input2 = cvCloneImage(img_input1);
			cvLine(img_input2, cvPoint(roi_x0, roi_y0), cvPoint(x, y), CV_RGB(255, 0, 255), 2);
			cvShowImage("VehicleCouting", img_input2);
			cvReleaseImage(&img_input2);
		}
	}
}

VehicleCouting::VehicleCouting() : firstTime(true), showOutput(true), key(0), countAB(0), countBA(0), showAB(0)
{
	std::cout << "VehicleCouting()" << std::endl;
}
//--------------------------------------------------------------------------------
VehicleCouting::~VehicleCouting()
{
	std::cout << "~VehicleCouting()" << std::endl;
}
//--------------------------------------------------------------------------------
void VehicleCouting::setInput(const cv::Mat &i)
{
	//i.copyTo(img_input);
	img_input = i;
}
//--------------------------------------------------------------------------------
void VehicleCouting::setTracks(const cvb::CvTracks &t)
{
	tracks = t;
}
//--------------------------------------------------------------------------------
VehiclePosition VehicleCouting::getVehiclePosition(const CvPoint2D64f centroid, cvb::CvID temp){
	VehiclePosition vehiclePosition = VP_NONE;
	cvb::CvID trackID = temp;

	// y = kx + b -> kx - y + b = 0
	slope = 1.0 * (FAV1::roi_y1 - FAV1::roi_y0) / (FAV1::roi_x1 - FAV1::roi_x0);
	intercept = FAV1::roi_y0 - slope * FAV1::roi_x0;

	// fx = distance between line and points
	double fx = (slope * centroid.x - centroid.y + intercept) / sqrt(slope * slope + 1);
	bool close_to_curb = (fx <5) && (fx >-5) && (centroid.x > (FAV1::roi_x0 - 2)) && (centroid.x < (FAV1::roi_x1 + 2));
	if (close_to_curb){
		if (fx < 0){
			// C++: void putText(Mat& img, const string& text, Point org, int fontFace, double fontScale, 
			//							Scalar color, int thickness=1, int lineType=8, bool bottomLeftOrigin=false )
			std::cout << "id = " << trackID << " --- A ---  (" << centroid.x << "," << centroid.y << ")     Distance to curb = " << fx << std::endl;
			//cv::putText(img_input, "STATE: A", cv::Point(centroid.x, img_h - 50), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
			vehiclePosition = VP_A;
		}
		else if (fx > 0){
			std::cout << "id = " << trackID << " --- B ---  (" << centroid.x << "," << centroid.y << ")     Distance to curb = " << fx << std::endl;
			//cv::putText(img_input, "STATE: B", cv::Point(centroid.x, img_h - 50), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
			vehiclePosition = VP_B;
		}
	}
	return vehiclePosition;
}
//--------------------------------------------------------------------------------
void VehicleCouting::process()
{
	if (img_input.empty()){
		return;
	}

	img_w = img_input.size().width;
	img_h = img_input.size().height;

	loadConfig();

	//--------------------------------------------------------------------------

	if (FAV1::use_roi == true && FAV1::roi_defined == false && firstTime == true)
	{
		do
		{
			cv::putText(img_input, "Please, set the counting line", cv::Point(1100, 600), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
			cv::imshow("PedestrianCount", img_input);
			FAV1::img_input1 = new IplImage(img_input);
			cvSetMouseCallback("VehicleCouting", FAV1::VehicleCouting_on_mouse, NULL);
			key = cvWaitKey(0);
			delete FAV1::img_input1;

			if (FAV1::roi_defined)
			{
				std::cout << "Counting line defined (" << FAV1::roi_x0 << "," << FAV1::roi_y0 << "," << FAV1::roi_x1 << "," << FAV1::roi_y1 << ")" << std::endl;
				break;
			}
			else
				std::cout << "Counting line undefined!" << std::endl;
		} while (1);
	}
	// Default, draw the line from XML
	if (FAV1::use_roi == true && FAV1::roi_defined == true){
		cv::line(img_input, cv::Point(FAV1::roi_x0, FAV1::roi_y0), cv::Point(FAV1::roi_x1, FAV1::roi_y1), cv::Scalar(0, 0, 255), 3);
	}

	bool ROI_OK = false;

	if (FAV1::use_roi == true && FAV1::roi_defined == true)
		ROI_OK = true;

	if (ROI_OK)
	{
		laneOrientation = LO_NONE;

		if (abs(FAV1::roi_x0 - FAV1::roi_x1) < abs(FAV1::roi_y0 - FAV1::roi_y1))
		{
			if (!firstTime)
				//cv::putText(img_input, "Vertical", cv::Point(1100, img_h - 140), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
				laneOrientation = LO_HORIZONTAL;
			cv::putText(img_input, "(A)", cv::Point(FAV1::roi_x1 + 3, FAV1::roi_y1 + 25), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
			cv::putText(img_input, "(B)", cv::Point(FAV1::roi_x1 + 5, FAV1::roi_y1 - 15), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
		}

		if (abs(FAV1::roi_x0 - FAV1::roi_x1) > abs(FAV1::roi_y0 - FAV1::roi_y1))
		{
			if (!firstTime)
				//cv::putText(img_input, "Horizontal", cv::Point(1100, img_h - 140), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
				laneOrientation = LO_VERTICAL;
			cv::putText(img_input, "(A)", cv::Point(FAV1::roi_x1 + 3, FAV1::roi_y1 + 25), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
			cv::putText(img_input, "(B)", cv::Point(FAV1::roi_x1 + 5, FAV1::roi_y1 - 15), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
		}
	}

	//--------------------------------------------------------------------------

	int it_indicator = 0;
	// iterator: std::pair;
	// iterator.first:	CvID;
	// iterator.second:	CvTrack;
	// std::map<CvID, CvTrack *> CvTracks --- brief List of tracks.(begin, end)
	for (std::map<cvb::CvID, cvb::CvTrack*>::iterator it = tracks.begin(); it != tracks.end(); it++)
	{
		//std::cout << (*it).first << " => " << (*it).second << std::endl;
		cvb::CvID id = (*it).first;
		// struct CvTrack: id
		cvb::CvTrack* track = (*it).second;

		CvPoint2D64f centroid = track->centroid;

		/*it_indicator++;
		std::cout << "---------------------------------------------------------------" << std::endl;
		std::cout << it_indicator << ")id:" << id << " (" << centroid.x << "," << centroid.y << ")" << std::endl;
		std::cout << "track->active:" << track->active << std::endl;
		std::cout << "track->inactive:" << track->inactive << std::endl;
		std::cout << "track->lifetime:" << track->lifetime << std::endl;
		*/
		//----------------------------------------------------------------------------

		if (points.count(id) > 0)
		{
			// Main List of id-location map
			std::map<cvb::CvID, std::vector<CvPoint2D64f> >::iterator it2 = points.find(id);
			std::vector<CvPoint2D64f> centroids = it2->second;

			std::vector<CvPoint2D64f> centroids2;
			// During the blue box period
			if (track->inactive == 0 && centroids.size() < 30)
			{
				// std::vector::push_back
				// Appends the given element value to the end of the container.
				centroids2.push_back(centroid);

				// iterate through those blob locations
				for (std::vector<CvPoint2D64f>::iterator it3 = centroids.begin(); it3 != centroids.end(); it3++)
				{

					cvb::CvID id2 = (*it2).first;
					// Push points back in list "centroids2"
					centroids2.push_back(*it3);
					//Real-time printing the id, location)
					std::string label = "id=" + std::to_string(int(id2)) + " (" + std::to_string(int(centroid.x)) + "," + std::to_string(int(centroid.y)) + ")";
					//std::string label = " (" + std::to_string(int(centroid.x)) + "," + std::to_string(int(centroid.y)) + ")";
					// args: Input, Label, Print_Location, Font, Size, Color, Thinkness
					cv::putText(img_input, label, cv::Point(int(centroid.x) + 20, int(centroid.y) - 20), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 255), 2);
					//cv::circle(img_input, cv::Point((*it3).x, (*it3).y), 3, cv::Scalar(255, 0, 0), 2);
				}

				/* std::map<cvb::CvID, std::vector<CvPoint2D64f> > points */
				points.erase(it2);
				points.insert(std::pair<cvb::CvID, std::vector<CvPoint2D64f>>(id, centroids2));
			}
			else
			{
				points.erase(it2);
			}
		}
		else
		{
			if (track->inactive == 0)
			{
				std::vector<CvPoint2D64f> centroids;
				centroids.push_back(centroid);
				points.insert(std::pair<cvb::CvID, std::vector<CvPoint2D64f>>(id, centroids));
			}
		}

		//----------------------------------------------------------------------------

		if (track->inactive == 0)
		{
			if (positions.count(id) > 0)
			{
				std::map<cvb::CvID, VehiclePosition>::iterator it2 = positions.find(id);
				VehiclePosition old_position = it2->second;
				VehiclePosition current_position = getVehiclePosition(centroid, id);

				slope = 1.0 * (FAV1::roi_y1 - FAV1::roi_y0) / (FAV1::roi_x1 - FAV1::roi_x0);
				intercept = FAV1::roi_y0 - slope * FAV1::roi_x0;

				double fx = slope * centroid.x + intercept;
				std::string xing = "(" + std::to_string(int(centroid.x)) + "," + std::to_string(int(centroid.y)) + ")";

				// If the position changed
				if (current_position != old_position){
					if ((centroid.x >= FAV1::roi_x0) && (centroid.x <= FAV1::roi_x1)){
						if (old_position == VP_A && current_position == VP_B){
							countAB++;
							cv::circle(img_input, cv::Point(centroid.x, centroid.y), 3, cv::Scalar(255, 255, 255), 2);
							cv::putText(img_input, xing, cv::Point(1100, img_h - 200), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 255), 2);
							std::cout << id << std::endl;
							std::cout << slope << "--" << intercept << "   A->B   " << centroid.y << "<" << fx << std::endl;
							std::cout << "" << std::endl;
							// Save the crossing snapshots
							std::string address = std::to_string(int(id)) + "AB" + xing + ".jpg";
							bool bSuccess = cv::imwrite(address, img_input);
							if (!bSuccess){
								std::cout << "Error: Failed to save the image" << std::endl;
							}
						}
						if (old_position == VP_B && current_position == VP_A){
							countBA++;
							cv::circle(img_input, cv::Point(centroid.x, centroid.y), 3, cv::Scalar(0, 0, 0), 2);
							cv::putText(img_input, xing, cv::Point(1100, img_h - 200), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 255), 2);
							std::cout << id << std::endl;
							std::cout << slope << "--" << intercept << "   B->A   " << centroid.y << ">=" << fx << std::endl;
							std::cout << "" << std::endl;
							// Save the crossing snapshots
							std::string address = std::to_string(int(id)) + "BA" + xing + ".jpg";
							bool bSuccess = cv::imwrite(address, img_input);
							if (!bSuccess){
								std::cout << "Error: Failed to save the image" << std::endl;
							}
						}
						positions.erase(positions.find(id));
					}
				}
			}
			else if (positions.count(id) <= 0)
			{

				VehiclePosition vehiclePosition = getVehiclePosition(centroid, id);

				if (vehiclePosition != VP_NONE)
					positions.insert(std::pair<cvb::CvID, VehiclePosition>(id, vehiclePosition));
			}
		}
		// else (track->inactive != 0)
		/*
		else
		{
		if(positions.count(id) > 0)
		positions.erase(positions.find(id));
		}
		*/

		//cv::waitKey(0);
	}

	//--------------------------------------------------------------------------

	std::string countABstr = "A>>B: " + std::to_string(countAB);
	std::string countBAstr = "B>>A: " + std::to_string(countBA);

	if (showAB == 0)
	{
		//A->B and B->A are both displayed.
		cv::putText(img_input, countABstr, cv::Point(1120, img_h - 140), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
		cv::putText(img_input, countBAstr, cv::Point(1120, img_h - 100), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
		cv::putText(img_input, "Northeast", cv::Point(1100, 20), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
		cv::putText(img_input, "Southwest", cv::Point(0, img_h - 20), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);

	}
	// Only Display A->B
	if (showAB == 1){
		cv::putText(img_input, countABstr, cv::Point(1100, img_h - 120), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
	}
	// Only Display B->A
	if (showAB == 2){
		cv::putText(img_input, countBAstr, cv::Point(1100, img_h - 80), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
	}
	if (showOutput){
		cv::imshow("PedCount", img_input);
		cv::moveWindow("PedCount", 0, 0);
	}
	if (firstTime){
		saveConfig();
	}
	firstTime = false;
}
//--------------------------------------------------------------------------------
void VehicleCouting::saveConfig()
{
	CvFileStorage* fs = cvOpenFileStorage("config/PedCount.xml", 0, CV_STORAGE_WRITE);

	cvWriteInt(fs, "showOutput", showOutput);
	cvWriteInt(fs, "showAB", showAB);

	cvWriteInt(fs, "fav1_use_roi", FAV1::use_roi);
	cvWriteInt(fs, "fav1_roi_defined", FAV1::roi_defined);
	cvWriteInt(fs, "fav1_roi_x0", FAV1::roi_x0);
	cvWriteInt(fs, "fav1_roi_y0", FAV1::roi_y0);
	cvWriteInt(fs, "fav1_roi_x1", FAV1::roi_x1);
	cvWriteInt(fs, "fav1_roi_y1", FAV1::roi_y1);

	cvReleaseFileStorage(&fs);
}
//--------------------------------------------------------------------------------
void VehicleCouting::loadConfig()
{
	CvFileStorage* fs = cvOpenFileStorage("config/PedCount.xml", 0, CV_STORAGE_READ);
	/* int cvReadIntByName(const CvFileStorage* fs, const CvFileNode* map, const char* name, int default_value=0 ) */
	showOutput = cvReadIntByName(fs, 0, "showOutput", true);
	showAB = cvReadIntByName(fs, 0, "showAB", 1);

	FAV1::use_roi = cvReadIntByName(fs, 0, "fav1_use_roi", true);
	FAV1::roi_defined = cvReadIntByName(fs, 0, "fav1_roi_defined", false);
	FAV1::roi_x0 = cvReadIntByName(fs, 0, "fav1_roi_x0", 0);
	FAV1::roi_y0 = cvReadIntByName(fs, 0, "fav1_roi_y0", 0);
	FAV1::roi_x1 = cvReadIntByName(fs, 0, "fav1_roi_x1", 0);
	FAV1::roi_y1 = cvReadIntByName(fs, 0, "fav1_roi_y1", 0);


	cvReleaseFileStorage(&fs);
}

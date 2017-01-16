#pragma once

#include <iostream>
#include <string>
#include <opencv/cv.h>
#include <opencv2/highgui.hpp>
#include <math.h> 
#include "../package_tracking/cvblob/cvblob.h"

enum LaneOrientation
{
  LO_NONE       = 0,
  LO_HORIZONTAL = 1,
  LO_VERTICAL   = 2
};

enum VehiclePosition
{
  VP_NONE = 0,
  VP_A  = 1,
  VP_B  = 2
};

class VehicleCouting
{
private:
  bool firstTime;
  bool showOutput;
  int key;
  cv::Mat img_input;

  //Map of id and struct CvTrack(All info) 
  cvb::CvTracks tracks;

  std::map<cvb::CvID, std::vector<CvPoint2D64f> > points;
  std::map<cvb::CvID, VehiclePosition> positions;

  LaneOrientation laneOrientation;
  long countAB;
  long countBA;
  int img_w;
  int img_h;
  int showAB;
  double slope;
  double intercept;

public:
  VehicleCouting();
  ~VehicleCouting();

  int it_indicator;
  void setInput(const cv::Mat &i);
  void setTracks(const cvb::CvTracks &t);
  void process();

private:
	VehiclePosition getVehiclePosition(const CvPoint2D64f centroid, cvb::CvID id);

  void saveConfig();
  void loadConfig();
};

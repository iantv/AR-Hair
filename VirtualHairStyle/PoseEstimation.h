#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
//#include "glut.h"

#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/tracking.hpp>
#include <opencv/cxcore.h>

using namespace cv;

extern Ptr<Tracker> faceTracker;
extern Rect2d faceRect;
extern bool isDetectedFace;

extern Ptr<Tracker> noseTracker;
extern Rect2d noseRect;
extern bool isDetectedNose;

extern Ptr<Tracker> leftEyeTracker;
extern Rect2d leftEyeRect;
extern bool isDetectedLeftEye;

extern Ptr<Tracker> rightEyeTracker;
extern Rect2d rightEyeRect;
extern bool isDetectedRightEye;

extern Ptr<Tracker> mouthTracker;
extern Rect2d mouthRect;
extern bool isDetectedMouth;

extern bool isDetectedEyes;
int load_cascades();
void detect2dpoints(cv::Mat im);
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
#include "BaseDetector.h"

extern unsigned int frameCount;

void detect2dpoints(cv::Mat im);
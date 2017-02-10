#pragma once
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
//#include "glut.h"

#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

int load_cascades();
void detect2dpoints(cv::Mat im);
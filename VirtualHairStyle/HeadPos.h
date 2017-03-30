#pragma once
#include <opencv\cxcore.h>
#include <vector>
#include "glm.h"
#include <opencv/cv.h>
#include "OGL_OCV_common.h"

extern std::vector<double> rv, tv;
extern cv::Mat rvec, tvec;
extern cv::Mat camMatrix;
extern GLMmodel* head_obj;
extern cv::Mat op;
extern OpenCVGLTexture imgTex, imgWithDrawing;

extern VideoCapture capture;

void loadNext();
int sample(int argc, char** argv);

void init_opengl(int argc, char** argv);
void solveHeadPos(Mat& ip, Mat& img);
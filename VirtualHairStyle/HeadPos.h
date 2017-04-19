#pragma once
#include <opencv\cxcore.h>
#include <vector>
#include "glm.h"
#include <opencv/cv.h>
#include "OGL_OCV_common.h"

extern std::vector<double> rv, tv;
extern cv::Mat rvec, tvec;
extern cv::Mat cam_mat;
extern GLMmodel* head_obj;
extern cv::Mat op;
extern OpenCVGLTexture imgTex;

extern cv::VideoCapture capture;
extern cv::Point2d camera_center;

void init_opengl(int argc, char** argv);
void solve_head_pos(cv::Mat& ip, cv::Mat& img);
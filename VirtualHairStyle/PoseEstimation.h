#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/tracking.hpp>
#include <opencv/cxcore.h>
#include "BaseDetector.h"

#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>

extern dlib::frontal_face_detector detector;
extern dlib::shape_predictor pose_model;
void detect_2d_points(cv::Mat &img);

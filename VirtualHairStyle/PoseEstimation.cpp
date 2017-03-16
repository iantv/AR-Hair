#include "PoseEstimation.h"
#include <Windows.h>

#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#include <stdio.h>
#include <iostream>

enum face_objects_t { NOSE, LEFTEYE, RIGHTEYE, MOUTH };

using namespace std;

static void draw_point(cv::Mat im, cv::Point2d point, Scalar color)
{ /* Temp function */
	circle(im, point, 3, color, -1);
}

cv::Point2d get_center_point(Rect2d candidate)
{
	return cv::Point2d(candidate.x + candidate.width*0.5,
		candidate.y + candidate.height*0.5);
}

static void add_best_point(face_objects_t obj_as_idx, cv::Mat im, Rect2d face, Rect2d obj,
	vector<cv::Point2d> &image_points, Scalar color = Scalar(255, 0, 0))
{
	cv::Point2d center = get_center_point(obj);
	//rectangle(im, obj, color, 2);
	image_points[obj_as_idx] = center;
	draw_point(im, center, color);
}

#define N_POINTS 4

vector<cv::Point2d> image_points(N_POINTS);
static vector<bool> found_point(N_POINTS);

void calcMatrix(Mat im);
unsigned int frameCount;

void detect2dpoints(Mat im)
{
	Mat frame_gray;
	Mat faceROI;
	
	cvtColor(im, frame_gray, CV_BGR2GRAY);
	if (!faceDetector.try_detect(im, frame_gray, Size(im.rows / 5.0, im.rows / 5.0))) {
		cv::imshow("Output", im);
		return;
	}
	Rect2d faceRect = faceDetector.getDetectedRect(); 
	faceROI = frame_gray(faceRect);
	rectangle(im, faceRect, Scalar(255, 0, 255), 2);
	
	if (!noseDetector.try_detect(im, faceROI, faceRect,
		Size(faceRect.width*0.15, faceRect.height*0.15), Size(faceRect.width*0.3, faceRect.height*0.3))) {
		cv::imshow("Output", im);
		return;
	}
	add_best_point(NOSE, im, faceRect, noseDetector.getDetectedRect(), image_points);
	
	if (!eyesDetector.try_detect_pair(im, faceROI, faceRect,
		Size(faceRect.width*0.15, faceRect.height*0.15), Size(faceRect.width*0.3, faceRect.height*0.3))) {
		cv::imshow("Output", im);
		return;
	}
	add_best_point(LEFTEYE, im, faceRect, eyesDetector.getLeftDetectedRect(), image_points, Scalar(255, 255, 0));
	add_best_point(RIGHTEYE, im, faceRect, eyesDetector.getRightDetectedRect(), image_points, Scalar(0, 255, 0));

	if (!mouthDetector.try_detect(im, faceROI, faceRect,
		Size(faceRect.width*0.15, faceRect.height*0.15), Size(faceRect.width*0.3, faceRect.height*0.3))) {
		cv::imshow("Output", im);
		return;
	}
	add_best_point(MOUTH, im, faceRect, mouthDetector.getDetectedRect(), image_points, Scalar(0, 255, 255));
	//cv::imshow("Output", im);
	calcMatrix(im);
}

Mat rotation_vector;
Mat translation_vector;

static vector<Point3d> nose_end_point3D;
static vector<Point2d> nose_end_point2D;

static double length = 200.0;
static bool camera_is_ready = false;
static vector<Point3d> model_points;

static double focal_length;
static Point2d center;
static Mat camera_matrix;
static Mat dist_coeffs;

void calcMatrix(Mat im) {
	std::vector<cv::Point2d> img_points_new;
	if (model_points.empty()) {
		model_points.push_back(Point3f(-0.0697709f, 18.6015f, 87.9695f)); // nose
		model_points.push_back(Point3f(-36.9522f, 39.3518f, 47.1217f)); // lefteye
		model_points.push_back(Point3f(35.446f, 38.4345f, 47.6468f)); // righteye
		model_points.push_back(Point3f(-0.0697709f, -29.2935f, 72.7329f)); // mouth
	}
	img_points_new.push_back(image_points[NOSE]);
	img_points_new.push_back(image_points[LEFTEYE]);
	img_points_new.push_back(image_points[RIGHTEYE]);
	img_points_new.push_back(image_points[MOUTH]);

	if (!camera_is_ready) {
		focal_length = MAX(im.cols, im.rows); // Approximate focal length.
		center = cv::Point2d(im.cols / 2, im.rows / 2);
		camera_matrix = (cv::Mat_<double>(3, 3) << focal_length, 0, center.x,
			0, focal_length, center.y,
			0, 0, 1);
		dist_coeffs = cv::Mat::zeros(4, 1, cv::DataType<double>::type); // Assuming no lens distortion
		camera_is_ready = true;
	}

	if (cv::solvePnP(model_points, img_points_new, camera_matrix, dist_coeffs, rotation_vector, translation_vector, false, CV_ITERATIVE)) {
		cout << "ok!";
	} else {
		cv::imshow("Output", im);
		return;
	}

	if (nose_end_point3D.empty()) {
		nose_end_point3D.push_back(Point3d(length, 0, 0));
		nose_end_point3D.push_back(Point3d(-length, 0, 0));
		nose_end_point3D.push_back(Point3d(0, length, 0));
		nose_end_point3D.push_back(Point3d(0, -length, 0));
		nose_end_point3D.push_back(Point3d(0, 0, length));
		nose_end_point3D.push_back(Point3d(0, 0, -length));
	}

	projectPoints(nose_end_point3D, rotation_vector, translation_vector, camera_matrix, dist_coeffs, nose_end_point2D);

	cv::line(im, image_points[NOSE], nose_end_point2D[0], cv::Scalar(255, 0, 0), 2);
	cv::line(im, image_points[NOSE], nose_end_point2D[2], cv::Scalar(0, 255, 0), 2);
	cv::line(im, image_points[NOSE], nose_end_point2D[4], cv::Scalar(0, 0, 255), 2); // z
	/*
	cv::line(im, nose_end_point2D[0], nose_end_point2D[1], cv::Scalar(255, 0, 0), 2);
	cv::line(im, nose_end_point2D[2], nose_end_point2D[3], cv::Scalar(0, 255, 0), 2);
	cv::line(im, nose_end_point2D[4], nose_end_point2D[5], cv::Scalar(0, 0, 255), 2); // z
	*/

	cv::imshow("Output", im);
}


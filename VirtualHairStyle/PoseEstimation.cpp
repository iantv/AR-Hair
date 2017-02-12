#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv/cxcore.h>

#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

//#include "OGL_OCV_common.h"
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#include <stdio.h>
#include <iostream>

enum face_objects_t { NOSE, LEFTEYE, RIGHTEYE, MOUTH };

using namespace cv;
using namespace std;

String face_cascade_name = "haarcascades/haarcascade_frontalface_default.xml";
String eyes_cascade_name = "haarcascades/haarcascade_eye_tree_eyeglasses.xml";
String nose_cascade_name = "haarcascades/haarcascade_mcs_nose.xml";
String lefteye_cascade_name = "haarcascades/haarcascade_mcs_lefteye.xml";
String righteye_cascade_name = "haarcascades/haarcascade_mcs_righteye.xml";
String eyepair_big_cascade_name = "haarcascades/haarcascade_mcs_eyepair_big.xml";
String mouth_cascade_name = "haarcascades/haarcascade_mcs_mouth.xml";

CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
CascadeClassifier lefteye_cascade;
CascadeClassifier righteye_cascade;
CascadeClassifier nose_cascade;
CascadeClassifier eyepair_big_cascade;
CascadeClassifier mouth_cascade;

int load_cascades() {
	if (!face_cascade.load(face_cascade_name)) { printf("--(!)Error loading\n"); return -1; };
	if (!eyes_cascade.load(eyes_cascade_name)) { printf("--(!)Error loading\n"); return -1; };
	if (!nose_cascade.load(nose_cascade_name)) { printf("--(!)Error loading\n"); return -1; };
	if (!lefteye_cascade.load(lefteye_cascade_name)) { printf("--(!)Error loading\n"); return -1; };
	if (!righteye_cascade.load(righteye_cascade_name)) { printf("--(!)Error loading\n"); return -1; };
	if (!eyepair_big_cascade.load(eyepair_big_cascade_name)) { printf("--(!)Error loading\n"); return -1; };
	if (!mouth_cascade.load(mouth_cascade_name)) { printf("--(!)Error loading\n"); return -1; };
}

static void draw_point(cv::Mat im, cv::Point2d point, Scalar color) { /* Temp function */
	circle(im, point, 3, color, -1);
}

cv::Point2d get_center_point(Rect face, Rect candidate) {
	return cv::Point2d(face.x + candidate.x + candidate.width*0.5,
		face.y + candidate.y + candidate.height*0.5);
}

static void add_best_point(face_objects_t obj_as_idx, cv::Mat im, Rect face, Rect obj,
	vector<cv::Point2d> &image_points, Scalar color = Scalar(255, 0, 0)) {
	cv::Point2d center = get_center_point(face, obj);
	image_points[obj_as_idx] = center;
	draw_point(im, center, color);
}

#define N_POINTS 4

std::vector<cv::Point2d> image_points(N_POINTS);
static std::vector<bool> found_point(N_POINTS);

bool is_left_eye(Rect eye, Rect face) {
	return eye.x + eye.width*0.5 < face.width*0.5;
}

void add_eye_point(cv::Mat im, Rect face, Rect eye,
	vector<cv::Point2d> &image_points, Scalar color = Scalar(255, 0, 0)) {
	if (eye.y > face.height * 0.5) return;
	if (is_left_eye(eye, face)) {
		found_point[LEFTEYE] = true;
		add_best_point(LEFTEYE, im, face, eye, image_points, Scalar(0, 255, 0));
	}
	else {
		found_point[RIGHTEYE] = true;
		add_best_point(RIGHTEYE, im, face, eye, image_points, Scalar(0, 255, 0));
	}
}

void detect2dpoints(Mat im) {
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor(im, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
	face_cascade.detectMultiScale(frame_gray, faces, 1.5, 6, 0 | CV_HAAR_SCALE_IMAGE, Size(100, 100)/*, Size(300, 300)*/);

	found_point[NOSE] = found_point[LEFTEYE] = found_point[RIGHTEYE] = found_point[MOUTH] = false;
	for (size_t i = 0; i < faces.size(); i++) {
		//Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
		rectangle(im, faces[i], Scalar(255, 0, 255), 2);
		Mat faceROI = frame_gray(faces[i]);

		vector<Rect> nose;
		nose_cascade.detectMultiScale(faceROI, nose, 1.4, 6, 0 | CV_HAAR_SCALE_IMAGE,
			Size(faces[i].width*0.2, faces[i].height*0.2), Size(faces[i].width*0.3, faces[i].height*0.3));
		if (nose.size()) {
			add_best_point(NOSE, im, faces[i], nose[0], image_points);
			found_point[NOSE] = true;
		}
		else {
			//draw_point(im, image_points[NOSE], Scalar(255, 0, 0));
		}

		vector<Rect> eyes;
		eyes_cascade.detectMultiScale(faceROI, eyes, 1.4, 6, 0 | CV_HAAR_SCALE_IMAGE,
			Size(faces[i].width*0.2, faces[i].height*0.2), Size(faces[i].width*0.3, faces[i].height*0.3));

		if (eyes.size() > 0) {
			add_eye_point(im, faces[i], eyes[0], image_points, Scalar(0, 255, 0));
			if (eyes.size() > 1) {
				add_eye_point(im, faces[i], eyes[1], image_points, Scalar(0, 255, 0));
			}
		}
		if (!found_point[LEFTEYE]) {
			//draw_point(im, image_points[LEFTEYE], Scalar(0, 255, 0));
		}
		if (!found_point[RIGHTEYE]) {
			//draw_point(im, image_points[RIGHTEYE], Scalar(0, 255, 0));
		}

		vector<Rect> mouths;
		mouth_cascade.detectMultiScale(faceROI, mouths, 1.4, 6, 0 | CV_HAAR_SCALE_IMAGE,
			Size(faces[i].width*0.3, faces[i].height*0.2), Size(faces[i].width*0.4, faces[i].height*0.3));

		if (mouths.size() && mouths[0].y > faces[i].height * 0.5) {
			found_point[MOUTH] = true;
			add_best_point(MOUTH, im, faces[i], mouths[0], image_points, Scalar(0, 255, 255));
		}
		else {
			//draw_point(im, image_points[MOUTH], Scalar(0, 255, 255));
		}

		break;
	}

	/*if (!found_point[NOSE] || !found_point[LEFTEYE] || !found_point[RIGHTEYE] || !found_point[MOUTH]) {
		cv::imshow("Output", im);
		return;
	}*/

	// 3D model points.
	std::vector<cv::Point3d> model_points;
	std::vector<cv::Point2d> img_points_new;
	if (found_point[NOSE]) {
		model_points.push_back(cv::Point3d(0.0f, 0.0f, 0.0f));               // Nose tip
		img_points_new.push_back(image_points[NOSE]);
	}
	//model_points.push_back(cv::Point3d(0.0f, -330.0f, -65.0f));          // Chin
	if (found_point[LEFTEYE]) {
		model_points.push_back(cv::Point3d(-225.0f, 170.0f, -135.0f));       // Left eye left corner
		img_points_new.push_back(image_points[LEFTEYE]);
	}
	if (found_point[RIGHTEYE]) {
		model_points.push_back(cv::Point3d(225.0f, 170.0f, -135.0f));        // Right eye right corner
		img_points_new.push_back(image_points[RIGHTEYE]);
	}
	if (found_point[MOUTH]) {
		model_points.push_back(cv::Point3d(0.0f, -150.0f, -125.0f));      // Mouth
		img_points_new.push_back(image_points[MOUTH]);
	}
									//model_points.push_back(cv::Point3d(-150.0f, -150.0f, -125.0f));      // Left Mouth corner
									//model_points.push_back(cv::Point3d(150.0f, -150.0f, -125.0f));       // Right mouth corner
																	  // Camera internals
	int count = 0;
	for (int i = 0; i < found_point.size(); i++){
		count += found_point[i];
	}
	cv::imshow("Output", im);
	return;
	//
	if (count < 3) {
		cv::imshow("Output", im);
		return;
	}
	cout << count;
	double focal_length = MAX(im.cols, im.rows); // Approximate focal length.
	Point2d center = cv::Point2d(im.cols / 2, im.rows / 2);
	cv::Mat camera_matrix = (cv::Mat_<double>(3, 3) << focal_length, 0, center.x,
		0, focal_length, center.y,
		0, 0, 1);
	cv::Mat dist_coeffs = cv::Mat::zeros(4, 1, cv::DataType<double>::type); // Assuming no lens distortion

	cout << "Camera Matrix " << endl << camera_matrix << endl;
	// Output rotation and translation
	cv::Mat rotation_vector; // Rotation in axis-angle form
	cv::Mat translation_vector;

	// Solve for pose
	int solvePnPMethod = CV_ITERATIVE; 
	if (count == 3) {
		solvePnPMethod = CV_P3P;
	}
	if (cv::solvePnP(model_points, img_points_new, camera_matrix, dist_coeffs, rotation_vector, translation_vector, false, CV_EPNP))
		cout << "ok!";

	// Project a 3D point (0, 0, 1000.0) onto the image plane.
	// We use this to draw a line sticking out of the nose

	vector<Point3d> nose_end_point3D;
	vector<Point2d> nose_end_point2D;
	nose_end_point3D.push_back(Point3d(0, 0, 200.0));
	nose_end_point3D.push_back(Point3d(200.0, 0, 0));
	nose_end_point3D.push_back(Point3d(0, 200.0, 0));

	projectPoints(nose_end_point3D, rotation_vector, translation_vector, camera_matrix, dist_coeffs, nose_end_point2D);

	cv::line(im, image_points[0], nose_end_point2D[0], cv::Scalar(255, 0, 0), 2);
	cv::line(im, image_points[0], nose_end_point2D[1], cv::Scalar(0, 255, 0), 2);
	cv::line(im, image_points[0], nose_end_point2D[2], cv::Scalar(0, 0, 255), 2); // y

	cout << "Rotation Vector " << endl << rotation_vector << endl;
	cout << "Translation Vector" << endl << translation_vector << endl;

	cout << nose_end_point2D << endl;

	// Display image.
	cv::imshow("Output", im);
	//cv::waitKey(0);
}

std::vector<CvPoint2D32f> projectedPoints;

CvPOSITObject *InitPOSIT(std::vector<CvPoint3D32f> &modelPoints) {
	modelPoints.push_back(cvPoint3D32f(0.0f, 0.0f, 0.0f));
	modelPoints.push_back(cvPoint3D32f(-225.0f, 170.0f, -135.0f));
	modelPoints.push_back(cvPoint3D32f(225.0f, 170.0f, -135.0f));
	modelPoints.push_back(cvPoint3D32f(0.0f, -150.0f, -125.0f));

	CvPOSITObject *positObject = cvCreatePOSITObject(&modelPoints[0], static_cast<int>(modelPoints.size()));


	return positObject;
}

void ProjectModelPoints(float *pose, vector<CvPoint2D32f> &projection_points, vector<CvPoint2D32f> &modelPoints) {
	for (int p = 0; p < modelPoints.size(); p++) {
		float modelPoint[] = { modelPoints[p].x, modelPoints[p].y, modelPoints[p].x, 1.0f };
		CvMat modelMatrix = cvMat(4, 4, CV_32F, modelPoint);
		//CvMat 
	}
}




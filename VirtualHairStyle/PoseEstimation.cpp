#include "PoseEstimation.h"
#include <Windows.h>
#include "HeadPos.h"
#include <iostream>

void draw_point(cv::Mat &img, const dlib::full_object_detection& d, const int point_idx) {
	cv::circle(img, cv::Point(d.part(point_idx).x(), d.part(point_idx).y()), 3, cv::Scalar(255, 0, 0), -1);
}

void render_face(cv::Mat &img, const dlib::full_object_detection& d) {
	DLIB_CASSERT
	(
		d.num_parts() == 68,
		"\n\t Invalid inputs were given to this function. "
		<< "\n\t d.num_parts():  " << d.num_parts()
	);

	draw_point(img, d, 8); // chin
	draw_point(img, d, 30); // nose
	draw_point(img, d, 48); // left lip
	draw_point(img, d, 54); // right lip
	draw_point(img, d, 36); // left eye
	draw_point(img, d, 45); // right eye
	draw_point(img, d, 0); // left ear
	draw_point(img, d, 16); // right ear

}

void add_point(std::vector<cv::Point2d> &img_points, const dlib::full_object_detection& d, const int idx) {
	img_points.push_back(cv::Point2d(d.part(idx).x(), d.part(idx).y()));
}

void detect_2d_points(cv::Mat &img) {
	std::vector<cv::Point2d> img_points;
	dlib::cv_image<dlib::bgr_pixel> cimg(img);	
	
	// Detect faces 
	std::vector<dlib::rectangle> faces = detector(cimg);
	// Find the pose of each face.
	std::vector<dlib::full_object_detection> shapes;

	for (unsigned long i = 0; i < faces.size(); ++i) {
		dlib::full_object_detection shape = pose_model(cimg, faces[i]);
		shapes.push_back(shape);
		render_face(img, shape);
		add_point(img_points, shape, 8); // chin
		add_point(img_points, shape, 30); // nose
		add_point(img_points, shape, 48); // left lip
		add_point(img_points, shape, 54); // right lip
		add_point(img_points, shape, 36); // left eye
		add_point(img_points, shape, 45); // right eye
		add_point(img_points, shape, 0); // left ear
		add_point(img_points, shape, 16); // right ear
		break;
	}
	imgTex.set(img);
	if (!img_points.empty()) solve_head_pos(Mat(img_points), img);
}
#include "PoseEstimation.h"
#include "HeadPos.h"
#include "OGL_OCV_common.h"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

GLMmodel* head_obj;
GLMmodel* hair_obj;

std::vector<double> rv(3), tv(3);
cv::Mat rvec(rv), tvec(tv);
OpenCVGLTexture imgTex;
cv::Mat cam_mat;
cv::Mat op;
VideoCapture capture;

dlib::frontal_face_detector detector;
dlib::shape_predictor pose_model;

int main(int argc, char **argv) {
	vector<Point3f > modelPoints;
	modelPoints.push_back(Point3f(36.54705, 9.83825, 32.10591)); // chin
	modelPoints.push_back(Point3f(36.83007, 78.31853, 52.03452)); // nose
	modelPoints.push_back(Point3f(14.84977, 51.01152, 30.2378)); // left lip
	modelPoints.push_back(Point3f(58.18253, 51.01152, 29.62239)); // right lip
	modelPoints.push_back(Point3f(-15.12295, 109.30827, 10.40453)); // left eye
	modelPoints.push_back(Point3f(84.44235, 108.86934, 10.04475)); // right eye
	modelPoints.push_back(Point3f(-49.06916, 123.52014, -82.31781)); // left ear
	modelPoints.push_back(Point3f(118.77664, 123.52014, -84.70152)); // right ear

	head_obj = glmReadOBJ("head-obj.obj");
	hair_obj = glmReadOBJ("hair.obj");
	op = Mat(modelPoints);
	/*Scalar m = mean(Mat(modelPoints));
	//op = op - m;

	//assert(norm(mean(op)) < 1e-05); //make sure is centered

	//op = op + Scalar(0,0,25);

	cout << "model points " << op << endl;*/

	rvec = Mat(rv);
	double _d[9] = { 1,0,0,
		0,-1,0,
		0,0,-1 };
	Rodrigues(Mat(3, 3, CV_64FC1, _d), rvec);
	tv[0] = 0; tv[1] = 0; tv[2] = 0;
	tvec = Mat(tv);

	cam_mat = Mat(3, 3, CV_64FC1);

	init_opengl(argc, argv);
	// prepare OpenCV-OpenGL images
	imgTex = MakeOpenCVGLTexture(Mat());

	capture.open(1);
	/*capture.open("C:\\Users\\iantv\\Videos\\gagara.mp4");
	capture.set(CV_CAP_PROP_POS_MSEC, 360000);*/
	/*capture.open("C:\\Users\\iantv\\Videos\\Lipnitskaya.mp4");
	capture.set(CV_CAP_PROP_POS_MSEC, 225000);*/
	//capture.open("C:\\Users\\iantv\\Videos\\i.avi");
	//capture.set(CV_CAP_PROP_POS_MSEC, 225000)

	detector = dlib::get_frontal_face_detector();
	dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;

	glutMainLoop();
	return 0;
}

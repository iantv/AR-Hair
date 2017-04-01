#include "PoseEstimation.h"
#include "HeadPos.h"
#include "OGL_OCV_common.h"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

string window_name = "Capture - Face detection";

FaceDetector faceDetector;
NoseDetector noseDetector;
EyesDetector eyesDetector;
MouthDetector mouthDetector;

GLMmodel* head_obj;
//vector<double> rv, tv;
std::vector<double> rv(3), tv(3);
cv::Mat rvec(rv), tvec(tv);
OpenCVGLTexture imgTex, imgWithDrawing;
Mat camMatrix;
cv::Mat op;
VideoCapture capture;

int main(int argc, char **argv) {
	vector<Point3f > modelPoints;
	modelPoints.push_back(Point3f(36.8301, 78.3185, 52.0345));	//nose (v 1879)
	modelPoints.push_back(Point3f(2.37427, 110.322, 21.7776));	// l eye (v 314)
	modelPoints.push_back(Point3f(70.0602, 109.898, 20.8234));	// r eye (v 0)
	modelPoints.push_back(Point3f(41.3327, 51.0115, 29.6224));	// c mouth (v 695)

//	modelPoints.push_back(Point3f(14.8498, 51.0115, 30.2378));	// l mouth (v 1502)
//	modelPoints.push_back(Point3f(58.1825, 51.0115, 29.6224));	// r mouth (v 695)
//	modelPoints.push_back(Point3f(-61.8886, 127.797, -89.4523));	// l ear (v 2011)
//	modelPoints.push_back(Point3f(127.603, 126.9, -83.9129));		// r ear (v 1138)

	head_obj = glmReadOBJ("head-obj.obj");

	op = Mat(modelPoints);
	Scalar m = mean(Mat(modelPoints));
	//op = op - m;

	//assert(norm(mean(op)) < 1e-05); //make sure is centered

	//op = op + Scalar(0,0,25);

	cout << "model points " << op << endl;

	rvec = Mat(rv);
	double _d[9] = { 1,0,0,
		0,-1,0,
		0,0,-1 };
	Rodrigues(Mat(3, 3, CV_64FC1, _d), rvec);
	tv[0] = 0; tv[1] = 0; tv[2] = 1;
	tvec = Mat(tv);

	camMatrix = Mat(3, 3, CV_64FC1);

	init_opengl(argc, argv); // get GL context, for loading textures
							 //GL_UNS
							 // prepare OpenCV-OpenGL images
	imgTex = MakeOpenCVGLTexture(Mat());
	imgWithDrawing = MakeOpenCVGLTexture(Mat());

	if (!faceDetector.load_cascade() ||
		!noseDetector.load_cascade() ||
		!eyesDetector.load_cascade() ||
		!mouthDetector.load_cascade()) return -1;

	//capture.open(0);
	capture.open("C:\\Users\\iantv\\Videos\\gagara.mp4");
	capture.set(CV_CAP_PROP_POS_MSEC, 360000);
	/*capture.open("C:\\Users\\iantv\\Videos\\Lipnitskaya.mp4");
	capture.set(CV_CAP_PROP_POS_MSEC, 225000);*/
	glutMainLoop();

	return 1;

	return 0;
}

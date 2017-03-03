#include "PoseEstimation.h"

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

string window_name = "Capture - Face detection";

FaceDetector faceDetector;
NoseDetector noseDetector;
EyesDetector eyesDetector;
MouthDetector mouthDetector;

int main(int argc, const char** argv) {
	if (!faceDetector.load_cascade() ||
		!noseDetector.load_cascade() ||
		!eyesDetector.load_cascade() ||
		!mouthDetector.load_cascade()) return -1;
	
	VideoCapture capture;
	Mat frame;

	//capture.open(0);
	capture.open("C:\\Users\\iantv\\Videos\\gagara.mp4");
	capture.set(CV_CAP_PROP_POS_MSEC, 360000);
	/*capture.open("C:\\Users\\iantv\\Videos\\Lipnitskaya.mp4");
	capture.set(CV_CAP_PROP_POS_MSEC, 225000);*/
	
	if (capture.isOpened()) {
		while (capture.read(frame)) {
			//flip(frame, frame, 1);
			if (!frame.empty()) {
				frameCount++;
				detect2dpoints(frame);
			}
			else {
				printf(" --(!) No captured frame -- Break!"); break;
			}

			int c = waitKey(10);
			if ((char)c == 'c') { break; }
		}
	}
	return 0;
}
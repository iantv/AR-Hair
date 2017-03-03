#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/tracking.hpp>
#include <opencv/cxcore.h>

using namespace cv;

#define FACE_DETECTION_INTERVAL 100
#define NOSE_DETECTION_INTERVAL 1
#define MOUTH_DETECTION_INTERVAL 1
#define EYES_DETECTION_INTERVAL 1

class BaseDetector
{
public:
	BaseDetector();
	~BaseDetector();
	int load_cascade();
	virtual Rect2d getDetectedRect() const { return _detectedRect; }
protected:
	CascadeClassifier _cascade;
	String _cascadeName;
	Ptr<Tracker> _tracker;
	Rect2d _detectedRect;
	bool _isDetected;
	int _frame_count;
};

class FaceDetector : public BaseDetector
{
public:
	FaceDetector();	
	~FaceDetector();
	bool try_detect(Mat im, Mat frame_gray, Rect2d *rect, Size minSize = Size(), Size maxSize = Size());
private:
	bool detect_and_tracker(Mat im, Mat frame_gray, Size minSize = Size(), Size maxSize = Size());
};

class FaceElementsDetector : public BaseDetector
{
public:
	FaceElementsDetector();
	~FaceElementsDetector();
	virtual bool try_detect(Mat im, Mat frame_gray, Rect2d *rect, Size minSize = Size(), Size maxSize = Size()) { return false; };
private:

};

class NoseDetector : public FaceElementsDetector
{
public:
	NoseDetector();
	~NoseDetector();
	bool try_detect(Mat im, Mat frame_gray, Rect2d *rect, Size minSize = Size(), Size maxSize = Size()) override;
private:
	bool detect_and_tracker(Mat im, Mat frame_gray, Size minSize = Size(), Size maxSize = Size());
};

class MouthDetector : public FaceElementsDetector
{
public:
	MouthDetector();
	~MouthDetector();
	bool try_detect(Mat im, Mat frame_gray, Rect2d *rect, Size minSize = Size(), Size maxSize = Size()) override;
private:
	bool detect_and_tracker(Mat im, Mat frame_gray, Size minSize = Size(), Size maxSize = Size());
};

class PairFaceElementsDetector : public FaceElementsDetector
{
public:
	PairFaceElementsDetector();
	~PairFaceElementsDetector();
	virtual bool try_detect_pair(Mat im, Mat frame_gray, Rect2d faceRect, Size minSize = Size(), Size maxSize = Size()) { return false; }
	Rect2d getLeftDetectedRect() { return _detectedLeftRect; }
	Rect2d getRightDetectedRect() { return _detectedRightRect; }
protected:
	Ptr<Tracker> _leftTracker;
	Ptr<Tracker> _rightTracker;
	Rect2d _detectedLeftRect;
	Rect2d _detectedRightRect;
};

class EyesDetector : public PairFaceElementsDetector
{
public:
	EyesDetector();
	~EyesDetector();
	bool try_detect_pair(Mat im, Mat frame_gray, Rect2d faceRect, Size minSize = Size(), Size maxSize = Size()) override;
private:
	bool is_left_eye(Rect eye, Rect face);
	bool detect_and_tracker(Mat im, Mat frame_gray, Size minSize = Size(), Size maxSize = Size());
	void add_eye_point(cv::Mat im, Rect face, Rect eye);
	bool _isDetectedLeft = false, _isDetectedRight = false;
};

extern FaceDetector faceDetector;
extern NoseDetector noseDetector;
extern EyesDetector eyesDetector;
extern MouthDetector mouthDetector;
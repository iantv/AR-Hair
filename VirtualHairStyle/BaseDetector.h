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
	unsigned int _interval;
};

class FaceDetector : public BaseDetector
{
public:
	FaceDetector();	
	~FaceDetector();
	bool try_detect(Mat im, Mat frame_gray, Size minSize = Size(), Size maxSize = Size());
private:
	bool detect_and_tracker(Mat im, Mat frame_gray, Size minSize = Size(), Size maxSize = Size());
};

class FaceElementsDetector : public BaseDetector
{
public:
	FaceElementsDetector();
	~FaceElementsDetector();
	virtual bool try_detect(Mat im, Mat frame_gray, Size minSize = Size(), Size maxSize = Size());
protected:
	virtual bool detect_and_tracker(Mat im, Mat frame_gray, Size minSize = Size(), Size maxSize = Size());
};

class NoseDetector : public FaceElementsDetector
{
public:
	NoseDetector();
	~NoseDetector();
	bool try_detect(Mat im, Mat frame_gray, Size minSize = Size(), Size maxSize = Size()) override;
private:
	bool detect_and_tracker(Mat im, Mat frame_gray, Size minSize = Size(), Size maxSize = Size()) override;
};

class MouthDetector : public FaceElementsDetector
{
public:
	MouthDetector();
	~MouthDetector();
	bool try_detect(Mat im, Mat frame_gray, Size minSize = Size(), Size maxSize = Size()) override;
private:
	bool detect_and_tracker(Mat im, Mat frame_gray, Size minSize = Size(), Size maxSize = Size()) override;
};

class PairFaceElementsDetector : public FaceElementsDetector
{
public:
	PairFaceElementsDetector();
	~PairFaceElementsDetector();
	virtual bool try_detect_pair(Mat im, Mat frame_gray, Rect2d faceRect, Size minSize = Size(), Size maxSize = Size());
	Rect2d getLeftDetectedRect() { return _detectedLeftRect; }
	Rect2d getRightDetectedRect() { return _detectedRightRect; }
private:
	bool is_left(Rect eye, Rect face);
	void add_point(cv::Mat im, Rect face, Rect eye);
protected:
	virtual bool detect_and_tracker(Mat im, Mat frame_gray, Rect2d faceRect, Size minSize = Size(), Size maxSize = Size());
	Ptr<Tracker> _leftTracker;
	Ptr<Tracker> _rightTracker;
	Rect2d _detectedLeftRect;
	Rect2d _detectedRightRect;
	bool _isDetectedLeft;
	bool _isDetectedRight;
};

class EyesDetector : public PairFaceElementsDetector
{
public:
	EyesDetector();
	~EyesDetector();
	bool try_detect_pair(Mat im, Mat frame_gray, Rect2d faceRect, Size minSize = Size(), Size maxSize = Size()) override;
};

extern FaceDetector faceDetector;
extern NoseDetector noseDetector;
extern EyesDetector eyesDetector;
extern MouthDetector mouthDetector;
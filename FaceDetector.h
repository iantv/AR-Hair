#ifndef FACEDETECTOR_H
#define FACEDETECTOR_H

#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>

class FaceDetector
{
public:
    FaceDetector();
    ~FaceDetector();
    std::vector<dlib::rectangle> detect(cv::Mat &image);
    void find2DKeyPoints(cv::Mat &image, std::vector<cv::Point2d> &points);
private:
    dlib::frontal_face_detector _detector;
    dlib::shape_predictor _poseModel;
    std::vector<int> _keyPoints;
    void renderFace(cv::Mat &image, dlib::full_object_detection &shape);
};

#endif // FACEDETECTOR_H

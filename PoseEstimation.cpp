#include "PoseEstimation.h"
#include <iostream>
#include <opencv2/calib3d.hpp>

PoseEstimation::PoseEstimation() {
   this->initObjPoints();
    _rv.resize(3);
    _tv.resize(3);

   _tvec = cv::Mat(_tv);
   _rvec = cv::Mat(_rv);
   _rmat = cv::Mat(3, 3, CV_64FC1, _d);
   cv::Rodrigues(_rmat, _rvec);
}

PoseEstimation::~PoseEstimation() {
}

void PoseEstimation::initObjPoints() {
    std::vector<cv::Point3f> modelPoints;
    modelPoints.push_back(cv::Point3f(0.002619f, 1.64075f, 0.10742f));     // chin
    modelPoints.push_back(cv::Point3f(0.002906f, 1.71028f, 0.12765f));     // nose
    modelPoints.push_back(cv::Point3f(-0.01894f, 1.68333f, 0.10709f));     // left lip
    modelPoints.push_back(cv::Point3f(0.02458f, 1.68255f, 0.1049f));       // right lip
    modelPoints.push_back(cv::Point3f(-0.04733f, 1.74132f, 0.08684f));     // left eye
    modelPoints.push_back(cv::Point3f(0.05289f, 1.74154f, 0.08526f));      // right eye
    modelPoints.push_back(cv::Point3f(-0.08431f, 1.75617f, -0.008755f));   // left ear
    modelPoints.push_back(cv::Point3f(0.0849f, 1.75458f, -0.005749f));     // right ear
    _objPoints = cv::Mat(modelPoints);
}

void PoseEstimation::addImagePoints(std::vector<cv::Point2d> &imagePoints) {
    _imgPoints = cv::Mat(imagePoints);
}

void PoseEstimation::solve(cv::Mat &image, std::vector<cv::Point2d> &imagePoints) {
    CameraParams cam(image);
    this->initObjPoints();
    _imgPoints = cv::Mat(imagePoints);
    if (!cv::solvePnP(_objPoints, _imgPoints, cam.cmat, cv::Mat(1, 4, CV_64FC1, cam.distCoeffs), _rvec, _tvec, true, cv::SOLVEPNP_EPNP)) {
        std::cout << "solvePnP was fail" << std::endl;
        return;
    }
    cv::Rodrigues(_rvec, _rmat);
}

cv::Mat PoseEstimation::getRotationMatrix() const {
    return this->_rmat;
}

cv::Mat PoseEstimation::getRotationVector() const {
    return this->_rvec;
}

cv::Mat PoseEstimation::getTranslationVector() const {
    return this->_tvec;
}

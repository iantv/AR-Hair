#ifndef POSEESTIMATION_H
#define POSEESTIMATION_H
#include <opencv2/imgproc.hpp>

class PoseEstimation
{
public:
    PoseEstimation();
    ~PoseEstimation();
    void addImagePoints(std::vector<cv::Point2d> &imagePoints);
    void solve(cv::Mat &image);
    cv::Mat getRotationMatrix() const;
    cv::Mat getRotationVector() const;
    cv::Mat getTranslationVector() const;
private:
    cv::Mat _objPoints;
    cv::Mat _imgPoints;
    double _d[9] = {
        1.f,0.f,0.f,
        0.f,-1.f,0.f,
        0.f,0.f,-1.f };
    std::vector<double> rv;
    std::vector<double> tv;
    cv::Mat _rvec;
    cv::Mat _tvec;
    cv::Mat _rmat;

    void initObjPoints();
};

#endif // POSEESTIMATION_H

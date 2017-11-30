#ifndef POSEESTIMATION_H
#define POSEESTIMATION_H
#include <opencv2/imgproc.hpp>

class PoseEstimation
{
public:
    PoseEstimation();
    ~PoseEstimation();
    void addImagePoints(std::vector<cv::Point2d> &imagePoints);
    void solve(cv::Mat &image, std::vector<cv::Point2d> &imagePoints);
    cv::Mat getRotationMatrix() const;
    cv::Mat getRotationVector() const;
    cv::Mat getTranslationVector() const;
protected:
    void drawBasis(cv::Mat &image, std::vector<cv::Point2d> &imagePoints);
private:
    struct CameraParams {
        CameraParams(cv::Mat &image) {
            this->dmax = MAX(image.rows, image.cols);
            this->cmat = (cv::Mat_<double>(3, 3) << dmax, 0, image.cols / 2.0,
                          0, dmax, image.rows / 2.0,
                          0, 0, 1.0);
            memset(distCoeffs, 0, sizeof distCoeffs);
        }
        int dmax;
        cv::Mat cmat;
        double distCoeffs[4];
    };

    cv::Mat _objPoints;
    cv::Mat _imgPoints;
    double _d[9] = {
        1.f,0.f,0.f,
        0.f,-1.f,0.f,
        0.f,0.f,-1.f };
    std::vector<double> _rv;
    std::vector<double> _tv;
    cv::Mat _rvec;
    cv::Mat _tvec;
    cv::Mat _rmat;

    void initObjPoints();
};

#endif // POSEESTIMATION_H

#include "FaceDetector.h"
#include <opencv2/imgproc.hpp>

#define FACE_DOWNSAMPLE_RATIO 1.5

FaceDetector::FaceDetector() {
    _detector = dlib::get_frontal_face_detector();
    dlib::deserialize("C:\\Users\\Tatiana\\Documents\\Qt\\Projects\\TestOpenCV\\shape_predictor_68_face_landmarks.dat") >> _poseModel;

    int PT_CHIN = 8;
    int PT_NOSE = 30;
    int PT_LLIP = 48;
    int PT_RLIP = 54;
    int PT_LEYE = 36;
    int PT_REYE = 45;
    int PT_LEAR = 0;
    int PT_REAR = 16;
    int points[8] = { PT_CHIN, PT_NOSE, PT_LLIP, PT_RLIP, PT_LEYE, PT_REYE, PT_LEAR, PT_REAR };
    for (int i = 0; i < 8; i++) {
        _keyPoints.push_back(points[i]);
    }
}

FaceDetector::~FaceDetector() {
    _keyPoints.clear();
}

std::vector<dlib::rectangle> FaceDetector::detect(cv::Mat &image) {
    cv::Mat img_small;
    cv::resize(image, img_small, cv::Size(), 1.0 / FACE_DOWNSAMPLE_RATIO, 1.0 / FACE_DOWNSAMPLE_RATIO);
    dlib::cv_image<dlib::bgr_pixel> cimg_small(img_small);
    std::vector<dlib::rectangle> faces = _detector(cimg_small);
    return faces;
}

void FaceDetector::find2DKeyPoints(cv::Mat &image, std::vector<cv::Point2d> &points) {
    std::vector<dlib::full_object_detection> shapes;
    dlib::cv_image<dlib::bgr_pixel> cimg(image);

    std::vector<dlib::rectangle> faces = this->detect(image);

    for (unsigned long i = 0; i < faces.size(); ++i) {
        dlib::rectangle r(
            (long)(faces[i].left() * FACE_DOWNSAMPLE_RATIO),
            (long)(faces[i].top() * FACE_DOWNSAMPLE_RATIO),
            (long)(faces[i].right() * FACE_DOWNSAMPLE_RATIO),
            (long)(faces[i].bottom() * FACE_DOWNSAMPLE_RATIO)
        );
        dlib::full_object_detection shape = _poseModel(cimg, r);
        shapes.push_back(shape);
        this->renderFace(image, shape);
        for (int i = 0; i < _keyPoints.size(); i++) {
            points.push_back(cv::Point2d(shape.part(_keyPoints[i]).x(), shape.part(_keyPoints[i]).y()));
        }
        break;
    }
}

void FaceDetector::renderFace(cv::Mat &image, dlib::full_object_detection &shape) {
    DLIB_CASSERT (
        shape.num_parts() == 68,
        "\n\t Invalid inputs were given to this function. "
        << "\n\t d.num_parts():  " << shape.num_parts());
    for (int i = 0; i < _keyPoints.size(); i++) {
        cv::circle(image, cv::Point(shape.part(_keyPoints[i]).x(), shape.part(_keyPoints[i]).y()), 3, cv::Scalar(255, 0, 0), -1);
    }
}

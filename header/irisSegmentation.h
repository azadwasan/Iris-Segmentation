#pragma once

#include "../header/MaxLocation.h"
#include "opencv2/highgui.hpp"
#include "../header/irisSegmentation.h"
#include <string>
#include <utility>

using namespace cv;

//Forward declaration
class MaxLocation;

class CIrisSegmentation{
public:
    //TODO: Apply rule of 5
    CIrisSegmentation(std::string imageLocation);
    ~CIrisSegmentation(){}  //Nothing to do here, as we use the member variables that don't require explicit cleanup.
    bool PerformSegmentation();

private:
    //Private defs:
    enum class ParseArea{
        Pupil,
        Iris
    };

private:
    //Private Methods
    Point correlateTemplate();
    Mat detectEdges();
    CMaxLocation detectPupil(const Mat& edged_image, const Point& center);
    CMaxLocation detectIris(const Mat& edged_image, Point center);
    CMaxLocation fcht(const Mat& edged_image, const Point& center, double radius, ParseArea area);

    void processPixel(Mat& result, Mat& imgCopy, Point currentPoint, const Point& center, double radius);
    void computeSemiCircle(Mat& result, Mat& imgCopy, double pointAngle, const Point& currentPoint, double radius);
    double adjustAngle(double dy, double dx, double angle);

    void extractIris(Mat& result, const CMaxLocation& iris, const CMaxLocation& pupil);
    void drawCircle(Mat& circleImage, Point center, int radius);
    void drawCircleToPoint();
    std::string type2str(int type) ;

private:
    const std::string m_templateLocation{"../misc/pupilmmu2.bmp"};
    //Private members
    std::string m_imageLocation;
    Mat eye_image;

    const std::pair<int, int> PUPIL_RANGE{20, 32};  //Range of radius that the pupil generally lies in 
    const std::pair<int, int> IRIS_RANGE{45, 70};   //Range of radius that the iris generally lies in
    const int ANGLE_COMPUTE_RANGE = 25;             //Hough transform searches in the direction of rough center. This tell it to search in the range around that center 
    const int PUPIL_AREA_SPAN = 40;                //How large is the area of pupil, in order to reduce the search space.
};

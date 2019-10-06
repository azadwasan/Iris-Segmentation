#pragma once

#include "opencv2/imgproc.hpp"

using namespace cv;

class CMaxLocation{
public:
    CMaxLocation();
    CMaxLocation(const Point& pnt, double max_val, double rad);
    //TODO: Apply rule of 5
    
    Point getLocation() const;
    void setLocation(const Point& loc);

    double getRadius() const;
    //TODO: Upload comparison operator
public:
    Point location;
    double max_value;
    double radius;
};
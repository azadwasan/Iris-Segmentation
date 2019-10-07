#include "../header/MaxLocation.h"
using namespace cv;

CMaxLocation::CMaxLocation():location{}, max_value(0), radius(0){
}

CMaxLocation::CMaxLocation(const Point& pnt, double max_val, double rad):location{pnt}, max_value{max_val}, radius{rad}{
}

CMaxLocation::CMaxLocation(CMaxLocation& other){
    *this = other;
}

CMaxLocation::CMaxLocation(CMaxLocation&& other){
    *this = other;
}

CMaxLocation& CMaxLocation::operator=(CMaxLocation& other){
    location = other.location;
    max_value = other.max_value;
    radius = other.radius;

    return *this;
}

CMaxLocation& CMaxLocation::operator=(CMaxLocation&& other){
    *this = other;
    return *this;
}

bool CMaxLocation::operator>(const CMaxLocation& other) const{
    return max_value > other.max_value;
}

bool CMaxLocation::operator<(const CMaxLocation& other) const{
    return max_value < other.max_value;
}

Point CMaxLocation::getLocation() const{
    return location;
}

void CMaxLocation::setLocation(const Point& loc){
    location = loc;
}

double CMaxLocation::getRadius() const{
    return radius;
}

double CMaxLocation::getMaxValue() const{
    return max_value;
}

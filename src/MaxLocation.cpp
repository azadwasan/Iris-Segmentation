#include "../header/MaxLocation.h"
using namespace cv;

CMaxLocation::CMaxLocation():location{}, max_value(0), radius(0){

}

CMaxLocation::CMaxLocation(const Point& pnt, double max_val, double rad):location{pnt}, max_value{max_val}, radius{rad}{
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

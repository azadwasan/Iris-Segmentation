#include "../header/irisSegmentation.h"
#include <exception>
#include <iostream>

using namespace std;
using namespace cv; 

const double pi = 3.1415;

CIrisSegmentation::CIrisSegmentation(std::string imageLocation):m_imageLocation{imageLocation}{
    eye_image = imread( imageLocation, 1);
    if(!eye_image.data){
        throw std::runtime_error("Failed to load the image");
    }
    cout << "OpenCV version : " << CV_VERSION << endl;
    cout << "Major version : " << CV_MAJOR_VERSION << endl;
    cout << "Minor version : " << CV_MINOR_VERSION << endl;
    cout << "Subminor version : " << CV_SUBMINOR_VERSION << endl;
}

CIrisSegmentation::CIrisSegmentation(const CIrisSegmentation& other){
    copyOtherObject(other);
}

CIrisSegmentation::CIrisSegmentation(const CIrisSegmentation&& other){
    copyOtherObject(other);
}

CIrisSegmentation& CIrisSegmentation::operator=(const CIrisSegmentation& other){
    copyOtherObject(other);
    return *this;
}

CIrisSegmentation& CIrisSegmentation::operator=(const CIrisSegmentation&& other){
    copyOtherObject(other);
    return *this;
}

void CIrisSegmentation::copyOtherObject(const CIrisSegmentation& other){
    m_imageLocation = other.m_imageLocation;
    other.eye_image.copyTo(eye_image);  //Simple assignment only copies the matrix header, hence we explicitly copy the matrix.
}

bool CIrisSegmentation::PerformSegmentation(){
    if(!eye_image.data){
        cout<<"Eye image invalid, returing"<<endl;
        return false;
    }
    //TODO:  Catch exception
    Point roughCenter = correlateTemplate();
    Mat edged_image = detectEdges();
    // imshow( "edged image", edged_image);

    CMaxLocation maxLocPupil = detectPupil(edged_image, roughCenter);
    cout << "Best fit found for pupil = maxValue = "<< maxLocPupil.getMaxValue()<<", radius = "<<maxLocPupil.getRadius()<<", (x, y)="<<maxLocPupil.getLocation().x<<", "<<maxLocPupil.getLocation().y<<endl;
 
    CMaxLocation maxLocIris = detectIris(edged_image, maxLocPupil.getLocation());
    cout << "Best fit found for iris = maxValue = "<< maxLocIris.getMaxValue()<<", radius = "<<maxLocIris.getRadius()<<", (x, y)="<<maxLocIris.getLocation().x<<", "<<maxLocIris.getLocation().y<<endl;

    circle(eye_image, maxLocPupil.getLocation(), maxLocPupil.getRadius(), Scalar::all(255), 1, 8, 0);
    circle(eye_image, maxLocIris.getLocation(), maxLocIris.getRadius(), Scalar::all(255), 1, 8, 0);

    circle(edged_image, maxLocPupil.getLocation(), maxLocPupil.getRadius(), Scalar::all(255), 1, 8, 0);
    circle(edged_image, maxLocIris.getLocation(), maxLocIris.getRadius(), Scalar::all(255), 1, 8, 0);

    imshow("Iris detected edged image ", edged_image);
    imshow("Iris detected image ", eye_image);

    // Mat iris(0,0, eye_image.type());
    // extractIris(iris, maxLocPupil, maxLocIris);

    cout<<"Iris extracted, returing...."<<endl;
    // imshow("Extracted iris ", iris);
    return true;
}

CMaxLocation CIrisSegmentation::detectPupil(const Mat& edged_image, const Point& center){

    int colStart = center.x - PUPIL_AREA_SPAN;
    if(colStart<0)
        colStart = 0;
    int colEnd = center.x + PUPIL_AREA_SPAN;
    if(colEnd>=edged_image.size().width)
        colEnd = edged_image.size().width-1;
    int rowStart = center.y - PUPIL_AREA_SPAN;
    if(rowStart<0)
        rowStart = 0;
    int rowEnd = center.y + PUPIL_AREA_SPAN;
    if(rowEnd>=edged_image.size().height)
        rowEnd = edged_image.size().height;

    //New center is simply as far in the new image as it was from the span area top-left    
    Point newCenter(PUPIL_AREA_SPAN, PUPIL_AREA_SPAN);

    Mat edged_pupil_image;
    edged_image(Rect(colStart,rowStart, colEnd - colStart, rowEnd - rowStart)).copyTo(edged_pupil_image);   //NOTE: To be investigated: if we don't clone it, the fcht results are really wierd

    // imshow("Pupil Subimage", edged_pupil_image);

    cout<<"Original, size, width = " << edged_image.size().width <<", height = " << edged_image.size().height
        <<", original center (x, y) = (" << center.x << ", " << center.y <<")"
        <<"Pupil subimage, size, width = " << edged_pupil_image.size().width <<", height = " << edged_pupil_image.size().height
        <<", new center (x, y) = ("      << newCenter.x << ", " << newCenter.y <<")"<<endl;

    CMaxLocation maxLoc{};
    for(double radius = std::get<0>(PUPIL_RANGE);radius<=std::get<1>(PUPIL_RANGE);radius++){
        CMaxLocation newMaxLoc = fcht(edged_pupil_image, newCenter, radius, ParseArea::Pupil);
        if(newMaxLoc>maxLoc){
            maxLoc = newMaxLoc;
        }
    }  
    
    maxLoc.setLocation(center - Point{PUPIL_AREA_SPAN, PUPIL_AREA_SPAN} + maxLoc.getLocation()) ;   //Readjust the detected pupil center according to the original image.

    return maxLoc;   
}

CMaxLocation CIrisSegmentation::detectIris(const Mat& edged_image, Point center){
    CMaxLocation maxLoc{};
    for(double radius = std::get<0>(IRIS_RANGE);radius<=std::get<1>(IRIS_RANGE);radius++){
        CMaxLocation newMaxLoc = fcht(edged_image, center, radius, ParseArea::Iris);
        if(newMaxLoc.getMaxValue()>maxLoc.getMaxValue()){
            maxLoc = newMaxLoc;
        }
    }  
    return maxLoc;
}

Point CIrisSegmentation::correlateTemplate(){
    Mat pupil_template;
    pupil_template = imread(m_templateLocation, 1);
    if(!pupil_template.data){
        throw runtime_error("Failed to load a pupil template image.");
    }

    Mat  corr_result;
    const int match_method = 1;   //1: SQDIFF NORMED 
        /// Source image to display
    Mat img_display;
    eye_image.copyTo( img_display );

    /// Create the result matrix
    int result_cols =  eye_image.cols - pupil_template.cols + 1;
    int result_rows =  eye_image.rows - pupil_template.rows + 1;

    corr_result.create( result_rows, result_cols, CV_32FC1 );

    /// Now perform the matching through correlation
    matchTemplate( eye_image, pupil_template, corr_result, match_method );

    // Normalize the result
    normalize( corr_result, corr_result, 0, 1, NORM_MINMAX, -1, Mat() );

  /// Localizing the best match with minMaxLoc
  double minVal; double maxVal; Point minLoc; Point maxLoc;
  Point matchLoc;

  minMaxLoc( corr_result, &minVal, &maxVal, &minLoc, &maxLoc);

    matchLoc = minLoc;

//   void rectangle(Mat& img, Point pt1, Point pt2, const Scalar& color, int thickness=1, int lineType=8, int shift=0)¶
  rectangle( img_display, matchLoc, Point( matchLoc.x + pupil_template.cols , matchLoc.y + pupil_template.rows ), Scalar::all(0), 2, 8, 0 );
  rectangle( corr_result, matchLoc, Point( matchLoc.x + pupil_template.cols , matchLoc.y + pupil_template.rows ), Scalar::all(0), 2, 8, 0 );

  Point matchCenter{matchLoc.x + int(pupil_template.cols/2), matchLoc.y + int(pupil_template.rows/2)};
  circle(img_display, matchCenter, 15, Scalar::all(255), 4, 8, 0);

    const char* image_window = "Source Image";
    const char* result_window = "Result window";
    /// Create windows
    namedWindow( image_window );
    namedWindow( result_window);

    imshow( image_window, img_display );
    imshow( result_window, corr_result );

    cout<<"Original Image width = " << img_display.cols <<", original Image height = " << img_display.rows << endl
        <<"result Image width = " << corr_result.cols <<", original Image height = " << corr_result.rows << endl;


    return matchCenter;
}

Mat CIrisSegmentation::detectEdges(){
    Mat eye_image_gray;
    Mat edged_image;

    const int lowThreshold = 10;
    const int max_lowThreshold = 200;
    const int ratio = 5;
    const int kernel_size = 3;

    cvtColor( eye_image, eye_image_gray, COLOR_BGR2GRAY );

    blur( eye_image_gray, edged_image, Size(7,7) );
    Canny( edged_image, edged_image, lowThreshold, lowThreshold*ratio, kernel_size );

    // drawCircle(edged_image, center, radius);

    return edged_image;
}

CMaxLocation CIrisSegmentation::fcht(const Mat& edged_image, const Point& center, double radius, ParseArea area){     //Fast circular hough transform
    Mat fchtResult=Mat::zeros(edged_image.size(), CV_32F);
    Mat origCopy;
    edged_image.copyTo(origCopy);

    unsigned char *input = (unsigned char*)(edged_image.data);
    imshow("Received pupil image", edged_image);

    for(int col = 0;col < edged_image.size().width;col++){
        for(int row = 0;row < edged_image.size().height;row++){
            int val = input[edged_image.cols * row + col ] ;
            if(val==255){   //If the pixel if part of an edge (i.e., set/white)
                processPixel(fchtResult, origCopy, Point{col, row}, center, radius);
            }
        }
    }

    // if(area == ParseArea::Pupil){
    //     for(int col = 0;col < edged_image.size().width;col++){
    //         for(int row = 0;row < edged_image.size().height;row++){
    //             int val = input[edged_image.cols * row + col ] ;
    //             if(val==255){   //If the pixel if part of an edge (i.e., set/white)
    //                 processPixel(fchtResult, origCopy, Point{col, row}, center, radius);
    //             }
    //         }
    //     }
    // }
    // else if( area == ParseArea::Iris){
    //     //TODO: Implement the iris optimized search (if the point if outside the pupil box, process it, otherwise don't)
    // }

  //  blur( fchtResult, fchtResult, Size(5,5) );
    double minVal; double maxVal; Point minLoc; Point maxLoc;
    minMaxLoc( fchtResult, &minVal, &maxVal, &minLoc, &maxLoc);

    // imshow("Result ", fchtResult);

    normalize( fchtResult, fchtResult, 0, 1, NORM_MINMAX, -1, Mat() );
    imshow("Normalized fcht result ", fchtResult);

    return CMaxLocation{maxLoc, maxVal, radius};
}

void CIrisSegmentation::processPixel(Mat& result, Mat& imgCopy, Point currentPoint, const Point& center, double radius){
    int dx=center.x - currentPoint.x;
    int dy=center.y - currentPoint.y;
    double pointAngle=atan(abs(double(dy)/double(dx)))*180/pi;      //Compute the angle to the point from the center and convert to degrees for easier looping
    pointAngle = adjustAngle(dy, dx, pointAngle);

    computeSemiCircle(result, imgCopy, pointAngle, currentPoint, radius);
}

void CIrisSegmentation::computeSemiCircle(Mat& result, Mat& imgCopy, double pointAngle, const Point& currentPoint, double radius){
    // const int computeRange = 180;
    Size img_size = result.size();
    for(int theta=pointAngle-ANGLE_COMPUTE_RANGE;theta<=pointAngle+ANGLE_COMPUTE_RANGE;theta++){
        double angle=pi*theta/180;  //Convert back to radians
        int x=ceil(radius*cos(angle)) + currentPoint.x;
        int y=ceil(radius*sin(angle)) + currentPoint.y;
        if(x>0 && x<img_size.width && y>0 && y<img_size.height){
            result.at<float>(y, x) = result.at<float>(y, x) + 1;        //Mat.at<double>(y,x).
            imgCopy.at<uchar>(y,x) = 255;
        }
    }
}

double CIrisSegmentation::adjustAngle(double dy, double dx, double angle){
    if(dy>0){
        if(dx<0){
            angle=90+angle; //or alternatively pointAngle=180-pointAngle;
        }
    }
    else if(dy<0){
        if(dx<0){
            angle=180+angle;
        }
        else
        {
            angle=angle - 90; //or alternatively angle=360-angle;// angle=270+angle;
        }
    }
    else if(dy==0){
        if(dx<0){
            angle=180;
        }
        else if(dx==0){
            angle=0;
        }
    } 
    return angle;
}

void CIrisSegmentation::extractIris(Mat& result, const CMaxLocation& iris, const CMaxLocation& pupil){
    double angularresolution=1;
    double radialresolution=60;
    Mat irisMat(radialresolution, ceil(360/angularresolution), eye_image.type());

    irisMat.copyTo(result);
    Mat eye_copy;
    eye_image.copyTo(eye_copy);

    // imshow("irisMat ", irisMat);
    // imshow("result matrix", result);

    cout<<"Eye image type = "<<type2str(eye_image.type()) << ", iris type = " << type2str(result.type()) <<", iris size (heigth, width)= (" <<result.size().height<<")"<<", "<< result.size().width<<endl;
    int rho=0;
    for(double p=0;p<1; p+=1/radialresolution){
        rho=rho+1;
        for(double theta=90; theta<450-1; theta+=angularresolution){
            double angle=theta*pi/180;
            double pupilx=pupil.getLocation().x + pupil.getRadius()*cos(angle);
            double pupily=pupil.getLocation().y + pupil.getRadius()*sin(angle);
            double irisx=iris.getLocation().x + iris.getRadius()*cos(angle);
            double irisy=iris.getLocation().y + iris.getRadius()*sin(angle);

            double eye_image_x=ceil((1-p)*pupilx+p*irisx);
            double eye_image_y=ceil((1-p)*pupily+p*irisy);

            int result_y = rho;
            int result_x = floor((theta+1-90)/angularresolution);

            cout
            <<"p = " << p << ", theta = " << theta
            <<"Copying pixel from location ("<<eye_image_x <<", " <<eye_image_y<<")"
            <<", to location ("<<result_x <<", " <<result_y<<")";


            if(result_x < result.size().width && result_y < result.size().height){
                // irisMat.at<uchar>(result_x,result_y)=eye_image.at<uchar>(eye_image_x, eye_image_y);
                eye_copy.at<uchar>(eye_image_x, eye_image_y) = 0;
                imshow("Eye copy ", eye_copy);
                // waitKey(0);
            }
            else{
                cout<<"***** Invalid result image coordinate ***** " 
                << "result_x =" << result_x << ", width =" << result.size().width << ", result_y=" << result_y << ", height= " <<result.size().height;
            }
            cout<<endl;
        }
    }
    cout<<"*** YUHU, iris extraction completed successfully ***" << endl;
}

// The following code has been taken from https://stackoverflow.com/questions/10167534/how-to-find-out-what-type-of-a-mat-object-is-with-mattype-in-opencv
string CIrisSegmentation::type2str(int type) {
    string r;
    uchar depth = type & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (type >> CV_CN_SHIFT);
    switch ( depth ) {
        case CV_8U:  r = "8U"; break;
        case CV_8S:  r = "8S"; break;
        case CV_16U: r = "16U"; break;
        case CV_16S: r = "16S"; break;
        case CV_32S: r = "32S"; break;
        case CV_32F: r = "32F"; break;
        case CV_64F: r = "64F"; break;
        default:     r = "User"; break;
    }
    r += "C";
    r += (chans+'0');
    return r;
}

void CIrisSegmentation::drawCircle(Mat& circleImage, Point center, int radius){
    double pi = 3.14;
    int px = center.x;
    int py = center.y;

    cout<<"Image type = "<<type2str(circleImage.type())<<endl;

    cout<< "Center, x = " << px << ", y = " << py <<endl;
    for(double theta=0;theta <= 70; theta= theta + 10){
        double angle=pi*theta/180;
        int x=ceil(radius*cos(angle)) + px;
        int y=ceil(radius*sin(angle)) + py;

        cout<< "Circle edge, x = " << x << ", y = " << y <<endl;

        if(x>0 && x<=200 && y>0 && y<=200){
            circleImage.at<uchar>(y,x)=255;
        }
    }
    imshow( "Semi circle", circleImage );
}

void CIrisSegmentation::drawCircleToPoint(){
    Mat circleImage=Mat::ones(200, 200, CV_32F);
    int radius=30;
    Point center(100,100);
    double pi = 3.14;

    cout<<"Image type = "<<type2str(circleImage.type());

    // Point pointInSpace(50, 50);  //Top-left
    Point pointInSpace(150, 50);  //Bottom-left
    // Point pointInSpace(150, 150);  //Top-right
    // Point pointInSpace(50, 150);  //Bottom-right

    circleImage.at<float>(center.y, center.x) = 0;

    circleImage.at<float>(pointInSpace.y, pointInSpace.x) = 0;

    int dx=center.x-pointInSpace.x;
    int dy=center.y-pointInSpace.y;
    double pointAngle=atan(abs(double(dy)/double(dx)))*180/pi;
    cout<<"Angle from point to the center = "<<pointAngle<<endl;

    if(dy>0){
        if(dx<0){
            pointAngle=90+pointAngle;
            cout<<"Point lies in bottom-left quadrant, adjusting the angle to "<<pointAngle<<endl;
        }
    }
    else if(dy<0){
        if(dx<0){
            pointAngle=180+pointAngle;
            cout<<"Point lies in bottom-right quadrant, adjusting the angle to "<<pointAngle<<endl;
        }
        else
        {
            pointAngle=pointAngle - 90;
            cout<<"Point lies in top-right quadrant, adjusting the angle to "<<pointAngle<<endl;
        }
    }
    else if(dy==0){
        if(dx<0){
            pointAngle=180;
        }
        else if(dx==0){
            pointAngle=0;
        }
    } 

    cout<<"Image type = "<<type2str(circleImage.type())<<endl;

    for(int theta=pointAngle-25;theta<=pointAngle+25;theta++){
        double angle=pi*theta/180;
        int x=ceil(radius*cos(angle)) + center.x;
        int y=ceil(radius*sin(angle)) + center.y;
        if(x>0 && x<=200 && y>0 && y<=200)
            circleImage.at<float>(y, x) = 0;
    }
    imshow( "Semi circle", circleImage );
}

// https://stackoverflow.com/questions/14028193/size-of-matrix-opencv
// Note that apart from rows and columns there is a number of channels and type. When it is clear what type is, the channels can act as an extra dimension as in CV_8UC3 so you would address a matrix as
// uchar a = M.at<Vec3b>(y, x)[i];


// https://answers.opencv.org/question/96739/returning-a-mat-from-a-function/

// cv::Mat A = cv::Mat(int rows, int cols, int type);
 

// int val = input[edged_image.cols * row + col ] ;

// _Tp& cv::Mat::at (int row, int col) 

    // If matrix is of type CV_8U then use Mat.at<uchar>(y,x).
    // If matrix is of type CV_8S then use Mat.at<schar>(y,x).
    // If matrix is of type CV_16U then use Mat.at<ushort>(y,x).
    // If matrix is of type CV_16S then use Mat.at<short>(y,x).
    // If matrix is of type CV_32S then use Mat.at<int>(y,x).
    // If matrix is of type CV_32F then use Mat.at<float>(y,x).
    // If matrix is of type CV_64F then use Mat.at<double>(y,x).

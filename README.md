# CPPND: Capstone Iris Segmentation using OpenCV

This is a Iris segmentation repo for fulfillment of the Capstone project in the [Udacity C++ Nanodegree Program](https://www.udacity.com/course/c-plus-plus-nanodegree--nd213).

Human iris can be using for uniquely identifying a certain person, similar to fingerprints. Iris is the region between pupil and the sclera (white area). It is made of various muscles that contract and relax to allow the appropriate amount of light into the eye through pupil. These muscles form patterns that are unique to each person.

## Dependencies for Running Locally
* cmake >= 3.7
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac), 3.81 (Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)
* opencv 
  * OpenCV version : 4.1.2-pre
  * Major version : 4
  * Minor version : 1
  * For details installation instructions, please follow https://linuxize.com/post/how-to-install-opencv-on-ubuntu-18-04/
* MMU Eyes Database Version 1.0
  * The current implementation has been tuned to work with MMU database. However, the code is generic enough to be easily adapted to other databases easily.
  * This database is no longer available, however, a copy of the database has been included with the repository here.
## Basic Build Instructions

1. Clone this repo.
2. Make a build directory in the top level directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./IrisSegmentation MMU_Data_path/EyeImage.bmp`.
   e.g., ./IrisRecognition ../database/MMU/1/left/aeval1.bmp
   
After the processing has been completed a final image with two circles will be displaying encircling the pupil and the iris. In most cases both iris and pupil are correctly detected. However, there are still cases where the detection does not succeed to detect one or the both the circles correctly due to poor lighting conditions.

## Brief code description

High level code structure:

 main.cpp  ->  main

 irisSegmentation.h/irisSegmentation.cpp

 This is the main class containing the whole logic regarding iris segmentation. 
 
   Brief description of the interface:
   
        Point correlateTemplate();
           Correlate the eye image with a predefined eye template. This gives a rough prelimnary estimate about where the pupil lies roughly in the image.
        Mat detectEdges();
            Produces an edge detected image, used to detect the pupil and the iris.
        CMaxLocation detectPupil(const Mat& edged_image, const Point& center);
            Uses edge detected image to detect the circle fitting the pupil using Circular hogh transform (CHT)
        CMaxLocation detectIris(const Mat& edged_image, Point center);
            Uses edge detected image to detect the circle fitting the iris using Circular hogh transform (CHT)
        CMaxLocation fcht(const Mat& edged_image, const Point& center, double radius, ParseArea area);
            My own improvement over the standard CHT to an imrpoved Fast Circular Hough Transform (FCHT). It improves the search through reducing the searching spacing by searching aroung the rough center detected by correlateTemplate() function. 

Exact details regarding the above mechanisms are provided in Chapter 2 of the thesis under https://github.com/sooperNoob/Iris-Segmentation/blob/master/docu/Iris_Recognition_Thesis.pdf.

## Futue Extensions
1. The current detection algorithm can easily be extended to support multi-threaded proessing. The functions CIrisSegmentation::detectPupil() and CIrisSegmentation::detectIris() can be modified to accept the range of radius they are suppsoed to look for and each range can be processed in a seperate thread.
2. Currently, the whole image is searched to detect the iris circle. This can be improved by searching in a circular pattern around the already detected pupil circle. 
3. Extract features from the segmentated pupil and store them into a database to support iris recognition.

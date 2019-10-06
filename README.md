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
4. Run it: `./irisSegmentation MMU_Data_path/EyeImage.bmp`.

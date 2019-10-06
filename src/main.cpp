#include <iostream>
#include <stdio.h>
#include "../header/irisSegmentation.h"

using namespace std;

/** @function main */
int main( int argc, char** argv )
{
//   img = imread( argv[1], 1 );
    CIrisSegmentation segmenter{argv[1]};
    segmenter.PerformSegmentation();
    cout<<"Segmentation completed.."<<endl;
      waitKey(0);

  return 0;
}
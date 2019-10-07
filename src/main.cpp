#include <iostream>
#include <stdio.h>
#include <exception>
#include "../header/irisSegmentation.h"

using namespace std;

/** @function main */
int main( int argc, char** argv )
{
  if(argc==2){
    try{
      CIrisSegmentation segmenter{argv[1]};
      segmenter.PerformSegmentation();
      cout<<"Segmentation completed.."<<endl;
  }
    catch (std::exception& e) {
      cout<<"Runtime error: " <<e.what()<<endl;
      return 1;
    }
    waitKey(0);
  }
  else{
    cout<<"Please provide a MMU database image."<<endl;
  }

  return 0;
}
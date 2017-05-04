#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <string>

#include "brighten_blue.hpp"

using namespace cv;

using namespace std;

const int rows = 552;
const int cols = 736;

void open_image(uchar* imagePixels)
{
    String imageName( "blue_flower.jpg" ); 

    Mat image;

    image = imread( imageName, IMREAD_COLOR ); // Read the file

    if( image.empty() )                      // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
    }

    memcpy(imagePixels, image.data, rows*cols*3);
}

void removeRG(uchar* imagePixels) {

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            imagePixels[i*3*cols + j*3 + 1] = 0;
            imagePixels[i*3*cols + j*3 + 2] = 0;
        }
    }
}

int main( int argc, char** argv ) {

    uchar* imagePixels;
    imagePixels = new uchar[rows*cols*3];
    open_image(imagePixels);
    removeRG(imagePixels);

    Mat result = Mat(rows, cols, CV_8UC3, imagePixels);
    namedWindow("Display window", WINDOW_AUTOSIZE);
    imshow("Display window", result);

    waitKey(0);
    return 0;

}

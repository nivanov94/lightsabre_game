#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <string>

using namespace cv;

using namespace std;

int main( int argc, char** argv )
{
    String imageName( "blue_flower.jpg" ); 
    if( argc > 1)
    {
        imageName = argv[1];
    }

    Mat image;

    image = imread( imageName, IMREAD_COLOR ); // Read the file


    uchar* imagePixels = image.data;
    //int value = (uchar) imagePixels[0];

    //cout << image.rows << "\r\n";
    //cout << image.cols << "\r\n";
    //cout << image.dims << "\r\n";
    cout << image.type() << "\r\n";

    int i;
    int j;

    for (i = 0; i < image.rows; i++) {
        for (j = 0; j < image.cols; j++) {
           *(imagePixels + i*image.step[0] + j*image.step[1] + 1) = 0;
           *(imagePixels + i*image.step[0] + j*image.step[1] + 2) = 0;
        }
    }

    if( image.empty() )                      // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    namedWindow( "Display window", WINDOW_AUTOSIZE ); // Create a window for display.

    imshow( "Display window", image );                // Show our image inside it.

    waitKey(0); // Wait for a keystroke in the window
    return 0;
}



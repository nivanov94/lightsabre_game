#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <string>

#include "vbx.h"

#include "brighten_blue.hpp"


using namespace cv;

using namespace std;

const int rows = 4032;
const int cols = 3024;

const uchar bluePixThresh = 125;
const uchar redPixThresh = 80;
const uchar grnPixThresh = 230;

void open_image(uchar* imagePixels)
{
    String imageName( "lightsabre.jpg" ); 

    Mat image;

    image = imread( imageName, IMREAD_COLOR ); // Read the file

    if( image.empty() )                      // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
    }

    memcpy(imagePixels, image.data, rows*cols*3);
}

void removeRG(uchar* imagePixels, const short image_width, const short image_height, const short image_pitch) {

    //char* SCRATCHPAD_BASE = VBX_GET_THIS_MXP()->scratchpad_addr;
    //int sp_size = VBX_GET_THIS_MXP()->scratchpad_size;

#if VBX_SIMULATOR==1
    vbxsim_init(4, 64, 256, 6, 5, 4);
#endif

    vbx_uword_t* allRowPix = (vbx_uword_t*)vbx_sp_malloc( image_width*sizeof(vbx_uword_t) );
    vbx_uword_t* nextRowPix = (vbx_uword_t*)vbx_sp_malloc( image_width*sizeof(vbx_uword_t) );
    vbx_ubyte_t* tempRowPix = (vbx_ubyte_t*)vbx_sp_malloc( image_pitch*sizeof(vbx_ubyte_t) );
    vbx_ubyte_t* blueRowPix = (vbx_ubyte_t*)vbx_sp_malloc( image_width*sizeof(vbx_ubyte_t) );
    vbx_ubyte_t* redRowPix = (vbx_ubyte_t*)vbx_sp_malloc( image_width*sizeof(vbx_ubyte_t) );
    vbx_ubyte_t* grnRowPix = (vbx_ubyte_t*)vbx_sp_malloc( image_width*sizeof(vbx_ubyte_t) );
    vbx_ubyte_t* tempPix = (vbx_ubyte_t*)vbx_sp_malloc( image_width*sizeof(vbx_ubyte_t));

    vbx_dma_to_vector(allRowPix, imagePixels, image_pitch);
    vbx_sync();

    for (int i = 0; i < image_height; i++) {

        // get the next row for processing
        if ( i != image_height - 1) {
            vbx_dma_to_vector(nextRowPix, imagePixels + image_pitch*(i + 1), image_pitch);
        }

        // get blue pixel values for processing
        vbx_set_2D(image_width, 1, 3, 0);
        vbx_set_vl(1);
        vbx_2D( VVWBU, VMOV, blueRowPix, allRowPix, 0);

        vbx_set_2D(image_width, 1, 0 ,3);
        vbx_2D( SVWBU, VSHR, grnRowPix, 8, allRowPix);
        vbx_2D( SVWBU, VSHR, redRowPix, 16, allRowPix);

        // enhance blue pixels
        vbx_set_vl(image_width);
        vbx(SVBU, VSUB, tempPix, bluePixThresh, blueRowPix);
        vbx(SVBU, VCMV_GTZ, blueRowPix, 0, tempPix);
        vbx(SVBU, VCMV_LTZ, blueRowPix, 255, tempPix);

        vbx(SVBU, VSUB, tempPix, grnPixThresh, grnRowPix);
        vbx(SVBU, VCMV_LTZ, blueRowPix, 0, tempPix);

        vbx(SVBU, VSUB, tempPix, redPixThresh, redRowPix);
        vbx(SVBU, VCMV_LTZ, blueRowPix, 0, tempPix);

        // put blue pixels back in with zeroed red and green pixels
        vbx_set_2D(image_width, 3, 1, 0);

        //for (int j = 0; j < image_width; j++) {
        vbx_set_vl(1);
        vbx_2D(VVBU, VMOV, tempRowPix, blueRowPix, 0);
        //}

        // write the enhanced blue pixels to host
        vbx_dma_to_host(imagePixels + i*image_pitch, tempRowPix, image_pitch);
        vbx_sync();

        allRowPix = nextRowPix;
    }
}

int main( int argc, char** argv ) {

    uchar* imagePixels;
    imagePixels = new uchar[rows*cols*3];
    open_image(imagePixels);
    removeRG(imagePixels, cols, rows, 3*cols);

    Mat result = Mat(rows, cols, CV_8UC3, imagePixels);
    namedWindow("Display window", WINDOW_NORMAL);
    imshow("Display window", result);

    waitKey(0);
    return 0;

}

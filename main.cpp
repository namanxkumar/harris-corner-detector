#include <iostream>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

Mat src, src_gray;
int thresh = 200;
int MAX_THRESH = 255;
string source_window("Source image");
string corners_window("Harris Corner detection");

void selfMadeHarrisCornerDetector(Mat input, Mat &output){
    Mat src_blurred;
    GaussianBlur(input, src_blurred, Size(7,7), 0, 0, BORDER_DEFAULT);
    Mat x_derivative, y_derivative;
    // First, calculate x and y derivatives
    Sobel(input, x_derivative, CV_32FC1, 1, 0);
    Sobel(input, y_derivative, CV_32FC1, 0, 1);
    // Find other parameters to calculate M matrix
    Mat x_derivative_squared, y_derivative_squared, xy_derivative;
    multiply(x_derivative, x_derivative, x_derivative_squared);
    multiply(y_derivative, y_derivative, y_derivative_squared);
    multiply(x_derivative, y_derivative, xy_derivative);
    // Compute R
    Mat R, x2y2, xy, det, trace, trace_sq;
    multiply(x_derivative_squared, y_derivative_squared, x2y2);
    multiply(x_derivative, y_derivative, xy);
    add(x_derivative_squared, y_derivative_squared, trace);
    multiply(trace, trace, trace_sq);
    output = (x2y2 - xy) - 0.04*trace_sq;
};

void detectCornerHarris(int,void*)
{
    int blockSize = 2;
    int apertureSize = 3;
    double k = 0.04;
    Mat dst = Mat::zeros( src.size(), CV_32FC1 );
    selfMadeHarrisCornerDetector(src_gray, dst);
    // cornerHarris( src_gray, dst, blockSize, apertureSize, k );
    Mat dst_norm, dst_norm_scaled;
    normalize( dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
    convertScaleAbs( dst_norm, dst_norm_scaled );
    for( int i = 0; i < dst_norm.rows ; i++ )
    {
        for( int j = 0; j < dst_norm.cols; j++ )
        {
            if( (int) dst_norm.at<float>(i,j) > thresh )
            {
                circle( dst_norm_scaled, Point(j,i), 5,  Scalar(255), 2, 8, 0 );
            }
        }
    }
    namedWindow( corners_window.c_str() );
    imshow( corners_window.c_str(), dst_norm_scaled );
}


int main( int argc, char** argv )
{
    if(argc != 2) {
        cout << "Error!!\n\nPlease specify input file..\n";
        return -1;
    }
    src = imread( argv[1] );
    if ( src.empty() )
    {
        cout << "Could not open or find the image!\n\n";
        cout << "Usage: " << argv[0] << " <Input image>\n" ;
        return -1;
    }
    cvtColor( src, src_gray, COLOR_BGR2GRAY );
    namedWindow( source_window.c_str());
    createTrackbar( "Threshold: ", source_window, &thresh, MAX_THRESH, detectCornerHarris );
    imshow( source_window.c_str(), src );
    detectCornerHarris(0,0);
    waitKey();
    return 0;
}
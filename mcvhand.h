#ifndef MYCVHAND_H
#define MYCVHAND_H

#include <opencv2/opencv.hpp>

using cv::Mat;
using cv::Size;


//RECT矩形区域缩放系数
typedef struct CVRectZoomCoeff{
    double w;  //宽放大比例
    double h;  //高放大比例
    double up;  //垂直上移比例
    double down;  //垂直下移比例
}CVRectZoomCoeff;


void saveDepthImg( cv::Mat depth, std::string path );
cv::Point getCenterPoint( cv::Rect rect);
cv::Rect rectCutHeight( cv::Rect rect, int cut );
bool inCVRect( const int width, const cv::Rect rect, const int pIndex );
void drawRect( cv::Mat img, cv::Rect rect, cv::Scalar color );

void morphologyFilterOpen( cv::Mat objImg , cv::Size  size );
void morphologyFilterClose( cv::Mat objImg , cv::Size  size );
void morphologyFilterCloseOpen( cv::Mat objImg , cv::Size closeSize, cv::Size openSize );

cv::Rect cvRectZoom( cv::Rect rect, CVRectZoomCoeff coeff, cv::Size maxSize );


cv::Rect tpRectUpScale( cv::Rect rect,
                        double wScaleSet,
                        double hScaleSet,
                        double upRatio,
                        double downRatio,
                        cv::Size maxSize );


void rectToVertexPpt( cv::Rect rect,  cv::Point ppt[] );
void getMaskByRect( cv::Rect rect, cv::Size size, cv::Mat* mask );
int depthMask( cv::Mat srcDepth, cv::Mat mask, cv::Mat* outDepth );
int findMaxArea( const cv::Mat srcImg, cv::Rect* rect );


void mycvhandTest( void );

#endif // MYCVHAND_H



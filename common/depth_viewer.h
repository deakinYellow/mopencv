#ifndef DEPTH_VIEWER_H
#define DEPTH_VIEWER_H

#include <vector>
#include "mat_viewer.hpp"

//////////////////////////////////////////////////////////////////////////////////
class DepthViewer : public OpencvViewer
{
public:
    DepthViewer( const std::string& win);
    virtual void show( const cv::Mat& depthImage );

    virtual void onMouseCallback( cv::Mat& img,
                                  int event,
                                  const cv::Point pnt,
                                  bool& repaint);
    int render( const cv::Mat& img );
    cv::Mat _renderedDepth;

    double depth_scale_unit;


    //获取已经选定的框
    cv::Rect GetSelectedRect( void ){
       cv::Rect rect( 0, 0, 0,0 );
       if( 2 ==  clicked_points_.size() ){
            rect.x = clicked_points_[ 0 ].x;
            rect.y = clicked_points_[ 0 ].y;
            rect.width = clicked_points_[1].x - clicked_points_[ 0 ].x;
            rect.height = clicked_points_[1].y - clicked_points_[ 0 ].y;
        }
       return rect;
    }

    //清除数据
    void ClearClicked( void ){
        clicked_points_.clear();
    }


private:
    cv::Mat _depth;
    //cv::Mat _renderedDepth;
    DepthRender _render;
    GraphicStringItem  _centerDepthItem;
    GraphicStringItem  _pickedDepthItem;
    cv::Point   _fixLoc;

    //单击的点
    std::vector<cv::Point> clicked_points_;

};



#endif // DEPTHVIEWER_H




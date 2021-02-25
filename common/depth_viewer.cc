#include "depth_viewer.h"
#include "../mopencv.hpp"

///////////////////////////// DepthViewer ///////////////////////////////////////
///scale设置为小数时可能会出行干扰横线（判断cv::putText本身问题）
DepthViewer::DepthViewer(const std::string& win)
    : OpencvViewer(win)
    , _centerDepthItem(std::string(), cv::Point(0,400), 0.8, cv::Scalar(0,255,255), 2 )
    , _pickedDepthItem(std::string(), cv::Point(0,440), 0.8, cv::Scalar(0,255,255), 2 ) {

    OpencvViewer::addGraphicItem(&_centerDepthItem);
    OpencvViewer::addGraphicItem(&_pickedDepthItem);
    depth_scale_unit = 1.0;
}


int DepthViewer::render( const cv::Mat& img ) {
    if(img.type() != CV_16U || img.total() == 0){
        return 1;
    }
    //-----------深度信息显示提示-------------
    _depth = img.clone();
    char str[128];

    double val = img.at<uint16_t>( img.rows / 2, img.cols / 2 ) * depth_scale_unit;
    sprintf( str , "center depth: %.1f", val );
    _centerDepthItem.set(str);   //显示图像中心点深度

    val = img.at<uint16_t>(_fixLoc.y, _fixLoc.x)*depth_scale_unit;
    sprintf( str, "point (%d,%d) depth: %.1f", _fixLoc.x, _fixLoc.y , val );
    _pickedDepthItem.set( str );  //显示当前鼠标单击点深度

    //-----------渲染深度图------------------
    //cv::Mat  test(cv::Size( 640, 480 ), CV_8SC3, cv::Scalar(100, 100, 100 ) );
    //_renderedDepth = test.clone();
    _renderedDepth = _render.Compute( img );
    return 0;

}

void DepthViewer::show( const cv::Mat& img) {

    int ret = render( img );  //渲染深度图

    if( 0 != ret ){
        return  ;
    }
    else {
        //先绘制鼠标单击效果
        for( std::vector<cv::Point>::iterator p = clicked_points_.begin() ; p != clicked_points_.end() ; p++ ) {
            cv::circle( _renderedDepth, *p , 3, cv::Scalar(0,255,0), 3 );
        }
        //再绘制选择框
        if( clicked_points_.size() == 2 ){
            mcv::DrawRect( _renderedDepth, GetSelectedRect(), cv::Scalar(0,255, 0 ) );
        }
        //调用显示函数
        OpencvViewer::show( _renderedDepth );
    }
}


////------------鼠标事件回调---------------------------
///查看点的深度信息
///计算区域深度信息
void DepthViewer::onMouseCallback( cv::Mat& img,
                                   int event,
                                   const cv::Point pnt,
                                   bool& repaint) {
    repaint = false;
    switch(event){
        case cv::EVENT_LBUTTONDOWN: {
            _fixLoc = pnt;
            char str[64];
            double val = _depth.at<uint16_t>(pnt.y , pnt.x )*depth_scale_unit;
            repaint = true;
            //--------------框选区域-----------------------
            printf("===========click:( %d %d ) \n", pnt.x, pnt.y );
            cv::Point click_p( pnt.x , pnt.y );
            clicked_points_.push_back( click_p );
            break;
        }
        case cv::EVENT_MOUSEMOVE:
            // uint16_t val = _img.at<uint16_t>(pnt.y, pnt.x);
            // char str[32];
            // sprintf(str, "Depth at mouse: %d", val);
            // drawText(img, str, cv::Point(0,60), 0.5, cv::Scalar(0,255,0), 2);
            break;
    }
}




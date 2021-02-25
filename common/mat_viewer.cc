#include <stdint.h>
#include <stdio.h>
#include "mat_viewer.hpp"


void GraphicRectangleItem::draw(cv::Mat& img){
    cv::rectangle( img, _rect, color() );
}

void GraphicStringItem::draw( cv::Mat& img ){
    cv::putText( img, _str, _loc, cv::FONT_HERSHEY_SIMPLEX  , _scale, _color, _thick );
}


int GraphicItem::globalID = 0;


void OpencvViewer::_onMouseCallback(int event, int x, int y, int /*flags*/, void* ustc)
{
    OpencvViewer* p = (OpencvViewer*)ustc;

    // NOTE: This callback will be called very frequently while mouse moving,
    // keep it simple

    bool repaint = false;
    p->onMouseCallback( p->_orgImg, event, cv::Point(x,y), repaint );
    if(repaint){
        p->showImage();
    }
}


void OpencvViewer::showImage()
{
    _showImg = _orgImg.clone();
    for(std::map<int, GraphicItem*>::iterator it = _items.begin()
            ; it != _items.end(); it++){
        it->second->draw( _showImg );
    }
    cv::imshow( _win.c_str(), _showImg );
    cv::setMouseCallback(_win, _onMouseCallback, this );
}


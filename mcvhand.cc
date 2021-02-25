//#define USING_MLOGD
#include "../utility-cc/tool.hpp"
#include "mcvhand.h"

//保存16位深度图数据  //8位也可以
//参数1:16位深度图数据, 参数2:保存路径,后缀为.png
void saveDepthImg( cv::Mat depth, std::string path ){
    std::vector<int> compressionParams;
    compressionParams.push_back( CV_IMWRITE_PNG_COMPRESSION );
    compressionParams.push_back(0);  //无压缩
    cv::imwrite( path, depth, compressionParams );
}

//获取矩形中心点
cv::Point getCenterPoint( cv::Rect rect)
{
    cv::Point cpt;
    cpt.x = rect.x + cvRound(rect.width/2.0);
    cpt.y = rect.y + cvRound(rect.height/2.0);
    return cpt;
}

//围绕矩形中心缩放
cv::Rect rectCenterScale( cv::Rect rect, cv::Size size )
{
    //求大小
    rect = rect + size;
    //求起点
    cv::Point pt;
    pt.x = cvRound( size.width/2.0);
    pt.y = cvRound( size.height/2.0);
    return ( rect - pt );
}


//裁剪rect //高向下缩小
cv::Rect rectCutHeight( cv::Rect rect, int cut ){
     if( cut < 0 ){
        MLOGE("cut rect fail, cut value is negative!");
        exit(1);
    }
    cv::Rect  cuted_rect  = rect;
    cuted_rect.y = rect.y + cut;
    cuted_rect.height = rect.height - cut;
    return cuted_rect;
}


/**
 * @brief  判断index是否在rect内
 * @param  [in]  width   原始图像宽
 * @param  [in]  rect    目标rect
 * @param  [in]  pIndex  目标index
 * @retval   true 在  false不在
 * @note
 */
bool inCVRect( const int width, const cv::Rect rect, const int pIndex ) {
     int x =  pIndex % width;
     int y =  pIndex / width;
     if( x < rect.x ||
         x > rect.x + rect.width - 1 ||
         y < rect.y ||
         y > rect.y + rect.height - 1 ){
         return false;
     }
     else
         return true;
}


//@brief   Rect安照一定比例缩放
cv::Rect cvRectZoom( cv::Rect rect, CVRectZoomCoeff coeff, cv::Size maxSize ){
    //计算输入中点
    cv::Point srcCenter = getCenterPoint( rect );
    //分别计算中点到上下左右边界最小值
    int wMin = std::min( srcCenter.x, maxSize.width - srcCenter.x );
    int hMin = std::min( srcCenter.y, maxSize.height - srcCenter.y );
    //计算不超边界的可以放大最大尺寸
    double wScaleMax = double( wMin ) / double (rect.width/2);
    double hScaleMax = double( hMin ) / double (rect.height/2);
    //double maxScale = std::min( wScale , hScale );
    //MLOGD("maxScale  %f ", maxScale );

    //限制放大比例
    if( coeff.w >= wScaleMax ){
        coeff.w = wScaleMax;
    }
    if( coeff.h >= hScaleMax ){
        coeff.h = hScaleMax;
    }
    //按照比例放大
    int px = cvCeil( rect.x - rect.width * ( coeff.w - 1 ) / 2 );
    int py = cvCeil( rect.y - rect.height * ( coeff.h - 1 ) / 2 );

    //稍微上移
    py -= coeff.up * cvRound( coeff.h * rect.height );
    //稍微下移
    py += coeff.down * cvRound( coeff.h * rect.height );
    if( px < 0 )
        px = 0;
    if( py < 0 )
        py = 0;
    cv::Point retPoint( px, py );
    int retWidth = cvRound( coeff.w * rect.width );
    int retHeight = cvRound( coeff.h * rect.height );
    //浮点型运行有误差,防止超出边界,
    if(retWidth + px > maxSize.width ){
        retWidth -= ( retWidth + px - maxSize.width );
    }
    if(retHeight + py > maxSize.height ){
        retHeight -= ( retHeight + py - maxSize.height );
    }
    MLOGD("return ret rect.");
    cv::Rect retRect( retPoint.x, retPoint.y, retWidth, retHeight );
    //std::cout  << retRect << std::endl;
    return retRect;
}


/**
 * @brief   安照一定比例放大rect
 * @param  [in]   param_name         some descript
 * @param  [in]   wScaleSet     横向缩放比例
 * @param  [in]   hScaleSet     纵向缩放比例
 * @param  [in]   upRatio     上移比例
 * @param  [in]   downRatio     下移比例
 * @param  [in]   maxSize       最大尺寸限制
 * @retval  操作结果retRect
 * @note
 */
cv::Rect tpRectUpScale( cv::Rect rect,
                        double wScaleSet,
                        double hScaleSet,
                        double upRatio,
                        double downRatio,
                        cv::Size maxSize ) {
    //计算输入中点
    cv::Point srcCenter = getCenterPoint( rect );
    //分别计算中点到上下左右边界最小值
    int wMin = std::min( srcCenter.x, maxSize.width - srcCenter.x );
    int hMin = std::min( srcCenter.y, maxSize.height - srcCenter.y );
    MLOGD("wMin: %d ", wMin );
    MLOGD("hMin: %d ", hMin );
    //计算不超边界的可以放大最大尺寸
    double wScaleMax = (double)wMin / (double)(rect.width/2);
    double hScaleMax = (double)hMin / (double)(rect.height/2);
    //double maxScale = std::min( wScale , hScale );
    //MLOGD("maxScale  %f ", maxScale );

    //限制放大比例
    if( wScaleSet >= wScaleMax ){
        wScaleSet = wScaleMax;
    }
    if( hScaleSet >= hScaleMax ){
        hScaleSet = hScaleMax;
    }
    //按照比例放大
    int px = cvCeil( rect.x - rect.width * ( wScaleSet - 1 ) / 2 );
    int py = cvCeil( rect.y - rect.height * ( hScaleSet - 1 ) / 2 );
    //稍微上移
    py -= upRatio * cvRound( hScaleSet * rect.height );
    //稍微下移
    py += downRatio * cvRound( hScaleSet * rect.height );
    if( px < 0 )
        px = 0;
    if( py < 0 )
        py = 0;
    cv::Point retPoint( px, py );
    int retWidth = cvRound( wScaleSet * rect.width );
    int retHeight = cvRound( hScaleSet * rect.height );
    //浮点型运行有误差,防止超出边界,
    if(retWidth + px > maxSize.width ){
        retWidth -= (retWidth + px - maxSize.width );
    }
    if(retHeight + py > maxSize.height ){
        retHeight -= (retHeight + py - maxSize.height );
    }
    MLOGD("return ret rect.");
    cv::Rect retRect( retPoint.x, retPoint.y, retWidth, retHeight );
    //std::cout  << retRect << std::endl;
    return retRect;
}


/**
 * @brief   由rect,获取4个顶点
 * @param  [in]   rect   目标矩形
 * @param  [out]   ppt    顶点数组
 * @retval
 * @note
 */
void rectToVertexPpt( cv::Rect rect,  cv::Point ppt[] ){
    ppt[ 0 ] = cv::Point( rect.x , rect.y );
    ppt[ 1 ] = cv::Point( rect.x , rect.y + rect.height );
    ppt[ 2 ] = cv::Point( rect.x + rect.width, rect.y + rect.height );
    ppt[ 3 ] = cv::Point( rect.x + rect.width, rect.y );
}


/**
 * @brief  由rect 获取mask
 * @param  [in]  rect          矩形区域
 * @param  [in]  size          mask size
 * @param  [in]  mask          mask图像地址
 * @retval
 * @note
 */
void getMaskByRect( cv::Rect rect, cv::Size size, cv::Mat* mask ){
    cv::Mat roi( rect.size(), CV_8U, cv::Scalar(255) );
    //*mask = cv::Mat( size, CV_8U, cv::Scalar(0) ).clone(); //将新分配的完全拷贝过去
    *mask = cv::Mat( size, CV_8U, cv::Scalar(0) );  //直接分配就行
    roi.copyTo( (*mask)( rect ) );
}



//mask提取深度图部分区域
int depthMask( cv::Mat srcDepth, cv::Mat mask, cv::Mat* outDepth ){
    if( !srcDepth.empty() && !mask.empty()){
        *outDepth = cv::Mat( mask.size(), CV_16U, cv::Scalar(0) );
        srcDepth.copyTo( *outDepth, mask );
        return MOK;
    }
    return MFAIL;
}

/**
 * @brief  opencv形态学滤波,先进行闭操作填充区域, 再进行开操作去除小块
 * @param  [in,out]  objImg        目标图像
 * @param  [in]      closeSize      闭操作大小(想保留哪个方向上的就加大哪个方向的系数)
 * @param  [in]      openSize       开操作滤波块大小
 * @retval
 * @note
 */
void morphologyFilterCloseOpen( cv::Mat objImg , cv::Size closeSize, cv::Size openSize ){

    if( closeSize.width > 0 && closeSize.height > 0 ){
        //闭操作,填充黑色区域
        Mat closeElement = getStructuringElement( cv::MORPH_RECT, closeSize );
        morphologyEx(objImg,objImg, cv::MORPH_CLOSE, closeElement );
    }
    if( openSize.width > 0 && openSize.height > 0 ){
        //开操作,去除噪点 ,滤去白色小块
        Mat openElement = getStructuringElement( cv::MORPH_RECT, openSize );
        morphologyEx(objImg ,objImg, cv::MORPH_OPEN, openElement );
    }
    //灰度操作,可以将边缘凸显出来
    //morphologyEx(objImg,objImg, cv::MORPH_GRADIENT, closeElement );
}


void morphologyFilterOpen( cv::Mat objImg , cv::Size  size ){
    if( size.width > 0 && size.height > 0 ){
        //开操作,去除噪点 ,滤去白色小块
        Mat openElement = getStructuringElement( cv::MORPH_RECT, size );
        morphologyEx(objImg ,objImg, cv::MORPH_OPEN, openElement );
   }
}

void morphologyFilterClose( cv::Mat objImg , cv::Size  size ){
    if( size.width > 0 && size.height > 0 ){
        //闭操作,填充黑色区域
        Mat closeElement = getStructuringElement( cv::MORPH_RECT, size );
        morphologyEx(objImg ,objImg, cv::MORPH_CLOSE, closeElement );
   }
}


//------------绘制多边形------------------------------
void drawRect( cv::Mat img, cv::Rect rect, cv::Scalar color ){
    cv::Point ppt1[4];// ppt2[4];
    rectToVertexPpt( rect, ppt1 );
    const cv::Point* pts[] = { ppt1 };
    //rectToVertexPpt( frame->cbUpRect, ppt2 );
    //const cv::Point* pts[] = { ppt1, ppt2 };
    const int npts[] = { 4 };
    //绘制外围线框
    //cv::polylines( img ,pts,npts,1,true, cv::Scalar(0,0,255),2);
    cv::polylines( img ,pts,npts,1,true, color ,2);
    cv::Point p  = getCenterPoint( rect );
    //绘制中心圆
    cv::circle( img ,p,6,cv::Scalar(0,0,255), 0 );
}

//========================测试区=============================
void mycvhandTest( void ){
    cv::Rect rect1(5,10,100,100);
    MLOGI("rect hand test.");
    std::cout << rect1 << std::endl;

    MLOGI("get center:");
    cv::Point center = getCenterPoint( rect1 );
    std::cout << center << std::endl;

    MLOGI("rect Scale:");
    //cv::Rect upRect1 = rectCenterScale( rect1, cv::Size(10,10));
    //std::cout << upRect1 << std::endl;

    cv::Rect upRect1 = tpRectUpScale(rect1 ,1.5,1,0,0, cv::Size(640,480) );
    std::cout << upRect1 << std::endl;
    while(true);
}


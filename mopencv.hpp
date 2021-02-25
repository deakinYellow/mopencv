#pragma once

#ifndef MOPENCV_HPP
#define MOPENCV_HPP

#include <opencv2/opencv.hpp>

///封装opencv一些常用功能
///
///1.读写操作
namespace mcv {

    using cv::Mat;
    using cv::Point;
    using cv::Rect;
    using cv::Size;
    using cv::Scalar;

    ///io
    static void ImWrite( const std::string path, const Mat src ){
        std::vector<int> compressionParams;
        compressionParams.push_back( CV_IMWRITE_PNG_COMPRESSION );
        compressionParams.push_back(0);  //无压缩
        cv::imwrite( path, src, compressionParams );
    }
    //无损读图
    static Mat ImRead( std::string path ){
        return cv::imread( path, CV_LOAD_IMAGE_UNCHANGED );
    }

    ///加载目录下所有图片
    static void LoadImagesFromDir( const std::string dir, std::vector< Mat >& img_lst ){
        std::vector< cv::String > files;
        cv::glob( (cv::String)dir , files );
        for ( size_t i = 0; i < files.size(); ++i ) {
            Mat img = mcv::ImRead( files[i] ); // load the image
            if ( img.empty() ) { // invalid image, skip it.
                //cout << files[i] << " is invalid!" << endl;
                continue;
            }
            img_lst.push_back( img );
        }
    }

    ///imageCreateByRect
    ///直接用 Image( rect ) 替代
    /*
    static Mat CutImageByRect( const Mat& src_image,  const cv::Rect& rect ){
        cv::Mat out( rect.size(), src_image.type() , cv::Scalar(0) );
        src_image( rect ).copyTo( out );
        return out;
    }
    */

    ///gui
    static void RectToVertexPpt( Rect rect,  Point ppt[] ){
        ppt[ 0 ] = Point( rect.x , rect.y );
        ppt[ 1 ] = Point( rect.x , rect.y + rect.height );
        ppt[ 2 ] = Point( rect.x + rect.width, rect.y + rect.height );
        ppt[ 3 ] = Point( rect.x + rect.width, rect.y );
    }
    static void DrawRect( Mat dst, const Rect rect, Scalar color ){
        Point ppt1[4];
        RectToVertexPpt( rect, ppt1 );
        const Point* pts[] = { ppt1 };
        const int npts[] = { 4 };
        cv::polylines( dst ,pts,npts,1,true, color ,2);
    }

    ///logic
    /**
     * @brief  判断index是否在rect内
     * @param  [in]  width   原始图像宽
     * @param  [in]  rect    目标rect
     * @param  [in]  pIndex  目标index
     * @retval   true 在  false不在
     * @note
     */
    static bool InRect( const int width, const Rect rect, const int pIndex ) {
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
     static bool SizeEqual( Size s1,  Size s2 ){
        if( s1.width == s2.width && s1.height == s2.height ){
            return true;
        } else{
            return false;
        }
     }
}

///CV Rect About
namespace mcv {

    //RECT矩形区域缩放系数
    typedef struct RectZoomCoeffT{
        double w;  //宽放大比例
        double h;  //高放大比例
    }RectZoomCoeffT;

    static RectZoomCoeffT RectZoomCoeff( const double w, const double h ){
        RectZoomCoeffT coff;
        coff.w = w;
        coff.h = h;
        return  coff;
    }

    //rect get by 2 point
    static Rect RectCreateByPoints( const Point P1 , const Point P2 ){
        return  Rect( P1.x, P1.y, P2.x - P1.x, P2.y - P1.y );
    }

    //rects get by points
    static int RectsListCreateByPointList( std::vector<cv::Point> point_list, std::vector<cv::Rect>& rect_list ){
        //clear origin data
        rect_list.clear();
        //check input
        if( point_list.size() < 2 ){
            //TPLOGW("save sample fail, please check points list size.");
            return 1;
        }
        for( int i = 0; i < point_list.size() - 1; i += 2 ){
            cv::Rect rect  = RectCreateByPoints( point_list[ i ], point_list[ i+1 ] );
            rect_list.push_back( rect );
        }
        return 0;
    }

    //获取Rect中心点
    static cv::Point RectCenter( cv::Rect rect ) {
        cv::Point cpt;
        cpt.x = rect.x + cvRound( rect.width / 2.0 );
        cpt.y = rect.y + cvRound( rect.height / 2.0 );
        return cpt;
    }

    //@brief Rect安照一定比例缩小
    static cv::Rect RectZoomD( cv::Rect rect, RectZoomCoeffT coeff ){
        //计算输入中点
        cv::Point srcCenter = RectCenter( rect );
        //根据缩放系数,计算size
        double w = ( coeff.w - 1.0 ) * rect.width;
        double h = ( coeff.h - 1.0 ) * rect.height;

        cv::Size size( w, h );

        cv::Rect ret_rect = rect + size;

        Point pt;
        pt.x = cvRound(size.width/2.0);
        pt.y = cvRound(size.height/2.0);
        ret_rect = ret_rect - pt;
        return ret_rect;
    }

}

////------------------------通用算法------------------------------------
namespace mcv {

    /**
    * @brief 统计rect内灰度等于大于设定阈值点在rect中占比
    * @param  [in]  src_image    目标图像
    * @param  [in]  roi_rect     目标区域
    * @param  [in]  ergodic_step    行列遍历步进,增大缩小计算量
    * @param  [in]  scale_threshold      像素值阈值
    * @retval   true 在  false不在
    * @note
    */
    template<typename  ScaleType>
    static double GetScaleReachThresholdPointsProportion(
                       const Mat&  src_image,
                       const Rect& roi_rect,
                       const int ergodic_step,
                       const uint8_t scale_threshold  ){

        int sum = roi_rect.area() /( ergodic_step * ergodic_step);
        int point_count = 0;
        for( int j = roi_rect.y; j < roi_rect.height + roi_rect.y; j += ergodic_step ){
            for( int i = roi_rect.x; i < roi_rect.width + roi_rect.x;  i += ergodic_step  ){
                if( src_image.at<ScaleType>(j,i) >= scale_threshold ){
                    point_count++;
                }
            }
        }
        return double(point_count) / double( sum );
    }

    /**
     * @brief  根据rect生成mask图像
     * @param  [in]  rect          矩形区域
     * @param  [in]  size          mask size
     * @retval
     * @note
     */
    static Mat GetMaskByRect( const cv::Rect rect, const cv::Size size ){
        cv::Mat mask( size, CV_8U, cv::Scalar(0) );
        cv::Mat roi( rect.size(), CV_8U, cv::Scalar(255) );
        roi.copyTo( mask( rect ) );
        return mask;
    }

    //开操作,去除噪点 ,滤去离散小块
    static int MorphologyFilterOpen( const cv::Mat& src , cv::Mat& dst,  const cv::Size&  size ){
        if( size.width > 0 && size.height > 0 ){
            cv::Mat openElement = getStructuringElement( cv::MORPH_RECT, size );
            cv::morphologyEx(src ,dst, cv::MORPH_OPEN, openElement );
            return 0;
       }
       return 1;
    }
    //闭操作,填充黑色区域
    static int MorphologyFilterClose( const cv::Mat& src, cv::Mat& dst, const cv::Size&  size ){
        if( size.width > 0 && size.height > 0 ){
            cv::Mat closeElement = getStructuringElement( cv::MORPH_RECT, size );
            morphologyEx( src, dst, cv::MORPH_CLOSE, closeElement );
            return 0;
        }
        return 1;
    }

    ///从contour中框定rect
    static cv::Rect GetRectByContour(  const std::vector<cv::Point>& contour ){
       cv::Rect rect;
       std::vector<int> pointX,pointY;
       for( ulong i = 0; i < contour.size(); i++  ){
           cv::Point P0( contour[ i ] );
           //cout << "point: " << P0.x << " " <<P0.y << endl;
           pointX.push_back(P0.x);
           pointY.push_back(P0.y);
       }
       int x_min,x_max,y_min,y_max;
       x_min = *( std::min_element( pointX.begin(),pointX.end() ) );
       x_max = *( std::max_element( pointX.begin(),pointX.end() ) );
       y_min = *( std::min_element( pointY.begin(),pointY.end() ) );
       y_max = *( std::max_element( pointY.begin(),pointY.end() ) );
       rect.x = x_min;
       rect.y = y_min;

       rect.width = x_max - x_min;
       rect.height = y_max - y_min;

       if( rect.width <= 0 ){
            rect.width = 1;
       }
       if( rect.height <= 0 ){
            rect.height = 1;
       }

       return rect;
    }
    ////middle二值化,小于或操作该阈值的都不要
    ///该遍历方式效率较低,后期修改
    template< typename T>
    static void middle_threshold( const Mat& src, Mat& dst, const double thresh_min, const double thresh_max, const double maxval ){
        dst.create( src.size(), src.type() );
        for( int i=0; i < src.rows; i++) {
            for( int j=0; j < src.cols; j++) {
                T value = src.at<T>(i,j); //i行j列
                if( value < thresh_min || value > thresh_max ){
                    dst.at<T>( i, j ) = 0;
                }else{
                    dst.at<T>( i, j ) = maxval;
                }
            }
        }
    }

}

////------------------------可视化相关------------------------------------
namespace mcv {

  /**
   * @brief        生成直方图
   * @param  [in]  src           必须为CV_8U格式
   * @param  [out] dst           histogram output
   * @param  [in]  size          histogram size
   * @param  [in]  min           计入统计的最小像素值
   * @param  [in]  max           计入统计的最大像素值
   * @retval
   * @note
   */
  static void Histogram( const Mat& src,
                         Mat& dst,
                         const Size& size,
                         const uchar min,
                         const uchar max ){
    //确保输入非空
    assert( src.type() == CV_8U || src.type() == CV_8UC1 );
    //printf("==size( %d %d )", size.width, size.height );
    cv::Mat his( size , CV_8U, cv::Scalar(255) );
    //遍历像素统计各像素值数量
    int key_values[256] = {0};

    for( int r = 0; r < src.rows ; r++ ){
      const uchar* row_data = src.ptr<uchar>( r );
      for( int l = 0; l < src.cols ; l++ ) {
        uchar value = row_data[ l ];
        if( value >= min && value <= max ){
          ++key_values[ value ];
        }
      }
    }
    //printf("---------------------------------------------\n");
    int max_value = 0;
    for(int i = 0; i < 256; i++) {
      //printf(" %d ", key_values[ i ] );
      if( key_values[i] > max_value ){
        max_value = key_values[i];
        }
    }
    //printf("\n=====================================max value: %d \n", max_value );
    cv::Scalar color(127);
    cv::Point p0,p1;

    ///draw histogram
    int point_x = 0;
    p0.y = size.height - 1;
    for( int i = 0; i < 256; i++ ){
        //int  h = key_values[ i ];
        int h = double( key_values[i] ) / double( max_value ) * size.height;
        if( h > size.height ){
            h = size.height;
          }
        point_x += 2;
        p0.x = point_x;
        p1.x = point_x;
        p1.y = size.height - 1 - h;
        cv::line( his, p0, p1, color, 1 );
      }
    his.copyTo( dst );
#if 0
    ///去除直方图数值高的点看看
    cv::Mat clean = src.clone();
    for( int r = 0; r < clean.rows ; r++ ){
      uchar* row_data = clean.ptr<uchar>( r );
      for( int l = 0; l < clean.cols ; l++ ) {
        uchar value = row_data[ l ];
        if( key_values[ value ] > 90 || value > 130 ){
          row_data[l] = 255;
        }
      }
    }
    cv::imshow("clean", clean );
    cv::waitKey(10);
#endif
  }
}  // end of namespace mcv



#endif // MOPENCV_HPP



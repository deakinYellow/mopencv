#ifndef SLIPER_H
#define SLIPER_H

#include <iostream>
#include <opencv2/core/core.hpp>

using std::cout;
using std::endl;
using cv::Mat;
using cv::imshow;
using cv::imread;

//滑条变量结构体
typedef struct SliperParam{
   std::string  id;
   double*  dParam;     //实际要修改的浮点型参数对应地址
   int*  iParam;        //实际要修改的整型参数对应地址
   int*  sValue;         //sliper滑条数值
   double scalar;       //实际与待修改参数对应比例关系 dValue = (double)sValue * scalar
}SliperParam;


class Sliper
{
public:
    static std::string _winID;
    Mat _img = Mat( cv::Size(640,480),CV_8UC3, cv::Scalar(100,100,0) );

    static void addParam(  const std::string id,
                    int* iParam,
                    double* dParam,
                    const int sliperValue,
                    const double scalar );

     Sliper( std::string name );

     void showWindow(){
        imshow( _winID, _img );
        cv::waitKey(1);
     }
private:

};

void testSliper();

#endif // SLIPER_H

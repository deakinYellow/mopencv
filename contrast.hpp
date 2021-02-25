#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

using std::cout;
using cv::Mat;
using cv::imshow;
using cv::imread;



static void ContrastAndBright(int, void *);
int g_nContrastValue; //对比度值
int g_nBrightValue;  //亮度值
Mat g_srcImage, g_dstImage;

int contrastTest()
{
    // 读入用户提供的图像
    g_srcImage = imread("0004.bmp");

    g_dstImage = Mat::zeros(g_srcImage.size(), g_srcImage.type());

    //设定对比度和亮度的初值
    g_nContrastValue = 80;
    g_nBrightValue = 80;

    //创建窗口
    namedWindow("【效果图窗口】", 1);

    //创建轨迹条
    createTrackbar("对比度：", "【效果图窗口】", &g_nContrastValue, 300, ContrastAndBright);
    createTrackbar("亮   度：", "【效果图窗口】", &g_nBrightValue, 200, ContrastAndBright);

    //调用回调函数
    ContrastAndBright(g_nContrastValue, 0);
    ContrastAndBright(g_nBrightValue, 0);

    waitKey(0);
    //输出一些帮助信息
    return 0;
}

//-----------------------------【ContrastAndBright( )函数】------------------------------------
//    描述：改变图像对比度和亮度值的回调函数
//-----------------------------------------------------------------------------------------------
static void ContrastAndBright(int, void *)
{
    // 三个for循环，执行运算 g_dstImage(i,j) = a*g_srcImage(i,j) + b
    for (int y = 0; y < g_srcImage.rows; y++)
    {
        for (int x = 0; x < g_srcImage.cols; x++)
        {
            for (int c = 0; c < 3; c++)
            {
                g_dstImage.at<Vec3b>(y, x)[c] = saturate_cast<uchar>((g_nContrastValue*0.01)*(g_srcImage.at<Vec3b>(y, x)[c]) + g_nBrightValue);
            }
        }
    }
    // 显示图像
    imshow("【原始图窗口】", g_srcImage);
    imshow("【效果图窗口】", g_dstImage);
}

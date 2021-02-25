#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "mcvhand/sliper.h"

std::string Sliper::_winID;
static std::vector<SliperParam> gDParams;
static std::vector<int> sValue;
//push_back 分配内存方式要搞清

Sliper::Sliper( std::string name ) {
    _winID = name;
    cv::namedWindow( _winID ,1 );
    //cv::createTrackbar("亮   度：", "【效果图窗口】", &g_nBrightValue, 200, ContrastAndBright);
    //showWindow();
}

//鼠标拉动滑条事件回调函数,更新修改的参数
//由于不知道修改了那个参数,这里选择全部更新
static void  callBack( int sValue, void * ){
    //cout << "sValue: %d " << sValue << endl;
    //cout << "gDParams 0: sValue: %d " << *( gDParams[0].sValue )  << endl;
    //cout << "gDParams 1: sValue: %d " << *( gDParams[1].sValue )  << endl;
    for( int i = 0; i < gDParams.size(); i++ ){
        if( gDParams[ i ].dParam != NULL ) {
           *( gDParams[ i ].dParam )= *( gDParams[ i ].sValue ) * gDParams[ i ].scalar;
        }
        if( gDParams[ i ].iParam != NULL ) {
           *( gDParams[ i ].iParam ) = *( gDParams[ i ].sValue ) * gDParams[ i ].scalar;
        }
    }
}

/**
 * @brief   添加浮点型或整形数据参数滑条(必须二选1)
 * @param  [in]  id       对象ID
 * @param  [in]  iParam  整形目标对象地址(二选1 不传为NULL)
 * @param  [in]  dParam  浮点型对象地址(二选1 不传为NULL)
 * @param  [in]  sliperValue  滑条最大值
 * @param  [in]  scalar    目标对象值与滑条数值比例 *objParam = sliperValue * scalar(因为只找到整形滑条)
 * @retval
 * @note
 */
void Sliper::addParam(      const std::string id,
                             int* iParam,
                             double* dParam,
                             const int sliperValue,
                             const double scalar ){
    SliperParam param;
    param.id = id;
    param.scalar = scalar;

    if( dParam != NULL ){
        sValue.push_back( (int)*dParam );
    }
    else{
        sValue.push_back( *iParam );
    }
    param.sValue = &(sValue[ sValue.size() -1 ] );

    param.dParam = dParam;
    param.iParam = iParam;

    gDParams.push_back( param );
    //cout << gDParams.size() << endl;
    cv::createTrackbar( id , _winID ,  gDParams[ gDParams.size() -  1 ].sValue , sliperValue, callBack );

}

//--------------------测试函数---------------------------------------------
void testSliper( ){
    Sliper psliper("参数调整测试");
    double param1;
    int param2;
    psliper.addParam("参数1_D",NULL, &param1, 100, 0.1 );
    psliper.addParam("参数2_I",&param2,NULL, 100, 1);
    psliper.showWindow();
}




#ifndef TPDEPTH_H
#define TPDEPTH_H

#include <iostream>

class TPDepthHander
{
public:
    void smoothFilter( uint16_t* depthArray,
                       uint16_t* smoothDepthArray,
                       int width,
                       int height );
    void rectSmoothFilter( uint16_t * depthArray,
                           uint16_t* smoothDepthArray,
                           int width, int height,
                           int px, int py, int w, int h
                           );

};

#endif // TPDEPTH_H

#pragma once

#include <blend2d.h>
#include <ULIS>
#include "OdysseyVectorPointCubic.h"
#include "OdysseyVectorPathCubic.h"

class FOdysseyVectorCircle : public FOdysseyVectorPathCubic
{
    private:
        void DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags );
        FOdysseyVectorObject* PickShape( double iX, double iY, double iRadius );
        void UpdateShape();
 
        FOdysseyVectorPointCubic* mCubicPoint[4];
        FOdysseyVectorSegmentCubic* mCubicSegment[4];

    protected :
        double mRadiusX; 
        double mRadiusY; 

    public:
        ~FOdysseyVectorCircle();
        FOdysseyVectorCircle( std::string iName );
        FOdysseyVectorCircle( std::string iName, double iRadius );
        FOdysseyVectorCircle( std::string iName, double iRadiusX, double iRadiusY );
        void SetRadius( double iRadius );
        void SetRadius( double iRadiusX, double iRadiusY );
        double GetRadiusX();
        double GetRadiusY();
        FOdysseyVectorObject* CopyShape();
        FOdysseyVectorPathCubic* Convert();
};

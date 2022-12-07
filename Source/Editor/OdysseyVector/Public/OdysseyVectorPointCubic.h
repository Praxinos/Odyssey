#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorPoint.h"
#include "OdysseyVectorHandlePoint.h"

class FOdysseyVectorPointCubic : public FOdysseyVectorPoint
{
    private:
        FOdysseyVectorHandlePoint mCtrlPoint;

    protected:

    public:
        ~FOdysseyVectorPointCubic();
        FOdysseyVectorPointCubic();
        FOdysseyVectorPointCubic( double iX, double iY );
        FOdysseyVectorPointCubic( double iX, double iY, double iRadius );
        FOdysseyVectorHandlePoint& GetControlPoint();
        void Set( double iX, double iY );
        void Set( double iX, double iY, bool iBuildSegments );
        void Set( double iX, double iY, double iRadius, bool iBuildSegments );
        ::ULIS::FVec2D GetPerpendicularVector( bool iNormalize );
        void BuildSegments();
        virtual void SetRadius( double iRadius, bool iBuildSegments );
        void SmoothSegments( bool iBuildSegments );
};

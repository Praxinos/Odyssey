#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorHandleBucket.h"

class ODYSSEYVECTOR_API FOdysseyVectorBucket
{
    private:
        UOdysseyVectorObject& mParent;
        ::ULIS::FVec2D mCoords;
        uint32_t mColor;
        UOdysseyVectorHandleBucket* mCtrlPoint;

    public:
        ~FOdysseyVectorBucket();
        FOdysseyVectorBucket( UOdysseyVectorObject& iParent, uint32 iColor, double iX, double iY );
        void Draw( ::ULIS::FRectD& iRoi, uint64 iFlags );
        void SetCoords( double iX, double iY );
        ::ULIS::FVec2D& GetCoords();
        void SetColor( uint32 iColor );
        uint32 GetColor();
        bool PickHandle( double iX, double iY );
        UOdysseyVectorHandleBucket* GetHandle();
        UOdysseyVectorObject& GetParent();
        double GetHandleDotProduct();
};

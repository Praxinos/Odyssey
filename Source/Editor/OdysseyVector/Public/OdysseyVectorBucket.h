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
        FColor mColor;
        UOdysseyVectorHandleBucket* mCtrlPoint;
        bool mIsGradient;

    public:
        ~FOdysseyVectorBucket();
        FOdysseyVectorBucket( UOdysseyVectorObject& iParent, double iX, double iY, uint8 iR, uint8 iG, uint8 iB, uint8 iA );
        void Draw( ::ULIS::FRectD& iRoi, uint64 iFlags );
        void SetCoords( double iX, double iY );
        ::ULIS::FVec2D& GetCoords();
        void SetColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA );
        void SetGradient( bool mGradient );
        void SetColor2( uint8 iR, uint8 iG, uint8 iB, uint8 iA );
        FColor GetColor();
        bool PickHandle( double iX, double iY );
        UOdysseyVectorHandleBucket* GetHandle();
        UOdysseyVectorObject& GetParent();
        double GetHandleDotProduct();
};

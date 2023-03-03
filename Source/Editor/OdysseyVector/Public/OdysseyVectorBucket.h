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
        FColor mGradientColor0;
        FColor mGradientColor1;
        void Reshape();

    public:
        static const uint32 PICKNONE = 0;
        static const uint32 PICKBUCKET = 1;
        static const uint32 PICKCROSS = 2;
        static const uint32 PICKHANDLE = 3;

        ~FOdysseyVectorBucket();
        FOdysseyVectorBucket( UOdysseyVectorObject& iParent, double iX, double iY );
        void SetColor(uint8 iR, uint8 iG, uint8 iB, uint8 iA );
        void Draw( ::ULIS::FRectD& iRoi, uint64 iFlags );
        void SetCoords( double iX, double iY );
        ::ULIS::FVec2D GetCoords();
        bool IsGradient();
        FColor& GetGradientColor0();
        FColor& GetGradientColor1();
        void SetGradient( bool iIsGradient );
        void SetGradientColors( uint8 iR0, uint8 iG0, uint8 iB0, uint8 iA0, uint8 iR1, uint8 iG1, uint8 iB1, uint8 iA1 );
        FColor& GetColor();
        void Copy( FOdysseyVectorBucket* iDestinationBucket );
        uint32 Pick( double iX, double iY );
        UOdysseyVectorHandleBucket* PickHandle( double iX, double iY );
        UOdysseyVectorHandleBucket* GetHandle();
        UOdysseyVectorObject& GetParent();
        double GetHandleDotProduct();
        void DrawCross();
};

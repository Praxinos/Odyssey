#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorHandleBucket.h"

class ODYSSEYVECTOR_API FOdysseyVectorBucket
{
    public:
        static const uint32 PICKNONE   = 0;
        static const uint32 PICKBUCKET = 1;
        static const uint32 PICKCROSS  = 2;
        static const uint32 PICKHANDLE = 3;

        ~FOdysseyVectorBucket();
        FOdysseyVectorBucket( UOdysseyVectorObject& iParent, double iX, double iY );

        /**
         * @brief Set this bucket's solid color
         * @param iR red
         * @param iG green
         * @param iB blue
         * @param iA alpha
         */
        void SetColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA );

        /**
         * @brief Draw this bucket (used for HUD).
         * @param iRoi region-of-interest
         * @param iFlags drawing flags
         */
        void Draw( ::ULIS::FRectD& iRoi, uint64 iFlags );

        /**
         * @brief Set this bucket's coordinates relative to the parent object
         * @param iX position on X axis
         * @param iY position on Y axis
         */
        void SetCoords( double iX, double iY );

        void SetGradient( bool iIsGradient );
        void SetGradientColor0( uint8 iR0, uint8 iG0, uint8 iB0, uint8 iA0 );
        void SetGradientColor1( uint8 iR1, uint8 iG1, uint8 iB1, uint8 iA1 );

        /**
         * @brief Get this bucket's coordinates
         */
        ::ULIS::FVec2D GetCoords();

        /**
         * @brief Tell whether or not the bucket is in gradient mode
         * @return true or false
         */
        bool IsGradient();

        /**
         * @brief Get gradient color 0
         * @return a reference to the gradient color 0
         */
        FColor& GetGradientColor0();

        /**
         * @brief Get gradient color 1
         * @return a reference to the gradient color 1
         */
        FColor& GetGradientColor1();

        /**
         * @brief Get gradient rotation in degrees. This is computed from the handle position.
         * @return rotation angle in degrees.
         */
        double GetGradientRotationInDegrees();

        /**
         * @brief Get solid color
         * @return a reference to the solid color
         */
        FColor& GetColor();

        /**
         * @brief Copy this bucket values to destination bucket
         * @param iDestinationBucket the destination bucket
         */
        void Copy( FOdysseyVectorBucket* iDestinationBucket );


        uint32 Pick( double iX, double iY );
        FOdysseyVectorHandleBucket* PickHandle( double iX, double iY );
        FOdysseyVectorHandleBucket* GetHandle();
        UOdysseyVectorObject& GetParent();
        double GetHandleDotProduct();
        void DrawCross();

    private:
        UOdysseyVectorObject& mParent;
        ::ULIS::FVec2D mCoords;
        FColor mColor;
        FOdysseyVectorHandleBucket mCtrlPoint;
        bool mIsGradient;
        FColor mGradientColor0;
        FColor mGradientColor1;
};

#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorHandleBucket.h"

class FOdysseyVectorObject;

enum FBucketDrawingFlags
{
    BUCKET = 1,
    PELLET = 2
};

class ODYSSEYVECTOR_API FOdysseyVectorBucket : public FOdysseyVectorPoint
{
    public:
        ~FOdysseyVectorBucket();
        FOdysseyVectorBucket( FOdysseyVectorObject& iParent, double iX, double iY,  bool iPropagated );
        FOdysseyVectorBucket( FOdysseyVectorObject& iParent );
        /**
         * @brief Set this bucket's solid color
         * @param iR red
         * @param iG green
         * @param iB blue
         * @param iA alpha
         */
        void SetColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA );

        void SetColor( FColor& iColor );

        /**
         * @brief Draw this bucket (used for HUD).
         * @param iRoi region-of-interest
         * @param iFlags drawing flags
         */
        void Draw( FBucketDrawingFlags iDrawingFlags );

        void SetGradient( bool iIsGradient );

        void SetGradientColor0( FColor& iColor );
        void SetGradientColor1( FColor& iColor );
        void SetGradientColor0( uint8 iR0, uint8 iG0, uint8 iB0, uint8 iA0 );
        void SetGradientColor1( uint8 iR1, uint8 iG1, uint8 iB1, uint8 iA1 );

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

        /**
         * @brief Tell whether or not this bucket is on the coordinates passed as parameters.
         * @param iX world x coordinate on X axis
         * @param iY world y coordinate on Y axis
         * @return PICKNONE, PICKBUCKET, PICKCROSS or PICKHANDLE.
         */
        uint32 Pick( double iWorldX, double iWorldY );

        /**
         * @brief Tell whether or not this bucket's handle is on the coordinates passed as parameters.
         * @param iX world x coordinate on X axis
         * @param iY world y coordinate on Y axis
         * @return a pointer to this bucket's handle.
         */
        FOdysseyVectorHandleBucket* PickHandle( double iWorldX, double iWorldY );

        /**
         * @brief Get the bucket's handle.
         * @return a pointer to this bucket's handle.
         */
        FOdysseyVectorHandleBucket* GetHandle();

        /**
         * @brief Get the bucket's parent object.
         * @return a reference to this bucket's parent object.
         */
        FOdysseyVectorObject& GetParent();

        /**
         * @brief Get the dot product between the vector to the handle and a horizontal vector.
         * @return the dot product between the vector to the handle and a horizontal vector.
         */
        double GetHandleDotProduct();

        void SetPropagated( bool iPropagated );
        bool IsPropagated();
        void SetX( double iX );
        void SetY( double iY );
        void Set( double iX, double iY );
        void Invalidate();

    private:
        void DrawBucket( FBucketDrawingFlags iDrawingFlags );
        void DrawPellet( FBucketDrawingFlags iDrawingFlags );

    public:
        static const uint32 PICKNONE   = 0;
        static const uint32 PICKBUCKET = 1;
        static const uint32 PICKCROSS  = 2;
        static const uint32 PICKPROPAGATED = 3;
        static const uint32 PICKHANDLE = 4;

    protected:
        FOdysseyVectorObject& mParent;
        FColor mColor;
        FOdysseyVectorHandleBucket mCtrlPoint;
        bool mPropagated;
        bool mIsGradient;
        FColor mGradientColor0;
        FColor mGradientColor1;
};

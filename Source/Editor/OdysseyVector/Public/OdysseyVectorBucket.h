#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include <OdysseyVectorPoint.h>
#include "Palette/OdysseyPaletteEntry.h"

class FOdysseyVectorObject;

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
        void SetSolidColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA );

        void SetSolidColor( FColor& iColor );

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
         * @brief Get the bucket's parent object.
         * @return a reference to this bucket's parent object.
         */
        FOdysseyVectorObject& GetParent();

        void SetPropagated( bool iPropagated );
        bool IsPropagated();
        virtual void SetX( double iX ) override;
        virtual void SetY( double iY ) override; 
        virtual void Set( double iX, double iY ) override;
        virtual void Set( double iX, double iY, double iRadius ) override;
        void Invalidate();
        double GetRotation();
        void SetRotation( double iRotation );
        FColor& GetSolidColor();

        void SetPaletteEntry( UOdysseyPaletteEntry* iPaletteEntry );
        UOdysseyPaletteEntry* GetPaletteEntry();
        FPaletteEntryDescription& GetPaletteEntryDescription();

    protected:
        FOdysseyVectorObject& mParent;
        FColor mSolidColor;
        double mRotation;
        bool mPropagated;
        bool mIsGradient;
        FColor mGradientColor0;
        FColor mGradientColor1;

        UOdysseyPaletteEntry* mPaletteEntry = nullptr;
        /* The Palette Entry associated with this vector object, if any. Else, the Guid inside will be 0 or invalid */
        FPaletteEntryDescription mPaletteEntryDescription;
};

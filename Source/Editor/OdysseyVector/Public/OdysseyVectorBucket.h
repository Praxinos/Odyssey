#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include <OdysseyVectorPoint.h>

class FOdysseyVectorObject;
class UOdysseyPaletteEntry;

class ODYSSEYVECTOR_API FOdysseyVectorBucket : public FOdysseyVectorPoint
{
    public:
        ~FOdysseyVectorBucket();
        FOdysseyVectorBucket( FOdysseyVectorObject& iOwner, double iX, double iY,  bool iPropagated );
        FOdysseyVectorBucket( FOdysseyVectorObject& iOwner );
        /**
         * @brief Set this bucket's solid color
         * @param iR red
         * @param iG green
         * @param iB blue
         * @param iA alpha
         */
        void SetSolidColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA );

        /**
         * @brief Set this bucket's solid color
         * @param iColor color in UE's FColor format
         */
        void SetSolidColor( FColor& iColor );

        /**
         * @brief Tell this bucket to use gradient mode
         * @param iIsGradient true or false
        */
        void SetGradient( bool iIsGradient );

        /**
         * @brief Set gradient color 0
         * @param iColor color in UE's FColor format
        */
        void SetGradientColor0( FColor& iColor );

        /**
         * @brief Set gradient color 1
         * @param iColor color in UE's FColor format
        */
        void SetGradientColor1( FColor& iColor );

        /**
         * @brief Set gradient color 0
         * @param iR red
         * @param iG green
         * @param iB blue
         * @param iA alpha
        */
        void SetGradientColor0( uint8 iR, uint8 iG, uint8 iB, uint8 iA );

        /**
         * @brief Set gradient color 1
         * @param iR red
         * @param iG green
         * @param iB blue
         * @param iA alpha
        */
        void SetGradientColor1( uint8 iR, uint8 iG, uint8 iB, uint8 iA );

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
         * @brief Get color relative to the current mode (palette or solid color)
         * @return a reference to the color
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
        FOdysseyVectorObject& GetOwner();

        void SetPropagated( bool iPropagated );
        bool IsPropagated();
        virtual void SetCoords( double iX, double iY, double iRadius ) override;
        void Invalidate();
        double GetRotation();
        void SetRotation( double iRotation );
        FColor& GetSolidColor();

        void SetPaletteEntry( UOdysseyPaletteEntry* iPaletteEntry );
        UOdysseyPaletteEntry* GetPaletteEntry();

    protected:
        FOdysseyVectorObject& mOwner;
        FColor mSolidColor;
        double mRotation;
        bool mPropagated;
        bool mIsGradient;
        FColor mGradientColor0;
        FColor mGradientColor1;

        /* The Palette Entry associated with this vector object, if any*/
        UOdysseyPaletteEntry* mPaletteEntry = nullptr;
};

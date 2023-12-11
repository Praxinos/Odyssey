#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include <OdysseyVectorPoint.h>

#include "OdysseyVectorBucket.generated.h"

class FOdysseyVectorObject;
class UOdysseyPaletteEntry;

UENUM()
enum class eBucketColorMode : uint8
{
    SolidColor = 0,
    LinearGradient = 1,
    RadialGradient = 2,
    Palette = 3
};

UENUM()
enum class eBucketSpreadingPolicy : uint8
{
    Group = 0,
    Local = 1
};

class ODYSSEYVECTOR_API FOdysseyVectorBucket : public FOdysseyVectorPoint
{
    private:
        static const uint32 mStaticClass = 0xcb5fa05c; // value is crc32 FOdysseyVectorBucket

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };

        ~FOdysseyVectorBucket();
        FOdysseyVectorBucket( FOdysseyVectorObject* iOwner, FOdysseyVectorBucket* iImportFrom );
        FOdysseyVectorBucket( FOdysseyVectorObject* iOwner, double iX, double iY,  bool iPropagated );
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
        FColor GetColor();

        /**
         * @brief Copy this bucket values to destination bucket. Note: no invalidation will happen
         *        It'll have to be done manually.
         * @param iDestinationBucket the destination bucket
         */
        void Copy( FOdysseyVectorBucket* iDestinationBucket );

        /**
         * @brief Get the bucket's parent object.
         * @return a pointer to this bucket's parent object.
         */
        FOdysseyVectorObject* GetOwner();

        virtual void SetCoords( double iX, double iY, double iRadius ) override;
        void SetColorMode( eBucketColorMode iColorMode );
        void SetSpreadingPolicy( eBucketSpreadingPolicy iSpreadingPolicy );

        eBucketColorMode GetColorMode();
        eBucketSpreadingPolicy GetSpreadingPolicy();

        void SetPropagated( bool iPropagated );
        bool IsPropagated();
        void SetSelected( bool iIsSelected );
        bool IsSelected();

        void Invalidate();
        double GetRotation();
        void SetRotation( double iRotation );
        FColor& GetSolidColor();

        void SetPaletteEntry( UOdysseyPaletteEntry* iPaletteEntry );
        UOdysseyPaletteEntry* GetPaletteEntry();

        ::ULIS::FVec2D& GetRadialOffset();
        void SetRadialOffset( const ::ULIS::FVec2D& iRadialOffset );

        void SetRadialRadius( double iRadialRadius );
        double GetRadialRadius();

    protected:
        FOdysseyVectorObject* mOwner;
        bool mIsSelected;

    public:
        eBucketColorMode mColorMode;
        eBucketSpreadingPolicy mSpreadingPolicy;
        FColor mSolidColor;
        double mRotation;
        bool bPropagated;
        FColor mGradientColor0;
        FColor mGradientColor1;
        double mRadialRadius; // radius in radial mode
        ::ULIS::FVec2D mRadialOffset; // distance from the radial-gradient to the bucket.
        /* The Palette Entry associated with this vector object, if any*/
        UOdysseyPaletteEntry* mPaletteEntry = nullptr;
};

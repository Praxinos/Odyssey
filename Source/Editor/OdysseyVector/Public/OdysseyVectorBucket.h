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

        /**
         * @brief Constructor to create a new bucket with the same settings as an existing bucket.
         * @param iOwner owner of this bucket
         * @param iImportFrom the bucket to copy settings from
         */
        FOdysseyVectorBucket( FOdysseyVectorObject* iOwner, FOdysseyVectorBucket* iImportFrom );

        /**
         * @brief Constructor to create a new bucket.
         * @param iOwner owner of this bucket
         * @param iX x-axis coordinates
         * @param iY y-axis coordinates
         * @param iPropagated propagation policy
         */
        FOdysseyVectorBucket( FOdysseyVectorObject* iOwner, double iX, double iY,  bool iPropagated );

        /**
         * @brief Copy this bucket values to destination bucket. Note: no invalidation will happen
         *        It'll have to be done manually.
         * @param iDestinationBucket the destination bucket
         */
        void Copy( FOdysseyVectorBucket* iDestinationBucket );

        /**
         * @brief Get color relative to the current mode (palette or solid color)
         * @return a reference to the color
         */
        FColor GetColor();

        /**
         * @brief Set this bucket's color mode
         * @return the color mode
         */
        eBucketColorMode GetColorMode();

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
         * @brief Get the bucket's parent object.
         * @return a pointer to this bucket's parent object.
         */
        FOdysseyVectorObject* GetOwner();

        /**
         * @brief Get this bucket's palette entry
         * @return a pointer to the palette entry
         */
        UOdysseyPaletteEntry* GetPaletteEntry();

        /**
         * @brief Get the position of the radial gradient's center, relative to the bucket position.
         * @return a reference the relative coordinates
         */
        ::ULIS::FVec2D& GetRadialOffset();

        /**
         * @brief Get the radius of the radial gradient
         * @param the radius
         */
        double GetRadialRadius();

        /**
         * @brief Get this bucket's rotation angle for linear gradient
         * @return the angle in radians
         */
        double GetRotation();

        /**
         * @brief Set this bucket's spreading policy
         * @return the spreading policy
         */
        eBucketSpreadingPolicy GetSpreadingPolicy();

        /**
         * @brief Get this bucket's solid color
         * @return color in UE's FColor format
         */
        FColor& GetSolidColor();

        /**
         * @brief Invalidate the bucket and its owner object (mark them ready for update).
         */
        void Invalidate();

        /**
         * @brief Get this bucket's propagation policy.
         * @return true or false
         */
        bool IsPropagated();

        /**
         * @brief Get this bucket's selection status
         * @return true or false
         */
        bool IsSelected();

        /**
         * @brief Set this bucket's color mode
         * @param iColorMode see enum eBucketColorMode
         */
        void SetColorMode( eBucketColorMode iColorMode );

        /**
         * @brief Set this bucket's coordinates
         * @param iX x coordinate
         * @param iY y coordinate
         * @param iRadius ignored - can be 0.0f
         */
        virtual void SetCoords( double iX, double iY, double iRadius ) override;

        /**
         * @brief Set gradient color 0
         * @param iColor color in UE's FColor format
        */
        void SetGradientColor0( FColor& iColor );

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
         * @param iColor color in UE's FColor format
        */
        void SetGradientColor1( FColor& iColor );

        /**
         * @brief Set gradient color 1
         * @param iR red
         * @param iG green
         * @param iB blue
         * @param iA alpha
        */
        void SetGradientColor1( uint8 iR, uint8 iG, uint8 iB, uint8 iA );

        /**
         * @brief Set this bucket's palette entry
         * @param iPaletteEntry a pointer to the palette entry
         */
        void SetPaletteEntry( UOdysseyPaletteEntry* iPaletteEntry );

        /**
         * @brief Set this bucket's propagation policy. A bucket will propagate
         *        its color to neighbour cycles.
         * @param iPropagated true or false
         */
        void SetPropagated( bool iPropagated );

        /**
         * @brief Set the position of the radial gradient's center, relative to the bucket position.
         * @param iRadialOffset a reference the relative coordinates
         */
        void SetRadialOffset( const ::ULIS::FVec2D& iRadialOffset );

        /**
         * @brief Set the radius of the radial gradient
         * @param iRadialRadius the desired radius
         */
        void SetRadialRadius( double iRadialRadius );

        /**
         * @brief Set this bucket's selection flag. Only sets the flag but does not select the bucket.
         * @param iIsSelected true or false
         */
        void SetSelected( bool iIsSelected );

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
         * @brief Set this bucket's spreading policy
         * @param iColorMode see enum eBucketSpreadingPolicy
         */
        void SetSpreadingPolicy( eBucketSpreadingPolicy iSpreadingPolicy );

        /**
         * @brief Set this bucket's rotation angle for linear gradient
         * @param iRotation the angle in radians
         */
        void SetRotation( double iRotation );

    protected:
        FOdysseyVectorObject* mOwner;
        bool bSelected;
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
        UOdysseyPaletteEntry* mPaletteEntry;
};

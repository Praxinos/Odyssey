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

USTRUCT()
struct FBucketParam
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category="Bucket")
    eBucketColorMode ColorMode;

    UPROPERTY(EditAnywhere,Category="Bucket")
    eBucketSpreadingPolicy SpreadingPolicy;

    UPROPERTY(EditAnywhere,Category="Bucket")
    FColor SolidColor;

    UPROPERTY(EditAnywhere,Category="Bucket")
    double Rotation;

    UPROPERTY(EditAnywhere,Category="Bucket")
    bool Propagated;

    UPROPERTY(EditAnywhere,Category="Bucket")
    FColor GradientColor0;

    UPROPERTY(EditAnywhere,Category="Bucket")
    FColor GradientColor1;
};

class ODYSSEYVECTOR_API FOdysseyVectorBucket : public FOdysseyVectorPoint
{
    public:
        ~FOdysseyVectorBucket();
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

        void ImportParam( FBucketParam& iBuketParam );
        virtual void SetCoords( double iX, double iY, double iRadius ) override;
        void SetColorMode( eBucketColorMode iColorMode );
        void SetSpreadingPolicy( eBucketSpreadingPolicy iSpreadingPolicy );

        eBucketColorMode GetColorMode();
        eBucketSpreadingPolicy GetSpreadingPolicy();

        void SetPropagated( bool iPropagated );
        bool IsPropagated();
        void Invalidate();
        double GetRotation();
        void SetRotation( double iRotation );
        FColor& GetSolidColor();

        void SetPaletteEntry( UOdysseyPaletteEntry* iPaletteEntry );
        UOdysseyPaletteEntry* GetPaletteEntry();

    protected:
        FOdysseyVectorObject* mOwner;

        /* The Palette Entry associated with this vector object, if any*/
        UOdysseyPaletteEntry* mPaletteEntry = nullptr;

    public:
        FBucketParam mBucketParam;
};

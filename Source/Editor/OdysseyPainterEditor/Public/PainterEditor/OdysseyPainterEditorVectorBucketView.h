#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorBucket.h"

#include "OdysseyPainterEditorVectorBucketView.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorBucketView : public UObject
{
    public:
        GENERATED_BODY()

    public:
        ~UOdysseyPainterEditorVectorBucketView();
        UOdysseyPainterEditorVectorBucketView();
        UOdysseyPainterEditorVectorBucketView( FOdysseyVectorBucket* iBucket );

        void Update( FOdysseyVectorBucket* iBucket );
        void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;

    protected:
        virtual void ImportParam();
        virtual void PropertyChanged( const FName& iPropertyName, const FName& iCategory );

    protected:
        FOdysseyVectorBucket* mBucket;

    public:
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

        UPROPERTY(EditAnywhere,Category="Bucket")
        double RadialRadius; // radius in radial mode
};

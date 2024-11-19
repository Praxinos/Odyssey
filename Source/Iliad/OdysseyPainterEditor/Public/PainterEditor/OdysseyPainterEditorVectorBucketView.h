// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorBucket.h"

#include "OdysseyPainterEditorVectorBucketView.generated.h"

class FOdysseyPainterEditor;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorBucketView : public UObject
{
    public:
        GENERATED_BODY()

    public:
        ~UOdysseyPainterEditorVectorBucketView();
        UOdysseyPainterEditorVectorBucketView();
        UOdysseyPainterEditorVectorBucketView( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket );

        void Update( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket );
        void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;

    protected:
        virtual void ImportParam();
        virtual void PropertyChanged( const FName& iPropertyName
                                    , const FName& iMemberPropertyName
                                    , const FName& iCategory );

    protected:
        FOdysseyPainterEditor* mEditor;
        FOdysseyVectorBucket* mBucket;

    public:
        UPROPERTY(EditAnywhere, Category="Bucket")
        eBucketColorMode ColorMode;

        UPROPERTY(EditAnywhere,Category="Bucket")
        eBucketSpreadingPolicy SpreadingPolicy;

        UPROPERTY(EditAnywhere,Category="Bucket", meta = (EditCondition = "(ColorMode == eBucketColorMode::SolidColor)", EditConditionHides))
        FColor SolidColor;

        UPROPERTY(EditAnywhere,Category="Bucket")
        double Rotation;

        UPROPERTY(EditAnywhere,Category="Bucket")
        bool Propagated;

        UPROPERTY(EditAnywhere,Category="Bucket", meta = (EditCondition = "(ColorMode == eBucketColorMode::LinearGradient) || (ColorMode == eBucketColorMode::RadialGradient)", EditConditionHides))
        FColor GradientColor0;

        UPROPERTY(EditAnywhere,Category="Bucket", meta = (EditCondition = "(ColorMode == eBucketColorMode::LinearGradient) || (ColorMode == eBucketColorMode::RadialGradient)", EditConditionHides))
        FColor GradientColor1;

        UPROPERTY(EditAnywhere,Category="Bucket")
        double RadialRadius; // radius in radial mode
};

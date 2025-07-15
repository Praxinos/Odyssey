// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorBucket.h"
#include "OdysseyPainterEditorVectorObjectView.h" // for FPaletteEntrySelection
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

        FOdysseyPainterEditor* GetEditor();

    protected:
        virtual void ImportParam();
        virtual void PropertyChanged( const FName& iPropertyName
                                    , const FName& iMemberPropertyName
                                    , const FName& iCategory );

    protected:
        FOdysseyPainterEditor* mEditor;
        FOdysseyVectorBucket* mBucket;

    public:
        UPROPERTY( EditAnywhere
                 , Category = "Bucket"
                 , meta = ( ToolTip  = "Color Mode" ) )
        eBucketColorMode ColorMode;

        UPROPERTY( EditAnywhere,
                   Category = "Bucket",
                   meta = ( EditCondition = "(ColorMode == eBucketColorMode::Palette )"
                          , EditConditionHides ) )
        FPaletteEntrySelection PaletteSelection;

        UPROPERTY( EditAnywhere
                 , Category = "Bucket"
                 , meta = ( ToolTip  = "Solid Color"
                          , EditCondition = "(ColorMode == eBucketColorMode::SolidColor)"
                          , EditConditionHides ) )
        FColor SolidColor;

        UPROPERTY( EditAnywhere
                 , Category = "Bucket"
                 , meta = ( ToolTip  = "Propagated" ) )
        bool Propagated;

        UPROPERTY( EditAnywhere
                 , Category = "Bucket"
                 , meta = ( ToolTip  = "Gradient Color 0"
                          , EditCondition = "(ColorMode == eBucketColorMode::LinearGradient) || (ColorMode == eBucketColorMode::RadialGradient)"
                          , EditConditionHides ) )
        FColor GradientColor0;

        UPROPERTY( EditAnywhere
                 , Category = "Bucket"
                 , meta = ( ToolTip  = "Gradient Color 1"
                          , EditCondition = "(ColorMode == eBucketColorMode::LinearGradient) || (ColorMode == eBucketColorMode::RadialGradient)"
                          , EditConditionHides ) )
        FColor GradientColor1;

        UPROPERTY( EditAnywhere
                 , Category = "Bucket"
                 , meta = ( ToolTip  = "Radial Radius"
                          , EditCondition = "(ColorMode == eBucketColorMode::RadialGradient)"
                          , EditConditionHides ) )
        double RadialRadius; // radius in radial mode
};

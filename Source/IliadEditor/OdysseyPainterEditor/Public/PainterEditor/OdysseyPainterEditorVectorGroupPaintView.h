// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyPainterEditorVectorObjectView.h"

#include "OdysseyPainterEditorVectorGroupPaintView.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorGroupPaintView : public UOdysseyPainterEditorVectorObjectView
{
    public:
        GENERATED_BODY()

    public:
        ~UOdysseyPainterEditorVectorGroupPaintView();
        UOdysseyPainterEditorVectorGroupPaintView();

    protected:
        virtual void ImportParam() override;
        virtual void PropertyChanged( const FName& iPropertyName
                                    , const FName& iMemberPropertyName
                                    , const FName& iCategory ) override;

    public:
        UPROPERTY( EditAnywhere
                 , Category = PaintGroup
                 , meta = ( ToolTip = "Painted" ) )
        bool Painted;

        UPROPERTY( EditAnywhere
                 , Category = PaintGroup
                 , meta = ( ToolTip = "Monochrome" ) )
        bool Monochrome;

        UPROPERTY( EditAnywhere
                 , Category = PaintGroup
                 , meta = ( ToolTip = "MonochromeColor" ) )
        FColor MonochromeColor;

        UPROPERTY( EditAnywhere
                 , Category = GapDetection
                 , meta = ( ToolTip  = "Gap Tolerance"
                          , ClampMin = "0.0"
                          , UIMin    = "0.0" ) )
        double GapTolerance;

        UPROPERTY( EditAnywhere
                 , Category = GapDetection
                 , meta = ( ToolTip  = "Gap Detection Scheme" ) )
        eGapDetectionScheme GapDetectionScheme;

        UPROPERTY( EditAnywhere
                 , Category = GapDetection
                 , meta = ( ToolTip  = "Extended Segment Scheme"
                          , EditCondition = "(GapDetectionScheme == eGapDetectionScheme::SegmentExtension)"
                          , EditConditionHides ) )
        eSegmentExtensionScheme SegmentExtensionScheme;

        UPROPERTY( EditAnywhere
                 , Category = GapDetection
                 , meta = ( ToolTip  = "Simplify resulting graph if true. Creates less cycles but make gap detection harder"
                          , EditCondition = "(GapDetectionScheme == eGapDetectionScheme::SegmentExtension)"
                          , EditConditionHides ) )
        bool SegmentExtensionSimplified;

        UPROPERTY( EditAnywhere
                 , Category = PaintGroup
                 , meta = ( ToolTip = "Intersects Canvas" ) )
        bool IntersectsCanvas;

        // commented out: now handled at layer level
        //UPROPERTY( EditAnywhere, Category=PaintGroup )
        //bool Wireframe;

        UPROPERTY( EditAnywhere
                 , Category = PaintGroup
                 , meta = ( ToolTip = "Wireframe Color" ) )
        FColor WireframeColor;

        UPROPERTY( EditAnywhere
                 , Category = Advanced
                 , meta = ( ToolTip = "Realtime" ) )
        bool Realtime; // relatime updates

        //UPROPERTY( EditAnywhere, Category=Advanced )
        bool Multithreaded;
};

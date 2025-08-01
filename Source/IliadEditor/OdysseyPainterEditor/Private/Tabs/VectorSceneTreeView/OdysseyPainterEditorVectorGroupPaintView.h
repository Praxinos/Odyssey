// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyPainterEditorVectorGroupView.h"

#include "OdysseyPainterEditorVectorGroupPaintView.generated.h"

UCLASS( HideCategories = "Hidden" )
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorGroupPaintView : public UOdysseyPainterEditorVectorGroupView
{
    // we use a bitfields in case we need more than 64 flags
    typedef union {
        struct
        {
            uint32 Painted : 1;
            uint32 Monochrome : 1;
            uint32 MonochromeColor : 1;
            uint32 GapTolerance : 1;
            uint32 WireframeColor : 1;
            uint32 IntersectsCanvas : 1;
            uint32 GapDetectionScheme : 1;
            uint32 SegmentExtensionScheme : 1;
            uint32 SegmentExtensionSimplified : 1;
            uint32 Realtime : 1;
        };
        uint8 raw[1];
    } PropertyBits;

    public:
        GENERATED_BODY()

    public:
        ~UOdysseyPainterEditorVectorGroupPaintView();
        UOdysseyPainterEditorVectorGroupPaintView();

    public:
        virtual bool GetPropertyBit( const FName& iPropertyName ) override;
        virtual void SetPropertyBit( const FName& iPropertyName
                                   , const FName& iMemberPropertyName
                                   , const FName& iCategory
                                   , bool iState ) override;
        virtual bool HasAnyPropertyBit() override;

    protected:
        virtual void ImportParam( const std::list<FOdysseyVectorObject*>& iFocusedObjectList ) override;
        virtual void ClearPropertyBits() override;
        virtual void ApplyPropertyBits( FOdysseyVectorObject* iObject ) override;

    private:
        PropertyBits mGroupPaintPropertyBits;

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

        // commented out: now handled at layer level
        //UPROPERTY( EditAnywhere, Category=PaintGroup )
        //bool Wireframe;

        UPROPERTY( EditAnywhere
                 , Category = PaintGroup
                 , meta = ( ToolTip = "Wireframe Color" ) )
        FColor WireframeColor;

        UPROPERTY( EditAnywhere
                 , Category = PaintGroup
                 , meta = ( ToolTip = "Intersects Canvas" ) )
        bool IntersectsCanvas;

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
                 , Category = Advanced
                 , meta = ( ToolTip = "Realtime" ) )
        bool Realtime; // relatime updates

        //UPROPERTY( EditAnywhere, Category=Advanced )
        bool Multithreaded;
};

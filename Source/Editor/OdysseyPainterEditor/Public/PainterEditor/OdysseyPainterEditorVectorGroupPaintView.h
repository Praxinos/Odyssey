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
        virtual uint64 PropertyChanged( const FName& iPropertyName
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
                 , Category = PaintGroup
                 , meta = ( ToolTip  = "Gap Tolerance"
                          , ClampMin = "0.0"
                          , UIMin    = "0.0" ) )
        double GapTolerance;

        UPROPERTY( EditAnywhere
                 , Category = PaintGroup
                 , meta = ( ToolTip = "Intersects Canvas" ) )
        bool IntersectsCanvas;

        // commented out: now handled at layer level
        //UPROPERTY( EditAnywhere, Category = PaintGroup )
        //bool Wireframe;

        UPROPERTY( EditAnywhere
                 , Category = PaintGroup
                 , meta = ( ToolTip = "Wireframe Color" ) )
        FColor WireframeColor;

        UPROPERTY( EditAnywhere
                 , Category = Advanced
                 , meta = ( ToolTip = "Realtime" ) )
        bool Realtime; // relatime updates

        //UPROPERTY( EditAnywhere, Category = Advanced )
        bool Multithreaded;
};

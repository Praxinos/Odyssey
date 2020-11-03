// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Types/SlateStructs.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SLeafWidget.h"

#include "Layout/Visibility.h"
#include "Widgets/SWidget.h"
#include "Layout/Margin.h"
#include "Layout/Children.h"
#include "Widgets/SPanel.h"


/////////////////////////////////////////////////////
// SOdysseyCursorWidget
class ODYSSEYWIDGETS_API SOdysseyCursorWidget : public SLeafWidget
{
private:
    typedef SOdysseyCursorWidget tSuperClass;

public:
    SLATE_BEGIN_ARGS( SOdysseyCursorWidget )
        : _DesiredWidth(    50 )
        , _DesiredHeight(   50 )
        , _Icon(            NULL )
        {}
        SLATE_ARGUMENT(     int,                    DesiredWidth    )
        SLATE_ARGUMENT(     int,                    DesiredHeight   )
        SLATE_ARGUMENT(     const FSlateBrush*,     Icon            )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    SOdysseyCursorWidget();
    virtual ~SOdysseyCursorWidget();
    void Construct(const FArguments& InArgs);

private:
    // Public SWidget overrides
    virtual int32 OnPaint( const FPaintArgs& Args
                         , const FGeometry& AllottedGeometry
                         , const FSlateRect& MyCullingRect
                         , FSlateWindowElementList& OutDrawElements
                         , int32 LayerId
                         , const FWidgetStyle& InWidgetStyle
                         , bool bParentEnabled ) const override;

private:
    // Private sWidget overrides
    virtual FVector2D ComputeDesiredSize(float) const override;

private:
    // Private Data Members
    int                     DesiredWidth;
    int                     DesiredHeight;
    const  FSlateBrush*     Icon;
};

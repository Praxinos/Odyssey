// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SCompoundWidget.h"
#include "Framework/SlateDelegates.h"

class UOdysseyPainterEditorRasterDrawingTool;
class UOdysseyBrush;

/////////////////////////////////////////////////////
// SOdysseyPainterEditorRasterDrawingToolBrushSelector
class ODYSSEYPAINTEREDITOR_API SOdysseyPainterEditorRasterDrawingToolBrushSelector
    : public SCompoundWidget
{
    typedef SCompoundWidget         tSuperClass;
    typedef SOdysseyPainterEditorRasterDrawingToolBrushSelector   tSelf;

public:
    ~SOdysseyPainterEditorRasterDrawingToolBrushSelector();

    // Construction / Destruction
    SLATE_BEGIN_ARGS( SOdysseyPainterEditorRasterDrawingToolBrushSelector )
        {}
        SLATE_ARGUMENT(UOdysseyPainterEditorRasterDrawingTool*, Tool)
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs );

private:
    void OnBrushSelected(UOdysseyBrush* iBrush);
    void OnToolBrushChanged();

    FReply OnExpanderArrowClicked();
    const FSlateBrush* GetExpanderArrowImage() const;

private:
    // Private data members
    UOdysseyPainterEditorRasterDrawingTool*                mTool;
    TSharedPtr<IDetailsView> mDetailsView;
    TSharedPtr<SButton> mExpanderArrow;
    bool mIsExpanded;
};

// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
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

private:
    // Private data members
    UOdysseyPainterEditorRasterDrawingTool*                mTool;
    TSharedPtr<IDetailsView> mDetailsView;
};


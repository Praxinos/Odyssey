// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Framework/SlateDelegates.h"
#include "IDetailsView.h"

class UOdysseyPainterEditorTool;

/////////////////////////////////////////////////////
// SOdysseyPainterEditorToolOptions
class ODYSSEYPAINTEREDITOR_API SOdysseyPainterEditorToolOptions
    : public SCompoundWidget
{
    typedef SCompoundWidget         tSuperClass;
    typedef SOdysseyPainterEditorToolOptions   tSelf;

public:
    // Construction / Destruction
    SLATE_BEGIN_ARGS( SOdysseyPainterEditorToolOptions )
        : _Tool(nullptr)
        {}
        SLATE_ATTRIBUTE(UOdysseyPainterEditorTool*, Tool)
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs );
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
    // Private data members
    TAttribute<UOdysseyPainterEditorTool*> mTool;
    UOdysseyPainterEditorTool* mDisplayedTool;
    TSharedPtr<IDetailsView> mDetailsView;
};

// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Framework/SlateDelegates.h"

class UOdysseyDrawingTool;

/////////////////////////////////////////////////////
// SOdysseyDrawingToolBrushSelector
class ODYSSEYPAINTEREDITOR_API SOdysseyDrawingToolBrushSelector
    : public SCompoundWidget
{
    typedef SCompoundWidget         tSuperClass;
    typedef SOdysseyDrawingToolBrushSelector   tSelf;

public:
    // Construction / Destruction
    SLATE_BEGIN_ARGS( SOdysseyDrawingToolBrushSelector )
        {}
        SLATE_ARGUMENT(UOdysseyDrawingTool*, Tool)
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs );

private:
    // Private data members
    UOdysseyDrawingTool*                mTool;
};


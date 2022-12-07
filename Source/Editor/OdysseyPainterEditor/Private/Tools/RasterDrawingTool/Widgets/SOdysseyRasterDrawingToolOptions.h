// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Framework/SlateDelegates.h"

class UOdysseyRasterDrawingTool;

/////////////////////////////////////////////////////
// SOdysseyRasterDrawingToolOptions
class ODYSSEYPAINTEREDITOR_API SOdysseyRasterDrawingToolOptions
    : public SCompoundWidget
{
    typedef SCompoundWidget         tSuperClass;
    typedef SOdysseyRasterDrawingToolOptions   tSelf;

public:
    // Construction / Destruction
    SLATE_BEGIN_ARGS( SOdysseyRasterDrawingToolOptions )
        {}
        SLATE_ARGUMENT(UOdysseyRasterDrawingTool*, Tool)
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs );

private:
    // Private data members
    UOdysseyRasterDrawingTool*                mTool;
};


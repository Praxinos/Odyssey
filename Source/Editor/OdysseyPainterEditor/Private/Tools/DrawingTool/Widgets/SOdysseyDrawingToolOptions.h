// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Framework/SlateDelegates.h"

class UOdysseyDrawingTool;

/////////////////////////////////////////////////////
// SOdysseyDrawingToolOptions
class ODYSSEYPAINTEREDITOR_API SOdysseyDrawingToolOptions
    : public SCompoundWidget
{
    typedef SCompoundWidget         tSuperClass;
    typedef SOdysseyDrawingToolOptions   tSelf;

public:
    // Construction / Destruction
    SLATE_BEGIN_ARGS( SOdysseyDrawingToolOptions )
        {}
        SLATE_ARGUMENT(UOdysseyDrawingTool*, Tool)
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs );


private:
    void OnShapeSelected(EOdysseyShape iSelectedShape);
    EOdysseyShape GetSelectedShape() const;

    void UpdateShapeSlot();

private:
    // Private data members
    UOdysseyDrawingTool*                mTool;

    TSharedPtr<SBorder>                 mShapeSlot;
};


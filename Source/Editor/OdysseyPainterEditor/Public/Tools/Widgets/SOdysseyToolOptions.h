// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Misc/NotifyHook.h"
#include "Framework/SlateDelegates.h"
#include "IStructureDetailsView.h"

class UOdysseyTool;

/////////////////////////////////////////////////////
// SOdysseyToolOptions
class ODYSSEYPAINTEREDITOR_API SOdysseyToolOptions
    : public SCompoundWidget
    , public FNotifyHook
{
    typedef SCompoundWidget         tSuperClass;
    typedef SOdysseyToolOptions   tSelf;

public:
    // Construction / Destruction
    SLATE_BEGIN_ARGS( SOdysseyToolOptions )
        {}
        SLATE_ATTRIBUTE(UOdysseyTool*, Tool)
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs );

public:
    // SWidget overrides
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime);

private:
    // Private data members
    TAttribute<UOdysseyTool*>                mTool;
    UOdysseyTool*                            mCurrentTool;
    
    SVerticalBox::FSlot*                    mToolSlot;
};


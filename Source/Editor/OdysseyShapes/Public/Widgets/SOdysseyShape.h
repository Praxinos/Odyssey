// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Misc/NotifyHook.h"
#include "Framework/SlateDelegates.h"
#include "IStructureDetailsView.h"

class UOdysseyShape;

/////////////////////////////////////////////////////
// SOdysseyShape
class ODYSSEYSHAPES_API SOdysseyShape
    : public SCompoundWidget
    , public FNotifyHook
{
    typedef SCompoundWidget         tSuperClass;
    typedef SOdysseyShape   tSelf;

public:
    // Construction / Destruction
    SLATE_BEGIN_ARGS( SOdysseyShape )
        {}
        SLATE_ATTRIBUTE(UOdysseyShape*, Shape)
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs );

public:
    // SWidget overrides
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime);

private:
    // Private data members
    TAttribute<UOdysseyShape*>                mShape;
    UOdysseyShape*                            mCurrentShape;
    
    TSharedPtr<IDetailsView>                  mDetailsView;
};


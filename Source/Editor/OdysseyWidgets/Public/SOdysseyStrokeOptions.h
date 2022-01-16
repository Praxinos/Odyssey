// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Misc/NotifyHook.h"
#include "Framework/SlateDelegates.h"
#include "IStructureDetailsView.h"
#include "OdysseyStrokeOptions.h"

class FOdysseyPaintEngine;

/////////////////////////////////////////////////////
// SOdysseyStrokeOptions
class ODYSSEYWIDGETS_API SOdysseyStrokeOptions
    : public SCompoundWidget
    , public FNotifyHook
{
    typedef SCompoundWidget         tSuperClass;
    typedef SOdysseyStrokeOptions   tSelf;

public:
    // Construction / Destruction
    SLATE_BEGIN_ARGS( SOdysseyStrokeOptions )
        {}
        SLATE_ATTRIBUTE( FOdysseyPaintEngine*,  PaintEngine )
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs );

public:
    // FNotifyHook Interface
    virtual void NotifyPostChange( const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged) override;

private:
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
    // Private data members
    TAttribute< FOdysseyPaintEngine* >                mPaintEngine;
    FOdysseyPaintEngine*                              mCurrentPaintEngine;

    TSharedPtr< IStructureDetailsView > DetailsView;
    TSharedPtr< FStructOnScope >        StructToDisplay;
};


// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Misc/NotifyHook.h"
#include "Framework/SlateDelegates.h"
#include "IStructureDetailsView.h"
#include "StrokeEngine/OdysseyStrokeOptions.h"

class UOdysseyStrokeEngine;

/////////////////////////////////////////////////////
// SOdysseyStrokeOptions
class ODYSSEYPAINTEREDITOR_API SOdysseyStrokeOptions
    : public SCompoundWidget
    , public FNotifyHook
{
    typedef SCompoundWidget         tSuperClass;
    typedef SOdysseyStrokeOptions   tSelf;

public:
    // Construction / Destruction
    SLATE_BEGIN_ARGS( SOdysseyStrokeOptions )
        {}
        SLATE_ARGUMENT(UOdysseyStrokeEngine*, StrokeEngine )
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs );

private:
    // Private data members
    UOdysseyStrokeEngine*               mStrokeEngine;

    TSharedPtr<IDetailsView>            DetailsView;
    //TSharedPtr< IStructureDetailsView > DetailsView;
    TSharedPtr< FStructOnScope >        StructToDisplay;
};


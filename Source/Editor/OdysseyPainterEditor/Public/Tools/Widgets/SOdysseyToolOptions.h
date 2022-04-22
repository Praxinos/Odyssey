// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
        SLATE_ARGUMENT(UOdysseyTool*, Tool)
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs );

private:
    // Private data members
    UOdysseyTool*                       mTool;

    TSharedPtr<IDetailsView>            DetailsView;
    //TSharedPtr< IStructureDetailsView > DetailsView;
    TSharedPtr< FStructOnScope >        StructToDisplay;
};


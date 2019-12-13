// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Misc/NotifyHook.h"
#include "Framework/SlateDelegates.h"
#include "IStructureDetailsView.h"
#include "OdysseyBrushPreferencesOverrides.h"

class FOdysseyBrushEditor;

/////////////////////////////////////////////////////
// SOdysseyStrokeOptions
class ODYSSEYBRUSHEDITOR_API SOdysseyBrushPreferencesOverrides
    : public SCompoundWidget
{
    typedef SCompoundWidget                     tSuperClass;
    typedef SOdysseyBrushPreferencesOverrides   tSelf;

public:
    // Construction / Destruction
    SLATE_BEGIN_ARGS( SOdysseyBrushPreferencesOverrides )
        {}
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs, const TSharedRef<FOdysseyBrushEditor>& InEditor );

private:
    // Private data members
    TSharedPtr< IStructureDetailsView > DetailsView;
    FOdysseyBrushPreferencesOverrides   StructData;
    TSharedPtr< FStructOnScope >        StructToDisplay;
};


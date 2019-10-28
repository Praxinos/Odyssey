// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

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


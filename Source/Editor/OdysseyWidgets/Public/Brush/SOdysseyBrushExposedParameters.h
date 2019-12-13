// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Misc/NotifyHook.h"

class IDetailsView;
class UOdysseyBrushAssetBase;

/** Delegate used to set a generic object */
DECLARE_DELEGATE( FOnParameterChanged );

//////////////////////////////////////////////////////////////////////////
// SOdysseyBrushExposedParameters
class ODYSSEYWIDGETS_API SOdysseyBrushExposedParameters
    : public SCompoundWidget
    , public FNotifyHook
{
    SLATE_BEGIN_ARGS( SOdysseyBrushExposedParameters )
        {}
        SLATE_EVENT( FOnParameterChanged, OnParameterChanged )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct( const FArguments& InArgs );

public:
    // Public API
    void Refresh( UOdysseyBrushAssetBase* iValue );

public:
    // FNotifyHook Interface
    virtual void NotifyPreChange( UProperty* PropertyAboutToChange ) override;
    virtual void NotifyPostChange( const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged) override;

private:
    // Private data members
    FOnParameterChanged         OnParameterChangedCallback;
    TSharedPtr< IDetailsView >  details_view;
};

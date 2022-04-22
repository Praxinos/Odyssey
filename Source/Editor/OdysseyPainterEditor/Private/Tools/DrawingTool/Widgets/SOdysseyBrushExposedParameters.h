// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Misc/NotifyHook.h"

class IDetailsView;
class UOdysseyBrushAssetBase;
class UOdysseyDrawingTool;

/** Delegate used to set a generic object */
DECLARE_DELEGATE( FOnParameterChanged );

//////////////////////////////////////////////////////////////////////////
// SOdysseyBrushExposedParameters
class ODYSSEYPAINTEREDITOR_API SOdysseyBrushExposedParameters
    : public SCompoundWidget
    , public FNotifyHook
{
    SLATE_BEGIN_ARGS( SOdysseyBrushExposedParameters )
        {}
        SLATE_ATTRIBUTE(UOdysseyBrushAssetBase*, BrushInstance)
        SLATE_ARGUMENT(UOdysseyDrawingTool*, Tool)
        SLATE_EVENT( FOnParameterChanged, OnParameterChanged )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct( const FArguments& InArgs );

public:
    // SWidget overrides
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime);

public:
    // FNotifyHook Interface
    virtual void NotifyPreChange( FProperty* PropertyAboutToChange ) override;
    virtual void NotifyPostChange( const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged) override;

private:
    // Private data members
    TAttribute<UOdysseyBrushAssetBase*>     mBrushInstance;
    UOdysseyBrushAssetBase*                 mCurrentBrushInstance;
    UOdysseyDrawingTool*                    mTool;

    FOnParameterChanged                     OnParameterChangedCallback;
    TSharedPtr< IDetailsView >              details_view;
};

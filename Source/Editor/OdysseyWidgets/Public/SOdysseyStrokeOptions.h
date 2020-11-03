// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Misc/NotifyHook.h"
#include "Framework/SlateDelegates.h"
#include "IStructureDetailsView.h"
#include "OdysseyStrokeOptions.h"

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
        SLATE_EVENT( FOnInt32ValueChanged   ,   OnStrokeStepChanged         )
        SLATE_EVENT( FOnBooleanValueChanged ,   OnStrokeAdaptativeChanged   )
        SLATE_EVENT( FOnBooleanValueChanged ,   OnStrokePaintOnTickChanged  )
        SLATE_EVENT( FOnInt32ValueChanged   ,   OnInterpolationTypeChanged  )
        SLATE_EVENT( FOnInt32ValueChanged   ,   OnSmoothingMethodChanged    )
        SLATE_EVENT( FOnInt32ValueChanged   ,   OnSmoothingStrengthChanged  )
        SLATE_EVENT( FOnBooleanValueChanged ,   OnSmoothingEnabledChanged   )
        SLATE_EVENT( FOnBooleanValueChanged ,   OnSmoothingRealTimeChanged  )
        SLATE_EVENT( FOnBooleanValueChanged ,   OnSmoothingCatchUpChanged   )
        SLATE_EVENT( FOnBooleanValueChanged ,   OnAnyValueChanged           )
    SLATE_END_ARGS()

    void  Construct( const  FArguments&  InArgs );

public:
    // Public Callbacks
    const  FOdysseyStrokeOptions&  GetStrokeOptions()  const;
    void  SetStrokeOptions( const  FOdysseyStrokeOptions& iValue );

    void  SetStrokeStep         ( int32 iValue );
    void  SetStrokeAdaptative   ( bool  iValue );
    void  SetStrokePaintOnTick  ( bool  iValue );
    void  SetInterpolationType  ( int32 iValue );
    void  SetSmoothingMethod    ( int32 iValue );
    void  SetSmoothingStrength  ( int32 iValue );
    void  SetSmoothingEnabled   ( bool  iValue );
    void  SetSmoothingRealTime  ( bool  iValue );
    void  SetSmoothingCatchUp   ( bool  iValue );

public:
    // FNotifyHook Interface
    virtual void NotifyPostChange( const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged) override;

private:
    // Private data members
    TSharedPtr< IStructureDetailsView > DetailsView;
    FOdysseyStrokeOptions               StructData;
    TSharedPtr< FStructOnScope >        StructToDisplay;

    FOnInt32ValueChanged    OnStrokeStepChangedCallback         ;
    FOnBooleanValueChanged  OnStrokeAdaptativeChangedCallback   ;
    FOnBooleanValueChanged  OnStrokePaintOnTickChangedCallback  ;
    FOnInt32ValueChanged    OnInterpolationTypeChangedCallback  ;
    FOnInt32ValueChanged    OnSmoothingMethodChangedCallback    ;
    FOnInt32ValueChanged    OnSmoothingStrengthChangedCallback  ;
    FOnBooleanValueChanged  OnSmoothingEnabledChangedCallback   ;
    FOnBooleanValueChanged  OnSmoothingRealTimeChangedCallback  ;
    FOnBooleanValueChanged  OnSmoothingCatchUpChangedCallback   ;
    FOnBooleanValueChanged  OnAnyValueChangedCallback           ;
};


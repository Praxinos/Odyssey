// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "SOdysseyStrokeOptions.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"


#define LOCTEXT_NAMESPACE "OdysseyStrokeOptions"

/////////////////////////////////////////////////////
// SOdysseyStrokeOptions
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SOdysseyStrokeOptions::Construct( const FArguments& InArgs )
{
    OnStrokeStepChangedCallback         = InArgs._OnStrokeStepChanged           ;
    OnStrokeAdaptativeChangedCallback   = InArgs._OnStrokeAdaptativeChanged     ;
    OnStrokePaintOnTickChangedCallback  = InArgs._OnStrokePaintOnTickChanged    ;
    OnInterpolationTypeChangedCallback  = InArgs._OnInterpolationTypeChanged    ;
    OnSmoothingMethodChangedCallback    = InArgs._OnSmoothingMethodChanged      ;
    OnSmoothingStrengthChangedCallback  = InArgs._OnSmoothingStrengthChanged    ;
    OnSmoothingEnabledChangedCallback   = InArgs._OnSmoothingEnabledChanged     ;
    OnSmoothingRealTimeChangedCallback  = InArgs._OnSmoothingRealTimeChanged    ;
    OnSmoothingCatchUpChangedCallback   = InArgs._OnSmoothingCatchUpChanged     ;
    OnAnyValueChangedCallback           = InArgs._OnAnyValueChanged;

    // Create a details view
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FNotifyHook* NotifyHook = this;
    StructData      = FOdysseyStrokeOptions();
    StructToDisplay = MakeShared< FStructOnScope >( FOdysseyStrokeOptions::StaticStruct(), (uint8*)&StructData );

    // create struct to display
    FStructureDetailsViewArgs StructureViewArgs;
    StructureViewArgs.bShowObjects      = true;
    StructureViewArgs.bShowAssets       = true;
    StructureViewArgs.bShowClasses      = true;
    StructureViewArgs.bShowInterfaces   = true;

    FDetailsViewArgs ViewArgs;
    ViewArgs.bAllowSearch       = false;
    ViewArgs.bHideSelectionTip  = false;
    ViewArgs.bShowActorLabel    = false;
    ViewArgs.NotifyHook         = NotifyHook;

    DetailsView = PropertyEditorModule.CreateStructureDetailView( ViewArgs, StructureViewArgs, StructToDisplay );

    this->ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
            .FillHeight(1.0f)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                    .FillWidth(1.0f)
                    [
                        DetailsView->GetWidget().ToSharedRef()
                    ]
            ]
    ];
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Private Callbacks
const  FOdysseyStrokeOptions&
SOdysseyStrokeOptions::GetStrokeOptions()  const
{
    return  StructData;
}


void
SOdysseyStrokeOptions::SetStrokeOptions( const  FOdysseyStrokeOptions& iValue )
{
    StructData = iValue;
    OnStrokeStepChangedCallback         .ExecuteIfBound( StructData.Step        );
    OnStrokeAdaptativeChangedCallback   .ExecuteIfBound( StructData.SizeAdaptative  );
    OnStrokePaintOnTickChangedCallback  .ExecuteIfBound( StructData.PaintOnTick );
    OnInterpolationTypeChangedCallback  .ExecuteIfBound( static_cast< int32 >( StructData.Type ) );
    OnSmoothingMethodChangedCallback    .ExecuteIfBound( static_cast< int32 >( StructData.Method ) );
    OnSmoothingStrengthChangedCallback  .ExecuteIfBound( StructData.Strength    );
    OnSmoothingEnabledChangedCallback   .ExecuteIfBound( StructData.Enabled     );
    OnSmoothingRealTimeChangedCallback  .ExecuteIfBound( StructData.RealTime    );
    OnSmoothingCatchUpChangedCallback   .ExecuteIfBound( StructData.CatchUp     );

    OnAnyValueChangedCallback.ExecuteIfBound( true );
}


void
SOdysseyStrokeOptions::SetStrokeStep(        int32 iValue )
{
    StructData.Step = iValue;
    OnStrokeStepChangedCallback.ExecuteIfBound( StructData.Step );
    OnAnyValueChangedCallback.ExecuteIfBound( true );
}


void
SOdysseyStrokeOptions::SetStrokeAdaptative(  bool iValue )
{
    StructData.SizeAdaptative = iValue;
    OnStrokeAdaptativeChangedCallback.ExecuteIfBound( StructData.SizeAdaptative );
    OnAnyValueChangedCallback.ExecuteIfBound( true );
}


void
SOdysseyStrokeOptions::SetStrokePaintOnTick( bool iValue )
{
    StructData.PaintOnTick = iValue;
    OnStrokePaintOnTickChangedCallback.ExecuteIfBound( StructData.PaintOnTick );
    OnAnyValueChangedCallback.ExecuteIfBound( true );
}


void
SOdysseyStrokeOptions::SetInterpolationType( int32 iValue )
{
    StructData.Type = static_cast< EOdysseyInterpolationType >( iValue );
    OnInterpolationTypeChangedCallback.ExecuteIfBound( static_cast< int32 >( StructData.Type ) );
    OnAnyValueChangedCallback.ExecuteIfBound( true );
}


void
SOdysseyStrokeOptions::SetSmoothingMethod(   int32 iValue )
{
    StructData.Method = static_cast< EOdysseySmoothingMethod >( iValue );
    OnSmoothingMethodChangedCallback.ExecuteIfBound( static_cast< int32 >( StructData.Method ) );
    OnAnyValueChangedCallback.ExecuteIfBound( true );
}


void
SOdysseyStrokeOptions::SetSmoothingStrength( int32 iValue )
{
    StructData.Strength = iValue;
    OnSmoothingStrengthChangedCallback.ExecuteIfBound( StructData.Strength );
    OnAnyValueChangedCallback.ExecuteIfBound( true );
}


void
SOdysseyStrokeOptions::SetSmoothingEnabled(  bool iValue )
{
    StructData.Enabled = iValue;
    OnSmoothingEnabledChangedCallback.ExecuteIfBound( StructData.Enabled );
    OnAnyValueChangedCallback.ExecuteIfBound( true );
}


void
SOdysseyStrokeOptions::SetSmoothingRealTime( bool iValue )
{
    StructData.RealTime = iValue;
    OnSmoothingRealTimeChangedCallback.ExecuteIfBound( StructData.RealTime );
    OnAnyValueChangedCallback.ExecuteIfBound( true );
}


void
SOdysseyStrokeOptions::SetSmoothingCatchUp(  bool iValue )
{
    StructData.CatchUp = iValue;
    OnSmoothingCatchUpChangedCallback.ExecuteIfBound( StructData.CatchUp );
    OnAnyValueChangedCallback.ExecuteIfBound( true );
}


//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- FNotifyHook Interface
void
SOdysseyStrokeOptions::NotifyPostChange( const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged )
{
    FString PropertyName = PropertyThatChanged->GetName();
    if( PropertyName == FString( TEXT("Step") ) ) {
        OnStrokeStepChangedCallback.ExecuteIfBound( StructData.Step );
    } else if( PropertyName == FString( TEXT("SizeAdaptative") ) ) {
        OnStrokeAdaptativeChangedCallback.ExecuteIfBound( StructData.SizeAdaptative );
    } else if( PropertyName == FString( TEXT("PaintOnTick" ) ) ) {
        OnStrokePaintOnTickChangedCallback.ExecuteIfBound( StructData.PaintOnTick );
    } else if( PropertyName == FString( TEXT("Type" ) ) ) {
        OnInterpolationTypeChangedCallback.ExecuteIfBound( static_cast< int32 >( StructData.Type ) );
    } else if( PropertyName == FString( TEXT("Method" ) ) ) {
        OnSmoothingMethodChangedCallback.ExecuteIfBound( static_cast< int32 >( StructData.Method ) );
    } else if( PropertyName == FString( TEXT("Strength" ) ) ) {
        OnSmoothingStrengthChangedCallback.ExecuteIfBound( StructData.Strength );
    } else if( PropertyName == FString( TEXT("Enabled" ) ) ) {
        OnSmoothingEnabledChangedCallback.ExecuteIfBound( StructData.Enabled );
    } else if( PropertyName == FString( TEXT("RealTime" ) ) ) {
        OnSmoothingRealTimeChangedCallback.ExecuteIfBound( StructData.RealTime );
    } else if( PropertyName == FString( TEXT("CatchUp" ) ) ) {
        OnSmoothingCatchUpChangedCallback.ExecuteIfBound( StructData.CatchUp );
    }

    OnAnyValueChangedCallback.ExecuteIfBound( true );
}


#undef LOCTEXT_NAMESPACE


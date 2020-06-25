// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "SOdysseyPerformanceOptions.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"


#define LOCTEXT_NAMESPACE "OdysseyPerformanceOptions"

void
FOdysseyPerformanceOptions::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	FString PropertyName = PropertyThatChanged->GetName();
	/* if( PropertyName == FString( TEXT("LiveUpdate") ) ) {
		OnLiveUpdateChangedCallback.ExecuteIfBound( PerformanceOptionsStructData.LiveUpdate );
	} */

	//mPerformanceOptions.OnValueChanged(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)

	if (PropertyName == FString(TEXT("DrawBrushPreview"))) {
		mOnDrawBrushPreviewChanged.ExecuteIfBound(DrawBrushPreview);
	}

	mOnAnyValueChanged.ExecuteIfBound(true);
}

/////////////////////////////////////////////////////
// SOdysseyPerformanceOptions
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SOdysseyPerformanceOptions::Construct( const FArguments& InArgs )
{
    // OnLiveUpdateChangedCallback  = InArgs._OnLiveUpdateChanged   ;
    // OnDrawBrushPreviewChangedCallback  = InArgs._OnDrawBrushPreviewChanged   ;
    // OnAnyValueChangedCallback           = InArgs._OnAnyValueChanged     ;

    // Create a details view
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FNotifyHook* NotifyHook = this;

    //PerformanceOptionsStructData        = FOdysseyPerformanceOptions();
    PerformanceOptionsStructData        = InArgs._PerformanceOptions.Get();
    PerformanceOptionsStructToDisplay   = MakeShared< FStructOnScope >( PerformanceOptionsStructData->GetScriptStruct(), (uint8*)PerformanceOptionsStructData );

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

    PerformanceOptionsDetailsView   = PropertyEditorModule.CreateStructureDetailView( ViewArgs, StructureViewArgs, PerformanceOptionsStructToDisplay    );

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
                        PerformanceOptionsDetailsView->GetWidget().ToSharedRef()
                    ]
            ]
    ];
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Private Callbacks
/* const  FOdysseyPerformanceOptions&
SOdysseyPerformanceOptions::GetPerformanceOptions()  const
{
    return  *PerformanceOptionsStructData;
}


void
SOdysseyPerformanceOptions::SetPerformanceOptions( const  FOdysseyPerformanceOptions& iValue )
{
    *PerformanceOptionsStructData = iValue;

    // OnLiveUpdateChangedCallback.ExecuteIfBound( PerformanceOptionsStructData.LiveUpdate );
    OnDrawBrushPreviewChangedCallback.ExecuteIfBound( PerformanceOptionsStructData.DrawBrushPreview );

    OnAnyValueChangedCallback.ExecuteIfBound( true );
} */


/* void
SOdysseyPerformanceOptions::SetPerformanceOptionLiveUpdate        ( bool iValue )
{
    PerformanceOptionsStructData.LiveUpdate = iValue;

    OnLiveUpdateChangedCallback.ExecuteIfBound( PerformanceOptionsStructData.LiveUpdate );
    OnAnyValueChangedCallback.ExecuteIfBound( true );
} */

/* void
SOdysseyPerformanceOptions::SetPerformanceOptionBrushPreview     ( bool iValue )
{
    PerformanceOptionsStructData.DrawBrushPreview = iValue;

    OnDrawBrushPreviewChangedCallback.ExecuteIfBound( PerformanceOptionsStructData.DrawBrushPreview );
    OnAnyValueChangedCallback.ExecuteIfBound( true );
}


void
SOdysseyPerformanceOptions::SetPeformanceCacheInfoSuperSize       ( int32 iValue )
{
    PerformanceOptionsStructData.SuperSize = iValue;
}


void
SOdysseyPerformanceOptions::SetPeformanceCacheInfoSuperCount      ( int32 iValue )
{
    PerformanceOptionsStructData.SuperCount = iValue;
}


void
SOdysseyPerformanceOptions::SetPeformanceCacheInfoStateSize       ( int32 iValue )
{
    PerformanceOptionsStructData.StateSize = iValue;
}


void
SOdysseyPerformanceOptions::SetPeformanceCacheInfoStateCount      ( int32 iValue )
{
    PerformanceOptionsStructData.StateCount = iValue;
}


void
SOdysseyPerformanceOptions::SetPeformanceCacheInfoStrokeSize      ( int32 iValue )
{
    PerformanceOptionsStructData.StrokeSize = iValue;
}


void
SOdysseyPerformanceOptions::SetPeformanceCacheInfoStrokeCount     ( int32 iValue )
{
    PerformanceOptionsStructData.StrokeCount = iValue;
}


void
SOdysseyPerformanceOptions::SetPeformanceCacheInfoSubstrokeSize   ( int32 iValue )
{
    PerformanceOptionsStructData.SubstrokeSize = iValue;
}


void
SOdysseyPerformanceOptions::SetPeformanceCacheInfoSubstrokeCount  ( int32 iValue )
{
    PerformanceOptionsStructData.SubstrokeCount = iValue;
}


void
SOdysseyPerformanceOptions::SetPeformanceCacheInfoStepSize        ( int32 iValue )
{
    PerformanceOptionsStructData.StepSize = iValue;
}


void
SOdysseyPerformanceOptions::SetPeformanceCacheInfoStepCount       ( int32 iValue )
{
    PerformanceOptionsStructData.StepCount = iValue;
}*/


//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- FNotifyHook Interface
void
SOdysseyPerformanceOptions::NotifyPostChange( const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged )
{
    /* FString PropertyName = PropertyThatChanged->GetName();
    if( PropertyName == FString( TEXT("LiveUpdate") ) ) {
        OnLiveUpdateChangedCallback.ExecuteIfBound( PerformanceOptionsStructData.LiveUpdate );
    }

	//mPerformanceOptions.OnValueChanged(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
    
    if( PropertyName == FString( TEXT("DrawBrushPreview") ) ) {
        OnDrawBrushPreviewChangedCallback.ExecuteIfBound( PerformanceOptionsStructData.DrawBrushPreview );
    }

    OnAnyValueChangedCallback.ExecuteIfBound( true ); */

	PerformanceOptionsStructData->NotifyPostChange(PropertyChangedEvent, PropertyThatChanged);
}


#undef LOCTEXT_NAMESPACE


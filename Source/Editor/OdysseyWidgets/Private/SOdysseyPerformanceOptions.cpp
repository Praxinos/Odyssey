// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "SOdysseyPerformanceOptions.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"


#define LOCTEXT_NAMESPACE "OdysseyPerformanceOptions"

void
FOdysseyPerformanceOptions::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	FString PropertyName = PropertyThatChanged->GetName();

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
//---------------------------------------------------------------- FNotifyHook Interface
void
SOdysseyPerformanceOptions::NotifyPostChange( const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged )
{
	PerformanceOptionsStructData->NotifyPostChange(PropertyChangedEvent, PropertyThatChanged);
}


#undef LOCTEXT_NAMESPACE


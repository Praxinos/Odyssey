// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "SOdysseyStrokeOptions.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "OdysseyPaintEngine.h"


#define LOCTEXT_NAMESPACE "OdysseyStrokeOptions"

/////////////////////////////////////////////////////
// SOdysseyStrokeOptions
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SOdysseyStrokeOptions::Construct( const FArguments& InArgs )
{
    mPaintEngine = InArgs._PaintEngine;

    // Create a details view
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FNotifyHook* NotifyHook = this;
    StructToDisplay = MakeShared< FStructOnScope >( FOdysseyStrokeOptions::StaticStruct(), (uint8*)mPaintEngine.Get()->StrokeOptions());

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
//-------------------------------------------------------------------- SWidget overrides
void
SOdysseyStrokeOptions::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    FOdysseyPaintEngine* paintEngine = mPaintEngine.Get();
    if (paintEngine != mCurrentPaintEngine)
    {
        mCurrentPaintEngine = paintEngine;
        StructToDisplay = MakeShared< FStructOnScope >( FOdysseyStrokeOptions::StaticStruct(), (uint8*)mPaintEngine.Get()->StrokeOptions());
        DetailsView->SetStructureData(StructToDisplay);
    }
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- FNotifyHook Interface
void
SOdysseyStrokeOptions::NotifyPostChange( const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged )
{
    mPaintEngine.Get()->UpdateStrokeOptions();
}


#undef LOCTEXT_NAMESPACE


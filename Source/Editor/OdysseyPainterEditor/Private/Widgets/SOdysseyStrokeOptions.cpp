// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Widgets/SOdysseyStrokeOptions.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "StrokeEngine/OdysseyStrokeOptions.h"


#define LOCTEXT_NAMESPACE "OdysseyStrokeOptions"

/////////////////////////////////////////////////////
// SOdysseyStrokeOptions
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SOdysseyStrokeOptions::Construct( const FArguments& InArgs )
{
    mStrokeEngine = InArgs._StrokeEngine;

    // Create a details view
    /*FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FNotifyHook* NotifyHook = this;

    StructToDisplay = MakeShared< FStructOnScope >( FOdysseyStrokeOptions::StaticStruct(), (uint8*)&mStrokeEngine->GetStrokeOptions());

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

    DetailsView = PropertyEditorModule.CreateStructureDetailView( ViewArgs, StructureViewArgs, StructToDisplay ); */


    FDetailsViewArgs ViewArgs;
    ViewArgs.bAllowSearch = false;
    ViewArgs.bHideSelectionTip = true;
    ViewArgs.bShowActorLabel = false;

    FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    DetailsView = PropertyEditorModule.CreateDetailView(ViewArgs);
    DetailsView->SetObject(mStrokeEngine);

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
                        //DetailsView->GetWidget().ToSharedRef()
                        DetailsView.ToSharedRef()
                    ]
            ]
    ];
}

#undef LOCTEXT_NAMESPACE


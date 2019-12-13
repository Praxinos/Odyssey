// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "SOdysseyBrushPreferencesOverrides.h"

#include "OdysseyBrushBlueprint.h"
#include "OdysseyBrushAssetBase.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "OdysseyBrushEditor.h"
#include "IDetailsView.h"

#define LOCTEXT_NAMESPACE "OdysseyBrushPreferencesOverrides"

/////////////////////////////////////////////////////
// SOdysseyBrushPreferencesOverrides
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SOdysseyBrushPreferencesOverrides::Construct( const  FArguments&  InArgs, const TSharedRef<FOdysseyBrushEditor>& InEditor )
{

    UOdysseyBrushAssetBase* brush =  Cast< UOdysseyBrushAssetBase >( InEditor->GetBlueprintObj()->GeneratedClass->GetDefaultObject() );

    // Create a details view
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FNotifyHook* NotifyHook = nullptr;

    StructData = brush->Preferences;
    StructToDisplay   = MakeShared< FStructOnScope >( FOdysseyBrushPreferencesOverrides::StaticStruct(), (uint8*)&StructData );

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


#undef LOCTEXT_NAMESPACE


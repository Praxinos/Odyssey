// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "SOdysseyBrushInternalExposed.h"


#include "OdysseyBrushBlueprint.h"
#include "OdysseyBrushAssetBase.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "OdysseyBrushEditor.h"
#include "IDetailsView.h"

#define LOCTEXT_NAMESPACE "OdysseyBrushInternalExposed"

//////////////////////////////////////////////////////////////////////////
// SOdysseyBrushInternalExposed

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ Widget construction


void
SOdysseyBrushInternalExposed::Construct( const FArguments& InArgs, const TSharedRef<FOdysseyBrushEditor>& InEditor )
{
    EditorPtr = InEditor;
    UObject* DefaultObject = InEditor->GetBlueprintObj()->GeneratedClass->GetDefaultObject();

    TSharedPtr<FOdysseyBrushEditor> EdPtr = EditorPtr.Pin();
    FNotifyHook* NotifyHook = EdPtr.Get();

    // Create a details view
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FDetailsViewArgs DetailsViewArgs(/*bUpdateFromSelection=*/ false, /*bLockable=*/ false, /*bAllowSearch=*/ false, FDetailsViewArgs::HideNameArea, /*bHideSelectionTip=*/ true, NotifyHook);
    DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Hide;
    DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

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
                        DetailsView.ToSharedRef()
                    ]
            ]
    ];

    DetailsView->SetObject( DefaultObject );
}


void
SOdysseyBrushInternalExposed::Refresh()
{
    UObject* DefaultObject = EditorPtr.Pin()->GetBlueprintObj()->GeneratedClass->GetDefaultObject();
    DetailsView->SetObject( DefaultObject );
}

#undef LOCTEXT_NAMESPACE


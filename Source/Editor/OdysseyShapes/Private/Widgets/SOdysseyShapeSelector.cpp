// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Widgets/SOdysseyShapeSelector.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"


#define LOCTEXT_NAMESPACE "SOdysseyShapeSelector"

/////////////////////////////////////////////////////
// SOdysseyShapeSelector
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SOdysseyShapeSelector::Construct( const FArguments& InArgs )
{
    mTool = InArgs._Tool;

    FDetailsViewArgs ViewArgs;
    ViewArgs.bAllowSearch = false;
    ViewArgs.bHideSelectionTip = true;
    ViewArgs.bShowActorLabel = false;

    FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    DetailsView = PropertyEditorModule.CreateDetailView(ViewArgs);
    DetailsView->SetObject(mTool);

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
}

#undef LOCTEXT_NAMESPACE


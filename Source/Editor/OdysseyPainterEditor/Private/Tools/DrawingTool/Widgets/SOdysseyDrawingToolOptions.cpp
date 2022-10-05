// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/DrawingTool/Widgets/SOdysseyDrawingToolOptions.h"

#include "ObjectEditorUtils.h"
#include "PropertyEditorModule.h"
#include "ISinglePropertyView.h"
#include "Widgets/SOdysseyShapeSelector.h"
#include "Widgets/SOdysseyShape.h"
#include "Tools/DrawingTool/Customizations/OdysseyDrawingToolOptionsCustomization.h"

#define LOCTEXT_NAMESPACE "SOdysseyDrawingToolOptions"

/////////////////////////////////////////////////////
// SOdysseyDrawingToolOptions
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SOdysseyDrawingToolOptions::Construct( const FArguments& InArgs )
{
    mTool = InArgs._Tool;

    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

    // Create a details view
    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.bAllowSearch = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
    
    TSharedRef<IDetailsView> detailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    detailsView->RegisterInstancedCustomPropertyLayout(UOdysseyDrawingTool::StaticClass(),
        FOnGetDetailCustomizationInstance::CreateLambda([this]() { return FOdysseyDrawingToolOptionsCustomization::MakeInstance(); }));

    detailsView->SetObject(mTool);

    this->ChildSlot
    [
        detailsView
    ];
}

#undef LOCTEXT_NAMESPACE


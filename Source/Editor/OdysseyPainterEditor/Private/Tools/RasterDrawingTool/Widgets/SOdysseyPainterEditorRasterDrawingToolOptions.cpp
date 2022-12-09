// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterDrawingTool/Widgets/SOdysseyPainterEditorRasterDrawingToolOptions.h"

#include "ObjectEditorUtils.h"
#include "PropertyEditorModule.h"
#include "ISinglePropertyView.h"
#include "Widgets/SOdysseyShapeSelector.h"
#include "Widgets/SOdysseyShape.h"
#include "Tools/RasterDrawingTool/Customizations/OdysseyPainterEditorRasterDrawingToolOptionsCustomization.h"

#define LOCTEXT_NAMESPACE "SOdysseyPainterEditorRasterDrawingToolOptions"

/////////////////////////////////////////////////////
// SOdysseyPainterEditorRasterDrawingToolOptions
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SOdysseyPainterEditorRasterDrawingToolOptions::Construct( const FArguments& InArgs )
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
    detailsView->RegisterInstancedCustomPropertyLayout(UOdysseyPainterEditorRasterDrawingTool::StaticClass(),
        FOnGetDetailCustomizationInstance::CreateLambda([this]() { return FOdysseyPainterEditorRasterDrawingToolOptionsCustomization::MakeInstance(); }));

    detailsView->SetObject(mTool);

    this->ChildSlot
    [
        detailsView
    ];
}

#undef LOCTEXT_NAMESPACE


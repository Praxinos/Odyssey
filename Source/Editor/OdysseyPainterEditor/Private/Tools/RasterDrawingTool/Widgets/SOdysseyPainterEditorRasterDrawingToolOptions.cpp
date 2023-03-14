// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterDrawingTool/Widgets/SOdysseyPainterEditorRasterDrawingToolOptions.h"

#include "ObjectEditorUtils.h"
#include "PropertyEditorModule.h"
#include "ISinglePropertyView.h"
#include "Widgets/SOdysseyShapeSelector.h"
#include "Widgets/SOdysseyShape.h"

#define LOCTEXT_NAMESPACE "SOdysseyPainterEditorRasterDrawingToolOptions"

/////////////////////////////////////////////////////
// SOdysseyPainterEditorRasterDrawingToolOptions
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
SOdysseyPainterEditorRasterDrawingToolOptions::~SOdysseyPainterEditorRasterDrawingToolOptions()
{
    mTool->OnShapeChanged().RemoveAll(this);
}

void
SOdysseyPainterEditorRasterDrawingToolOptions::Construct( const FArguments& InArgs )
{
    mTool = InArgs._Tool;
    mTool->OnShapeChanged().AddRaw(this, &SOdysseyPainterEditorRasterDrawingToolOptions::OnToolShapeChanged);

    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

    // Create a details view
    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Automatic;
    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.bAllowSearch = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
    
    mDetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    mDetailsView->SetObject(mTool);

    this->ChildSlot
    [
        //TODO: The following works, just uncomment it when you cant to implement new shapes

        /* SNew(SVerticalBox)
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SOdysseyShapeSelector)
            .SelectedShape_UObject(mTool, &UOdysseyPainterEditorRasterDrawingTool::GetSelectedShape)
            .OnShapeSelected(this, &SOdysseyPainterEditorRasterDrawingToolOptions::OnShapeSelected)
        ]
        + SVerticalBox::Slot()
        [ */
            mDetailsView.ToSharedRef()
        //]
    ];
}

void
SOdysseyPainterEditorRasterDrawingToolOptions::OnShapeSelected(EOdysseyShape iShape)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mTool, "SelectedShape", iShape);
}

void
SOdysseyPainterEditorRasterDrawingToolOptions::OnToolShapeChanged()
{
    mDetailsView->ForceRefresh();
}

#undef LOCTEXT_NAMESPACE


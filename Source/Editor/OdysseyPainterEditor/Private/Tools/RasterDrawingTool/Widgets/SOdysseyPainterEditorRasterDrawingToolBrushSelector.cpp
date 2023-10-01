// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterDrawingTool/Widgets/SOdysseyPainterEditorRasterDrawingToolBrushSelector.h"

#include "ObjectEditorUtils.h"
#include "PropertyEditorModule.h"
#include "ISinglePropertyView.h"
#include "Widgets/Brush/SOdysseyBrushSelector.h"

#define LOCTEXT_NAMESPACE "SOdysseyPainterEditorRasterDrawingToolBrushSelector"

/////////////////////////////////////////////////////
// SOdysseyPainterEditorRasterDrawingToolBrushSelector
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
SOdysseyPainterEditorRasterDrawingToolBrushSelector::~SOdysseyPainterEditorRasterDrawingToolBrushSelector()
{
    mTool->OnBrushChanged().RemoveAll(this);
}

void
SOdysseyPainterEditorRasterDrawingToolBrushSelector::Construct( const FArguments& InArgs )
{
    mTool = InArgs._Tool;
    mTool->OnBrushChanged().AddRaw(this, &SOdysseyPainterEditorRasterDrawingToolBrushSelector::OnToolBrushChanged);

    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    
    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Automatic;
    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.bAllowSearch = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
    
    mDetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

    mDetailsView->SetObject(mTool->GetBrushInstance());

    this->ChildSlot
    [
        SNew(SVerticalBox)
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SOdysseyBrushSelector)
            .Brush_UObject(mTool, &UOdysseyPainterEditorRasterDrawingTool::GetBrush)
            .OnBrushChanged(this, &SOdysseyPainterEditorRasterDrawingToolBrushSelector::OnBrushSelected)
        ]
        + SVerticalBox::Slot()
        [
            mDetailsView.ToSharedRef()
        ]
    ];
}

void
SOdysseyPainterEditorRasterDrawingToolBrushSelector::OnBrushSelected(UOdysseyBrush* iBrush)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mTool, "Brush", iBrush);
}

void
SOdysseyPainterEditorRasterDrawingToolBrushSelector::OnToolBrushChanged()
{
    mDetailsView->SetObject(mTool->GetBrushInstance());
    mDetailsView->ForceRefresh();
}

#undef LOCTEXT_NAMESPACE


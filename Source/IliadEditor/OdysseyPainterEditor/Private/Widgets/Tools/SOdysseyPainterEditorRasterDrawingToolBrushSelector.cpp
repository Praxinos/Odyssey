// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyPainterEditorRasterDrawingToolBrushSelector.h"
#include "OdysseyBrushAssetBase.h"
#include "OdysseyPainterEditorRasterDrawingTool.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#include "ObjectEditorUtils.h"
#include "PropertyEditorModule.h"
#include "ISinglePropertyView.h"
#include "SOdysseyBrushSelector.h"
#include "OdysseyPainterEditorRasterDrawingTool.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyBrushAssetBase.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Input/SButton.h"

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
    mIsExpanded = true;

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



    FSinglePropertyParams brushPropertyParams;
    brushPropertyParams.NamePlacement = EPropertyNamePlacement::Hidden;

    this->ChildSlot
    [
        SNew(SGridPanel)
        .FillColumn( 1, 1.f )
        + SGridPanel::Slot(0, 0)
        [
            SAssignNew(mExpanderArrow, SButton)
            .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
            .VAlign(VAlign_Center)
            .HAlign(HAlign_Center)
            .ClickMethod( EButtonClickMethod::MouseDown )
            .OnClicked( this, &SOdysseyPainterEditorRasterDrawingToolBrushSelector::OnExpanderArrowClicked )
            .ContentPadding(0.f)
            .ForegroundColor( FSlateColor::UseForeground() )
            .IsFocusable( false )
            [
                SNew(SImage)
                .Image( this, &SOdysseyPainterEditorRasterDrawingToolBrushSelector::GetExpanderArrowImage )
                .ColorAndOpacity( FSlateColor::UseSubduedForeground() )
            ]
        ]
        + SGridPanel::Slot(1, 0)
        [
            PropertyEditorModule.CreateSingleProperty(mTool, "Brush", brushPropertyParams).ToSharedRef()
        ]
        + SGridPanel::Slot(1, 1)
        [
            SNew(SBox)
            .Visibility_Lambda(
                [this]()
                {
                    return mIsExpanded ? EVisibility::Visible : EVisibility::Collapsed;
                }
            )
            [
                mDetailsView.ToSharedRef()
            ]
        ]
    ];
}

const FSlateBrush*
SOdysseyPainterEditorRasterDrawingToolBrushSelector::GetExpanderArrowImage() const
{
    FName resourceName;
    if (mIsExpanded)
    {
        if ( mExpanderArrow->IsHovered() )
        {
            static FName expandedHoveredName = "TreeArrow_Expanded_Hovered";
            resourceName = expandedHoveredName;
        }
        else
        {
            static FName expandedName = "TreeArrow_Expanded";
            resourceName = expandedName;
        }
    }
    else
    {
        if ( mExpanderArrow->IsHovered() )
        {
            static FName collapsedHoveredName = "TreeArrow_Collapsed_Hovered";
            resourceName = collapsedHoveredName;
        }
        else
        {
            static FName collapsedName = "TreeArrow_Collapsed";
            resourceName = collapsedName;
        }
    }

    return FAppStyle::Get().GetBrush(resourceName);
}

FReply
SOdysseyPainterEditorRasterDrawingToolBrushSelector::OnExpanderArrowClicked()
{
    mIsExpanded = !mIsExpanded;
    return FReply::Handled();
}

void
SOdysseyPainterEditorRasterDrawingToolBrushSelector::OnBrushSelected(UOdysseyBrush* iBrush)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mTool, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, Brush), iBrush);
}

void
SOdysseyPainterEditorRasterDrawingToolBrushSelector::OnToolBrushChanged()
{
    mDetailsView->SetObject(mTool->GetBrushInstance());
    mDetailsView->ForceRefresh();
}

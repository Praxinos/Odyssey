// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/SOdysseyShapeSelector.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "OdysseyStyleSet.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Images/SImage.h"


#define LOCTEXT_NAMESPACE "SOdysseyShapeSelector"

/////////////////////////////////////////////////////
// SOdysseyShapeSelector
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SOdysseyShapeSelector::Construct( const FArguments& InArgs )
{
    mSelectedShape = InArgs._SelectedShape;
    mOnShapeSelected = InArgs._OnShapeSelected;

    const FCheckBoxStyle* checkboxStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("OdysseyCheckBoxStyle.TransparentCheckBox");

    this->ChildSlot
    [
        SNew( SWrapBox )
        .UseAllottedWidth( true )
        +SWrapBox::Slot()
        [
            SNew( SCheckBox )
            .Style( checkboxStyle )
            .OnCheckStateChanged( this, &SOdysseyShapeSelector::OnCheckStateChanged, EOdysseyShape::kFreehand )
            .IsChecked_Raw(this, &SOdysseyShapeSelector::IsChecked, EOdysseyShape::kFreehand )
            [
                SNew( SImage )
                .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.FreeHand32" ) )
            ]
        ]
        +SWrapBox::Slot()
        [
            SNew( SCheckBox )
            .Style( checkboxStyle )
            .OnCheckStateChanged( this, &SOdysseyShapeSelector::OnCheckStateChanged, EOdysseyShape::kRectangle )
            .IsChecked_Raw(this, &SOdysseyShapeSelector::IsChecked, EOdysseyShape::kRectangle )
            [
                SNew( SImage )
                .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Square32" ) )
            ]
        ]
    ];
}

void
SOdysseyShapeSelector::OnCheckStateChanged(ECheckBoxState iState, EOdysseyShape iShape)
{
    mOnShapeSelected.ExecuteIfBound(iShape);
}

ECheckBoxState
SOdysseyShapeSelector::IsChecked(EOdysseyShape iShape) const
{
    return mSelectedShape.Get() == iShape ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

#undef LOCTEXT_NAMESPACE


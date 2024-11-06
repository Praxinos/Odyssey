// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorRow.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimeline.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineLightTableKey.h"
#include "Widgets/LayerStack/SOdysseyAnimationLayerStack.h"
#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Math/UnitConversion.h"
#include "SEnumCombo.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineLightTableHeader.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineInbetweeningHeader.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "OdysseyPainterEditor.h"
#include "HUD/OdysseyVectorHUD.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow

void SOdysseyAnimationLayerImageVectorRow::Construct(
    const FArguments& InArgs,
    const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView,
    UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector
)
{
    ensure(iAnimationLayerImageVector);
    mAnimationLayerImageVector = iAnimationLayerImageVector;
	mEditor = InArgs._PainterEditor;

    SOdysseyAnimationLayerRow::Construct(
        SOdysseyAnimationLayerRow::FArguments()
			.TimelinePosition(InArgs._TimelinePosition),
        iOwnerTableView,
		iAnimationLayerImageVector
    );

	SignalSelectionMode = ETableRowSignalSelectionMode::Instantaneous;
}

//PRIVATE API-----------------------------------------------------------

TSharedRef<SWidget>
SOdysseyAnimationLayerImageVectorRow::GenerateWidget( const FName& iRow, const FName& iColumn )
{
	if (iRow == "Inbetweening")
	{
		if (iColumn == "Header")
		{
			return GenerateInbetweeningRowHeaderWidget();
		}
	}

	return SOdysseyAnimationLayerRow::GenerateWidget(iRow, iColumn);
}

TArray<TSharedPtr<SWidget>>
SOdysseyAnimationLayerImageVectorRow::GenerateMainRowHeaderOptionWidgets()
{
	TArray<TSharedPtr<SWidget>> widgets = SOdysseyAnimationLayerRow::GenerateMainRowHeaderOptionWidgets();

    const FCheckBoxStyle* coloredToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("Texture.ColoredToggle");
    const FCheckBoxStyle* wireframeToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("Texture.WireframeToggle");

	//AlphaLock
	widgets.Add(
		//WireframeLock
		SNew(SCheckBox)
		.Style(wireframeToggleStyle)
		.OnCheckStateChanged(this, &SOdysseyAnimationLayerImageVectorRow::OnIsWireframeCheckStateChanged)
		.IsChecked(this, &SOdysseyAnimationLayerImageVectorRow::GetIsWireframeIsChecked)
	);

	widgets.Add(
		//ColoredLock
		SNew(SCheckBox)
		.Style(coloredToggleStyle)
		.OnCheckStateChanged(this, &SOdysseyAnimationLayerImageVectorRow::OnIsColoredCheckStateChanged)
		.IsChecked(this, &SOdysseyAnimationLayerImageVectorRow::GetIsColoredIsChecked)
	);

	return widgets;
}

TSharedRef<SWidget>
SOdysseyAnimationLayerImageVectorRow::GenerateInbetweeningRowHeaderWidget()
{
	return SAssignNew( mInbetweeningHeader, SOdysseyAnimationTimelineInbetweeningHeader, mAnimationLayerImageVector)
		.PainterEditor(mEditor);
}

EVisibility
SOdysseyAnimationLayerImageVectorRow::GetRowVisibility(FName iRow) const
{
	if (iRow == "Inbetweening")
	{
		TSharedPtr<FOdysseyPainterEditor> editor = mEditor.Get();
		if (!editor)
			return EVisibility::Collapsed;

		return ( editor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_INBETWEEN ) ? EVisibility::Visible : EVisibility::Collapsed;
	}

	return SOdysseyAnimationLayerRow::GetRowVisibility(iRow);
}

TSharedPtr<SOdysseyAnimationTimelineInbetweeningHeader>
SOdysseyAnimationLayerImageVectorRow::GetInbetweeningHeader()
{
    return mInbetweeningHeader;
}

void
SOdysseyAnimationLayerImageVectorRow::OnIsWireframeCheckStateChanged( ECheckBoxState iState )
{
    FScopedTransaction ScopedTransaction(LOCTEXT("layer-image-vector.transaction.set-wireframe", "Change Layer Wireframe status"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayerImageVector, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayerImageVector, IsWireframe), iState == ECheckBoxState::Checked);
}

void
SOdysseyAnimationLayerImageVectorRow::OnIsColoredCheckStateChanged( ECheckBoxState iState )
{
    FScopedTransaction ScopedTransaction(LOCTEXT("layer-image-vector.transaction.set-coloring", "Change Layer Coloring"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimationLayerImageVector, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayerImageVector, IsColored), iState == ECheckBoxState::Checked);
}

ECheckBoxState
SOdysseyAnimationLayerImageVectorRow::GetIsWireframeIsChecked() const
{
	return mAnimationLayerImageVector->IsWireframe ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

ECheckBoxState
SOdysseyAnimationLayerImageVectorRow::GetIsColoredIsChecked() const
{
	return mAnimationLayerImageVector->IsColored ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

#undef LOCTEXT_NAMESPACE

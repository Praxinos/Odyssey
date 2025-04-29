// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Animation/Timeline/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorRow.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyStyle.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationLayerImageVector.h"
#include "OdysseyPainterEditorModule.h"
#include "Widgets/Animation/Timeline/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimeline.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineLighttableKey.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationLayerStack.h"
#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Math/UnitConversion.h"
#include "SEnumCombo.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineLighttableHeader.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineInbetweeningHeader.h"
#include "OdysseyPainterEditor.h"
#include "HUD/OdysseyVectorHUD.h"
#include "ScopedTransaction.h"

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
    return SAssignNew( mInbetweeningHeader, SOdysseyAnimationTimelineInbetweeningHeader, mAnimationLayerImageVector);
}

EVisibility
SOdysseyAnimationLayerImageVectorRow::GetRowVisibility(FName iRow) const
{
    if (iRow == "Inbetweening")
    {
        FOdysseyPainterEditorModule& painterEditorModule = FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
        FOdysseyPainterEditor* editor = painterEditorModule.GetOpenedEditorForAsset(mAnimationLayerImageVector->GetAnimation());
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
    mAnimationLayerImageVector->SetIsWireframe(iState == ECheckBoxState::Checked);
}

void
SOdysseyAnimationLayerImageVectorRow::OnIsColoredCheckStateChanged( ECheckBoxState iState )
{
    FScopedTransaction ScopedTransaction(LOCTEXT("layer-image-vector.transaction.set-coloring", "Change Layer Coloring"));
    mAnimationLayerImageVector->SetIsColored(iState == ECheckBoxState::Checked);
}

ECheckBoxState
SOdysseyAnimationLayerImageVectorRow::GetIsWireframeIsChecked() const
{
    return mAnimationLayerImageVector->IsWireframe() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

ECheckBoxState
SOdysseyAnimationLayerImageVectorRow::GetIsColoredIsChecked() const
{
    return mAnimationLayerImageVector->IsColored() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

#undef LOCTEXT_NAMESPACE

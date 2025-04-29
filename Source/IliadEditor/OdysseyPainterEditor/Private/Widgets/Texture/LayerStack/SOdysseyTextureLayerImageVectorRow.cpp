// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Texture/LayerStack/SOdysseyTextureLayerImageVectorRow.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyStyle.h"
#include "OdysseyTextureLayerImageVector.h"
#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Math/UnitConversion.h"
#include "SEnumCombo.h"

#define LOCTEXT_NAMESPACE "TextureEditor"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow

void SOdysseyTextureLayerImageVectorRow::Construct( const FArguments& InArgs
                                                  , const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView
                                                  , UOdysseyTextureLayerImageVector* iTextureLayerImageVector )
{
    ensure(iTextureLayerImageVector);
    mTextureLayerImageVector = iTextureLayerImageVector;

    SOdysseyLayerRow::Construct(
        SOdysseyLayerRow::FArguments(),
        iOwnerTableView,
        iTextureLayerImageVector
    );

    SignalSelectionMode = ETableRowSignalSelectionMode::Instantaneous;
}

//PRIVATE API-----------------------------------------------------------

TArray<TSharedPtr<SWidget>>
SOdysseyTextureLayerImageVectorRow::GenerateMainRowHeaderOptionWidgets()
{
    TArray<TSharedPtr<SWidget>> widgets = SOdysseyLayerRow::GenerateMainRowHeaderOptionWidgets();

    const FCheckBoxStyle* coloredToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("Texture.ColoredToggle");
    const FCheckBoxStyle* wireframeToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("Texture.WireframeToggle");

    //AlphaLock
    widgets.Add(
        //WireframeLock
        SNew(SCheckBox)
        .Style(wireframeToggleStyle)
        .OnCheckStateChanged(this, &SOdysseyTextureLayerImageVectorRow::OnIsWireframeCheckStateChanged)
        .IsChecked(this, &SOdysseyTextureLayerImageVectorRow::GetIsWireframeIsChecked)
    );

    widgets.Add(
        //ColoredLock
        SNew(SCheckBox)
        .Style(coloredToggleStyle)
        .OnCheckStateChanged(this, &SOdysseyTextureLayerImageVectorRow::OnIsColoredCheckStateChanged)
        .IsChecked(this, &SOdysseyTextureLayerImageVectorRow::GetIsColoredIsChecked)
    );

    return widgets;
}

void
SOdysseyTextureLayerImageVectorRow::OnIsWireframeCheckStateChanged( ECheckBoxState iState )
{
    FScopedTransaction ScopedTransaction(LOCTEXT("layer-image-vector.transaction.set-wireframe", "Change Layer Wireframe status"));
    mTextureLayerImageVector->SetIsWireframe(iState == ECheckBoxState::Checked);
}

void
SOdysseyTextureLayerImageVectorRow::OnIsColoredCheckStateChanged( ECheckBoxState iState )
{
    FScopedTransaction ScopedTransaction(LOCTEXT("layer-image-vector.transaction.set-coloring", "Change Layer Coloring"));
    mTextureLayerImageVector->SetIsColored(iState == ECheckBoxState::Checked);
}

ECheckBoxState
SOdysseyTextureLayerImageVectorRow::GetIsWireframeIsChecked() const
{
    return mTextureLayerImageVector->IsWireframe() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

ECheckBoxState
SOdysseyTextureLayerImageVectorRow::GetIsColoredIsChecked() const
{
    return mTextureLayerImageVector->IsColored() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

#undef LOCTEXT_NAMESPACE

// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Texture/LayerStack/SOdysseyTextureLayerImageVectorRow.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyStyle.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
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
    FOdysseyObjectEditorUtils::SetPropertyValue(mTextureLayerImageVector, GET_MEMBER_NAME_CHECKED(UOdysseyTextureLayerImageVector, IsWireframe), iState == ECheckBoxState::Checked);
}

void
SOdysseyTextureLayerImageVectorRow::OnIsColoredCheckStateChanged( ECheckBoxState iState )
{
    FScopedTransaction ScopedTransaction(LOCTEXT("layer-image-vector.transaction.set-coloring", "Change Layer Coloring"));
    FOdysseyObjectEditorUtils::SetPropertyValue(mTextureLayerImageVector, GET_MEMBER_NAME_CHECKED(UOdysseyTextureLayerImageVector, IsColored), iState == ECheckBoxState::Checked);
}

ECheckBoxState
SOdysseyTextureLayerImageVectorRow::GetIsWireframeIsChecked() const
{
    return mTextureLayerImageVector->IsWireframe ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

ECheckBoxState
SOdysseyTextureLayerImageVectorRow::GetIsColoredIsChecked() const
{
    return mTextureLayerImageVector->IsColored ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

#undef LOCTEXT_NAMESPACE

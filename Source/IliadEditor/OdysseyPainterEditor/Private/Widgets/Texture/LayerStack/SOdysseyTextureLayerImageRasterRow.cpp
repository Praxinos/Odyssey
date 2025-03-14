// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Texture/LayerStack/SOdysseyTextureLayerImageRasterRow.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyStyle.h"
#include "LayerStack/OdysseyTextureLayerImageRaster.h"
#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Math/UnitConversion.h"
#include "SEnumCombo.h"

#define LOCTEXT_NAMESPACE "TextureEditor"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow

void SOdysseyTextureLayerImageRasterRow::Construct(const FArguments& InArgs, const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView, UOdysseyTextureLayerImageRaster* iTextureLayerImageRaster)
{
    ensure(iTextureLayerImageRaster);
    mTextureLayerImageRaster = iTextureLayerImageRaster;

    SOdysseyLayerRow::Construct(
        SOdysseyLayerRow::FArguments(),
        iOwnerTableView,
        iTextureLayerImageRaster
    );

    SignalSelectionMode = ETableRowSignalSelectionMode::Instantaneous;
}

//PRIVATE API-----------------------------------------------------------

TArray<TSharedPtr<SWidget>>
SOdysseyTextureLayerImageRasterRow::GenerateMainRowHeaderOptionWidgets()
{
    TArray<TSharedPtr<SWidget>> widgets = SOdysseyLayerRow::GenerateMainRowHeaderOptionWidgets();

    const FCheckBoxStyle* alphaLockedToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("Texture.AlphaLockedToggle");

    //AlphaLock
    widgets.Add(
        SNew(SCheckBox)
        .Style(alphaLockedToggleStyle)
        .OnCheckStateChanged(this, &SOdysseyTextureLayerImageRasterRow::OnIsAlphaLockedCheckStateChanged)
        .IsChecked(this, &SOdysseyTextureLayerImageRasterRow::GetIsAlphaLockedIsChecked)
    );

    return widgets;
}

void
SOdysseyTextureLayerImageRasterRow::OnIsAlphaLockedCheckStateChanged(ECheckBoxState iState)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mTextureLayerImageRaster, GET_MEMBER_NAME_CHECKED(UOdysseyTextureLayerImageRaster, IsAlphaLocked), iState == ECheckBoxState::Checked);
}

ECheckBoxState
SOdysseyTextureLayerImageRasterRow::GetIsAlphaLockedIsChecked() const
{
    return mTextureLayerImageRaster->IsAlphaLocked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

#undef LOCTEXT_NAMESPACE

// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Texture/LayerStack/SOdysseyTextureLayerFolderRow.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "SEnumCombo.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "OdysseyStyle.h"
#include "OdysseyTextureLayerFolder.h"
#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"
#include "Math/UnitConversion.h"

#define LOCTEXT_NAMESPACE "TextureEditor"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow

void SOdysseyTextureLayerFolderRow::Construct(const FArguments& InArgs, const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView, UOdysseyTextureLayerFolder* iTextureLayerFolder)
{
    ensure(iTextureLayerFolder);
    mTextureLayerFolder = iTextureLayerFolder;

    SOdysseyLayerRow::Construct(
        SOdysseyLayerRow::FArguments(),
        iOwnerTableView,
        iTextureLayerFolder
    );

    SignalSelectionMode = ETableRowSignalSelectionMode::Instantaneous;
}

#undef LOCTEXT_NAMESPACE

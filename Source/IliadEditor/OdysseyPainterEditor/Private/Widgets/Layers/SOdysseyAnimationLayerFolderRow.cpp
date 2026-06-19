// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyAnimationLayerFolderRow.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "SEnumCombo.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "OdysseyStyle.h"
#include "LayerFolder/OdysseyAnimationLayerFolder.h"
#include "SOdysseyAnimationLayerStack.h"
#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"
#include "Math/UnitConversion.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow

void SOdysseyAnimationLayerFolderRow::Construct(
    const FArguments& iArgs,
    const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView,
    class UOdysseyAnimationLayerFolder* iAnimationLayerFolder
)
{
    ensure(iAnimationLayerFolder);
    mAnimationLayerFolder = iAnimationLayerFolder;

    SOdysseyAnimationLayerRow::Construct(
        SOdysseyAnimationLayerRow::FArguments()
            .TimelinePosition(iArgs._TimelinePosition),
        iOwnerTableView,
        iAnimationLayerFolder
    );
}

#undef LOCTEXT_NAMESPACE

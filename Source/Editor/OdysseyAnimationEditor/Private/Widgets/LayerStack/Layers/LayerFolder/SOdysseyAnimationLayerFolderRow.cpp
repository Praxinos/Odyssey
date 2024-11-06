// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Layers/LayerFolder/SOdysseyAnimationLayerFolderRow.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "SEnumCombo.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/Layers/LayerFolder/OdysseyAnimationLayerFolder.h"
#include "Widgets/LayerStack/SOdysseyAnimationLayerStack.h"
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
	mEditor = iArgs._PainterEditor;

    SOdysseyAnimationLayerRow::Construct(
        SOdysseyAnimationLayerRow::FArguments()
			.TimelinePosition(iArgs._TimelinePosition),
        iOwnerTableView,
		iAnimationLayerFolder
    );
}

#undef LOCTEXT_NAMESPACE

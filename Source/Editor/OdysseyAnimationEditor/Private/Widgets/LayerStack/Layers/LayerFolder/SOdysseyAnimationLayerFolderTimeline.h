// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/SOdysseyLayerRowBase.h"

class UOdysseyAnimationLayerFolder;
class SOdysseyAnimationLayerStackTreeView;
/**
 * Implements a layer row widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerFolderTimeline
    : public SOdysseyLayerRowBase
{
public:
    // Construction / Destruction
    void Construct(
        const FArguments& iArgs, 
		const TSharedRef<SOdysseyAnimationLayerStackTreeView>& iOwnerTableView,
        UOdysseyAnimationLayerFolder* iLayer
    );

private:
	virtual TSharedRef<SWidget> GenerateWidgetForColumn( const FName& InColumnName ) override;
};
// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SOdysseyLayerRow.h"

/**
 * Implements a layer row widget
 */
class SOdysseyTextureLayerFolderRow
    : public SOdysseyLayerRow
{
public:
    SLATE_BEGIN_ARGS(SOdysseyTextureLayerFolderRow)
        {}
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(const FArguments& iArgs, const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView, class UOdysseyTextureLayerFolder* iTextureLayerFolder);

private:
    class UOdysseyTextureLayerFolder* mTextureLayerFolder;
};

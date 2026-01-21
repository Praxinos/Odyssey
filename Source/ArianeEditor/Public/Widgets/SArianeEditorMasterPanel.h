// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

class FArianeEditor;

class SArianeEditorMasterPanel
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArianeEditorMasterPanel)
        {}
    SLATE_END_ARGS()

    // Construct the widget
    void Construct(const FArguments& InArgs, FArianeEditor* iEditor );

protected:
    // Event Listeners
    void OnActorChanged(const FAssetData& iAssetData);
    FReply OnMeshComponentChanged(const FString iName);
};

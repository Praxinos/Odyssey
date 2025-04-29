// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/SOdysseyLayerStackTreeView.h"

class FOdysseyPainterEditor;

class SOdysseyTextureLayerStackTreeView
    : public SOdysseyLayerStackTreeView
{
public:
    SOdysseyTextureLayerStackTreeView();
    void Construct(const FArguments& InArgs);

public:
    virtual TArray<TSharedPtr<FExtender>> ExtendContextMenu();

private:
    void ExtendContextMenuLayerSection(FMenuBuilder& iMenuBuilder);
    void Action_ConvertLayerToRasterLayer();
};

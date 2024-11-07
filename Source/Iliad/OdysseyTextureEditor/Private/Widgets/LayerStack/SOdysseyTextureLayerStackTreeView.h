// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/SOdysseyLayerStackTreeView.h"

class FOdysseyTextureEditorExtension;

class SOdysseyTextureLayerStackTreeView
    : public SOdysseyLayerStackTreeView
{
public:
    SOdysseyTextureLayerStackTreeView();
    void Construct(const FArguments& InArgs, FOdysseyTextureEditorExtension* iTextureExtension);

public:
    FOdysseyTextureEditorExtension* GetTextureEditorExtension() const;

    virtual TArray<TSharedPtr<FExtender>> ExtendContextMenu();

private:
    void ExtendContextMenuLayerSection(FMenuBuilder& iMenuBuilder);
    void Action_ConvertLayerToRasterLayer();

private:
    FOdysseyTextureEditorExtension* mExtension;
};

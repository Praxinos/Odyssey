// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/SOdysseyLayerStackTreeView.h"

class FOdysseyAnimationTimelineShortcuts;

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerStackTreeView
    : public SOdysseyLayerStackTreeView
{
public:
    SOdysseyAnimationLayerStackTreeView();
    void Construct(const FArguments& InArgs, FOdysseyAnimationEditorExtension* iAnimationExtension);

public:
    FOdysseyAnimationEditorExtension* GetAnimationEditorExtension() const;

private:
    virtual FReply OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent ) override;
    virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;
    virtual void Private_SignalSelectionChanged(ESelectInfo::Type SelectInfo);
    
    virtual TArray<TSharedPtr<FExtender>> ExtendContextMenu();

private:
    void ExtendContextMenuLayerSection(FMenuBuilder& iMenuBuilder);
    void Action_ConvertVectorLayerToRasterLayer();

private:
    TSharedPtr<FOdysseyAnimationTimelineShortcuts> mTimelineShortcuts;
    FOdysseyAnimationEditorExtension* mExtension;
};

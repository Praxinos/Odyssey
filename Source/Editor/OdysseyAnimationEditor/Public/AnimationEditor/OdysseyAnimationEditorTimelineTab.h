// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorTab.h"

class FOdysseyAnimationEditorExtension;
class UOdysseyAnimationLayerStack;
class UOdysseyAnimation;
class UOdysseyAnimationPlayer;

class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationEditorTimelineTab :
    public FOdysseyEditorTab
{
public:
    static const FName& StaticId();
    
public:
    // Construction / Destruction
    virtual ~FOdysseyAnimationEditorTimelineTab();
    FOdysseyAnimationEditorTimelineTab(FOdysseyAnimationEditorExtension* iExtension);

public:
    void SetEmptyTimelineWidget(TSharedRef<SWidget> iWidget);

protected:
    // FOdysseyAnimationEditorTab interface
    virtual const FName& GetId() const override;
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;
    virtual void ExtendMenu(TSharedRef<FExtender> iExtender) override;

protected:
    // Widget Getters
    virtual UOdysseyAnimationLayerStack* LayerStack() const;
    virtual UOdysseyAnimation* Animation() const;
    virtual UOdysseyAnimationPlayer* Player() const;
    virtual float PlaybackFramesPerSecond() const;

protected:
    // Event 
    //DEBUG:
    FReply OnAddFrameClicked();
    //DEBUG:

private:
    //Methods
    virtual void ExtendMenuFile(TSharedRef<FExtender> iExtender);

    void BuildImportMenu(FMenuBuilder& iMenuBuilder);
    void BuildExportMenu(FMenuBuilder& iMenuBuilder);

    void MapActions( TSharedPtr<FUICommandList> iCommandList );

    virtual void ImportTextureSequence();
    void ImportImageSequence();
    void ExportImageSequence();
    void ExportAsFlipbook();
    virtual void CreateNewLayer();
    virtual void ChangeLayerOpacity(float iOpacity);

    void StepForward();
    void StepBackward();

    TSharedPtr<SWidget> CreateDefaultEmptyTimelineTabWidget() const;

private:
    FOdysseyAnimationEditorExtension* mExtension;
    FText mEmptyTimelineMessage;
    TSharedPtr<SWidget> mEmptyTimelineTabWidget;
};


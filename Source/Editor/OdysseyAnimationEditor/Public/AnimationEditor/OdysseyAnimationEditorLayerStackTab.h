// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorTab.h"

class FOdysseyAnimationEditor;
class UOdysseyAnimationLayerStack;
class UAnimation;
class UOdysseyAnimationPlayer;

class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationEditorLayerStackTab :
	public FOdysseyEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyAnimationEditorLayerStackTab();
    FOdysseyAnimationEditorLayerStackTab(FOdysseyAnimationEditor* iEditor);

protected:
    // FOdysseyAnimationEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;

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
    virtual void CreateNewLayer();
    virtual void ChangeLayerOpacity(float iOpacity);

private:
    FOdysseyAnimationEditor* mEditor;
};


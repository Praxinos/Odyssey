// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorTab.h"

class FOdysseyFlipbookEditor;
class SOdysseyFlipbookTimelineView;

class ODYSSEYFLIPBOOKEDITOR_API FOdysseyFlipbookEditorTimelineTab :
	public FOdysseyEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyFlipbookEditorTimelineTab();
    FOdysseyFlipbookEditorTimelineTab(FOdysseyFlipbookEditor* iEditor);

protected:
    // FOdysseyEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;

public:
    // Public Getters
    TSharedPtr<SOdysseyFlipbookTimelineView> Timeline();

public:
    // Widget Getters
    virtual TSharedPtr<FOdysseyFlipbookWrapper> FlipbookWrapper() const;

protected:
    // Event Listeners    
    virtual void OnTimelineCurrentKeyframeChanged(int32 iKeyframe);
    virtual void OnTimelineScrubStarted();
    virtual void OnTimelineScrubStopped();
    virtual void OnFlipbookChanged();
    virtual void OnSpriteCreated(UPaperSprite* iSprite);
    virtual void OnTextureCreated(UTexture2D* iTexture);
    virtual void OnKeyframeRemoved(FPaperFlipbookKeyFrame& iKeyframe);

private:
    // Methods
    void SetTextureAtKeyframeIndex(int32 iKeyframeIndex);
    void BindNavigationShortcuts(FBaseToolkit* iToolkit);

private:
    FOdysseyFlipbookEditor* mEditor;

    TSharedPtr<SOdysseyFlipbookTimelineView> mTimeline;
};


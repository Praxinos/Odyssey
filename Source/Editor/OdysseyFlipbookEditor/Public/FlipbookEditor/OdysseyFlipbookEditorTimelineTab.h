// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditorTab.h"

class FOdysseyFlipbookEditor;
class SOdysseyFlipbookTimelineView;

class ODYSSEYFLIPBOOKEDITOR_API FOdysseyFlipbookEditorTimelineTab :
	public FOdysseyPainterEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyFlipbookEditorTimelineTab();
    FOdysseyFlipbookEditorTimelineTab(FOdysseyFlipbookEditor* iEditor);

protected:
    // FOdysseyPainterEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts() override;
    virtual void OnToolkitInitialized() override;

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
    void BindNavigationShortcuts();

private:
    FOdysseyFlipbookEditor* mEditor;

    TSharedPtr<SOdysseyFlipbookTimelineView> mTimeline;
};


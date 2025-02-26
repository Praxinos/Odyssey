// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "OdysseyEditorTab.h"
#include "PaperFlipbook.h"

class FOdysseyFlipbookEditorExtension;
class SOdysseyFlipbookTimelineView;
class FOdysseyFlipbookWrapper;
class UPaperSprite;

class FOdysseyFlipbookEditorTimelineTab :
    public FOdysseyEditorTab
{
public:
    static const FName& StaticId();

public:
    // Construction / Destruction
    virtual ~FOdysseyFlipbookEditorTimelineTab();
    FOdysseyFlipbookEditorTimelineTab(FOdysseyFlipbookEditorExtension* iExtension);

protected:
    // FOdysseyEditorTab interface
    virtual const FName& GetId() const override;
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
    FOdysseyFlipbookEditorExtension* mExtension;

    TSharedPtr<SOdysseyFlipbookTimelineView> mTimeline;
};

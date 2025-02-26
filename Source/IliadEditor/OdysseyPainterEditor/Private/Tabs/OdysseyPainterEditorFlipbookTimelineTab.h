// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "OdysseyEditorTab.h"
#include "PaperFlipbook.h"
#include "SOdysseyFlipbookTimelineViewEvents.h"

class SOdysseyFlipbookTimelineView;
class UPaperSprite;
class FOdysseyPainterEditor;

class FOdysseyPainterEditorFlipbookTimelineTab :
    public FOdysseyEditorTab
{
public:
    static const FName& StaticId();

public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorFlipbookTimelineTab();
    FOdysseyPainterEditorFlipbookTimelineTab(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyEditorTab interface
    virtual const FName& GetId() const override;
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;
    virtual bool CanOpen() const override;

public:
    // Public Getters
    TSharedPtr<SOdysseyFlipbookTimelineView> Timeline();
    UPaperFlipbook* GetFlipbook() const;

protected:
    // Event Listeners
    virtual void OnTimelineCurrentKeyframeChanged(int32 iKeyframe);
    virtual void OnTimelineScrubStarted();
    virtual void OnTimelineScrubStopped();
    virtual void OnFlipbookChanged();
    virtual void OnSpriteCreated(UPaperSprite* iSprite);
    virtual void OnTextureCreated(UTexture2D* iTexture, FOdysseyTextureConfiguration iTextureConfiguration);
    virtual void OnKeyframeRemoved(FPaperFlipbookKeyFrame& iKeyframe);

private:
    // Methods
    void SetTextureAtKeyframeIndex(int32 iKeyframeIndex);
    void BindNavigationShortcuts(FBaseToolkit* iToolkit);

private:
    FOdysseyPainterEditor* mEditor;

    TSharedPtr<SOdysseyFlipbookTimelineView> mTimeline;
};

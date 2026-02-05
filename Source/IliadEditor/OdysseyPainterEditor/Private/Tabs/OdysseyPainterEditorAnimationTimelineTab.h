// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyEditorTab.h"
#include "OdysseyPainterEditorAnimationExportShortcuts.h"

class FOdysseyPainterEditor;
class UOdysseyAnimationLayerStack;
class UOdysseyAnimation;
class UOdysseyAnimationPlayer;
class UOdysseyLayerCell;
class FOdysseyPainterEditorAnimationTimelinePosition;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorAnimationTimelineTab :
    public FOdysseyEditorTab
{
public:
    static const FName& StaticId();

public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorAnimationTimelineTab();
    FOdysseyPainterEditorAnimationTimelineTab(FOdysseyPainterEditor* iEditor);

public:
    void SetEmptyTimelineWidget(TSharedRef<SWidget> iWidget);

    FInt32Range GetAnimationValidRange() const;
    void SetAnimationValidRange(const TAttribute<FInt32Range>& iValue);

protected:
    virtual const FName& GetId() const override;
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void ExtendMenu(TSharedRef<FExtender> iExtender) override;
    virtual bool CanOpen() const override;

protected:
    // Widget Getters
    virtual UOdysseyAnimation* Animation() const;
    virtual UOdysseyAnimationPlayer* Player() const;
    TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition> GetTimelinePosition() const;

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

    void StepForward();
    void StepBackward();

    TSharedPtr<SWidget> CreateDefaultEmptyTimelineTabWidget() const;

    void OnActivateOutOfPegs(UOdysseyLayerCell* iCell);
    void OnInactivateOutOfPegs();
    ECheckBoxState OnIsOutOfPegsChecked(UOdysseyLayerCell* iCell);
    void OnScrubStart();
    void OnScrubEnd();

private:
    FOdysseyPainterEditor* mEditor;
    FText mEmptyTimelineMessage;
    TSharedPtr<SWidget> mEmptyTimelineTabWidget;
    TAttribute<FInt32Range> mAnimationValidRange;
    FOdysseyPainterEditorAnimationExportShortcuts mAnimationExportShortcuts;
};

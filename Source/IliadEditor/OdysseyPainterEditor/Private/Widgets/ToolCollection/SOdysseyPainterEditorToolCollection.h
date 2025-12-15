// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Layout/SWrapBox.h"

class FOdysseyPainterEditor;
class UOdysseyPainterEditorToolConfiguration;
class UOdysseyToolCollection;

/**
 * Implements the Tool collection widget
 */
class ODYSSEYPAINTEREDITOR_API SOdysseyPainterEditorToolCollection
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyPainterEditorToolCollection)
        {}
        SLATE_ARGUMENT(FOdysseyPainterEditor*, Editor)
        SLATE_ARGUMENT(UOdysseyToolCollection*, ToolCollection)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyPainterEditorToolCollection();
    SOdysseyPainterEditorToolCollection();

    void Construct(const FArguments& InArgs);

protected:

    TSharedRef<SWidget> GenerateToolConfigTile(UOdysseyPainterEditorToolConfiguration* iTool);
    FText GetCollectionDisplayName() const;
    const FSlateBrush* GetToolConfigIcon(UOdysseyPainterEditorToolConfiguration* iTool) const;
    FReply OnAddToolClicked();
    void HandleToolsChanged();
    void RefreshToolsGUI();

protected:
    FOdysseyPainterEditor* mEditor;
    UOdysseyToolCollection* mToolCollection;

    TArray<UOdysseyPainterEditorToolConfiguration*> mDisplayedTools;
    TSharedPtr<SWrapBox> mToolWrapBox;
};

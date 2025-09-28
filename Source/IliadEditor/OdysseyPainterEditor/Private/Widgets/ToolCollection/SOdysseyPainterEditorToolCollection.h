// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Layout/SWrapBox.h"

class FOdysseyPainterEditor;
class UOdysseyPainterEditorTool;
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
        SLATE_ATTRIBUTE(FOdysseyPainterEditor*, Editor)
        SLATE_ATTRIBUTE(UOdysseyToolCollection*, ToolCollection)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyPainterEditorToolCollection();
    SOdysseyPainterEditorToolCollection();

    void Construct(const FArguments& InArgs);

protected:

    TSharedRef<SWidget> GenerateToolTile(UOdysseyPainterEditorTool* Tool);
    FText GetCollectionDisplayName() const;
    const FSlateBrush* GetToolIcon(UOdysseyPainterEditorTool* iTool) const;
    FReply OnAddToolClicked();
    void HandleToolsChanged();
    void RefreshToolsGUI();

protected:
    TAttribute<FOdysseyPainterEditor*> mEditor;
    TAttribute<UOdysseyToolCollection*> mToolCollection;

    TArray<UOdysseyPainterEditorTool*> mDisplayedTools;
    TSharedPtr<SWrapBox> mToolWrapBox;
};

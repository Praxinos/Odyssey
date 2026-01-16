// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Views/STileView.h"

#include "Tools/OdysseyPainterEditorTool.h"
#include "ToolCollection/ToolConfiguration/OdysseyPainterEditorToolConfiguration.h"

enum class EDropIndicatorSide
{
    None,
    Left,
    Right
};

enum class EIconPickerTab : uint8
{
    Textures,
    Icons
};

class ODYSSEYPAINTEREDITOR_API SOdysseyPainterEditorToolTile
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyPainterEditorToolTile)
        {}
        SLATE_ARGUMENT( UOdysseyPainterEditorToolConfiguration*, ToolConfig )
        SLATE_ARGUMENT( UOdysseyToolCollection*, ToolCollection)
        SLATE_ARGUMENT( FOdysseyPainterEditor*, Editor)
        SLATE_ARGUMENT(TAttribute<bool>, IsUnlocked)
        SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyPainterEditorToolTile();
    SOdysseyPainterEditorToolTile();

    void Construct(const FArguments& InArgs);

public:
    FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
    FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
    FReply OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

    virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
    virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
    virtual void OnDragLeave(const FDragDropEvent& DragDropEvent) override;

    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

private:
    FLinearColor GetTileColor() const;
    TSharedRef<SWidget> BuildContextMenu();

    // Locked = can't modify the tool or the collection
    bool IsUnlocked() const;

    // ToolActivable = can load the tool by clicking on it -> Depends on the context (ex: active layer) in the editor
    bool IsToolActivable() const;

    bool CanDeleteTool() const;
    void OnDeleteTool();
    bool CanDuplicateTool() const;
    void OnDuplicateTool();
    bool CanChangeIcon() const;

    TSharedRef<SWidget> BuildTexturePicker();
    TSharedRef<SWidget> BuildIconPicker(TSharedRef<SWindow> PickerWindow);
    void OnChangeIcon();

    void OnTextureSelected(const FAssetData& AssetData);
    void OnStyleIconSelected(const FSlateBrush* iBrush);

private:
    UOdysseyPainterEditorToolConfiguration* mToolConfig = nullptr;
    UOdysseyToolCollection* mCollection = nullptr;
    FOdysseyPainterEditor* mEditor = nullptr;

    mutable EDropIndicatorSide mDropSide = EDropIndicatorSide::None;

    TWeakPtr<SWindow> mPickerWindowPtr;

    bool bIsPressed = false;
    bool bIsDragged = false;
    TAttribute<bool> bIsUnlocked;

    EIconPickerTab mActiveTab = EIconPickerTab::Icons;
    TSharedPtr<SWidgetSwitcher> mWidgetSwitcher;
};

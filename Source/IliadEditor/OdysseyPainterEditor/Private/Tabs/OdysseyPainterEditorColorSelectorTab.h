// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include <ULIS>
#include "OdysseyEditorTab.h"
#include "OdysseyEventState.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorPaletteSet.h"

class FOdysseyPainterEditor;
class UOdysseyPalette;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorColorSelectorTab :
    public FOdysseyEditorTab
{
public:
    static const FName& StaticId();

public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorColorSelectorTab();
    FOdysseyPainterEditorColorSelectorTab(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyEditorTab interface
    virtual const FName& GetId() const override;
    virtual TSharedPtr<SWidget> CreateWidget() override;

protected:
    // Widget Getters
    ::ULIS::FColor GetRawColor() const;

    TArray<TSharedPtr<FOdysseyPainterEditorPaletteSet>> GetPaletteSets() const;
    UOdysseyPaletteEntryColor* GetCurrentPaletteColorEntry() const;
    int GetCurrentPaletteSet() const;

    EOdysseyPainterEditorColorType GetColorType() const;
    void OnColorTypeChanged(EOdysseyPainterEditorColorType iType, ECheckBoxState iState);

    EVisibility GetColorTypeVisibility() const;
    EVisibility GetRawColorWidgetsVisibility() const;
    EVisibility GetColorWheelVisibility() const;
    EVisibility GetColorSlidersVisibility() const;
    EVisibility GetColorHexadecimalVisibility() const;
    EVisibility GetColorPaletteVisibility() const;

    const FSlateBrush* GetExpanderArrowImage(TSharedPtr<SButton> iExpander, bool iIsExpanded) const;
    const FSlateBrush* GetColorWheelExpanderArrowImage() const;
    const FSlateBrush* GetColorSlidersExpanderArrowImage() const;
    const FSlateBrush* GetHexadecimalExpanderArrowImage() const;
    const FSlateBrush* GetColorPaletteExpanderArrowImage() const;

protected:
    // Event Listeners
    void OnColorChanged( eOdysseyEventState::Type iEventState, const ::ULIS::FColor& iColor );

    //void OnPaletteSetChanged(int iIndex, FOdysseyPainterEditorPaletteSet iPaletteSet);
    void OnAddPaletteSet(TSharedPtr<FOdysseyPainterEditorPaletteSet> iPaletteSet);
    void OnRemovePaletteSet(TSharedPtr<FOdysseyPainterEditorPaletteSet> iPaletteSet);
    void OnPaletteCurrentColorEntryChanged(UOdysseyPaletteEntryColor* iEntry, int iSet);

    FReply OnColorWheelExpanderArrowClicked();
    FReply OnColorSlidersExpanderArrowClicked();
    FReply OnHexadecimalExpanderArrowClicked();
    FReply OnColorPaletteExpanderArrowClicked();

private:
    FOdysseyPainterEditor* mEditor;

    TSharedPtr<SButton> mColorWheelExpanderArrow;
    TSharedPtr<SButton> mColorSlidersExpanderArrow;
    TSharedPtr<SButton> mHexadecimalExpanderArrow;
    TSharedPtr<SButton> mColorPaletteExpanderArrow;
    bool mIsColorWheelExpanded = true;
    bool mIsColorSlidersExpanded = false;
    bool mIsHexadecimalExpanded = false;
    bool mIsColorPaletteExpanded = true;
};

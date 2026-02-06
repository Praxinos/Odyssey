// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include <ULIS>
#include "OdysseyEditorTab.h"
#include "OdysseyEventState.h"
#include "OdysseyPainterEditor.h"
#include "Palette/OdysseyPalette.h"

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
    FLinearColor GetLinearColor() const;

    TArray<UOdysseyPaletteSet*> GetPaletteSets() const;
    UOdysseyPaletteEntryColor* GetCurrentPaletteColorEntry() const;
    FGuid GetCurrentPaletteSet() const;

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
    void OnLinearColorChanged( eOdysseyEventState::Type iEventState, const FLinearColor& iColor );

    void OnPaletteSetChanged(FGuid iIndex, UOdysseyPaletteSet* iPaletteSet);
    void OnAddPaletteSet(UOdysseyPalette* iPalette);
    void OnRemovePaletteSet(UOdysseyPaletteSet* iPaletteSet);
    void OnPaletteCurrentColorEntryChanged(UOdysseyPaletteEntryColor* iEntry, FGuid iSet);

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

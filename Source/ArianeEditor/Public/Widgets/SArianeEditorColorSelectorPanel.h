// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
// Odyssey Headers
#include "OdysseyPainterEditorColorType.h"
#include "OdysseyEventState.h"
// ULIS Headers
#include <ULIS>

class FArianeEditor;
class UArianeEditorTool;
class UOdysseyPalette;
class UOdysseyPaletteSet;
class UOdysseyPaletteEntryColor;

/**
 * Implements the Scene Tree View Widget
 */
class ARIANEEDITOR_API SArianeEditorColorSelectorPanel
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArianeEditorColorSelectorPanel)
        {}
        //SLATE_ARGUMENT(UArianeEditorTool*, CurrentTool)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SArianeEditorColorSelectorPanel();
    SArianeEditorColorSelectorPanel();

    void Construct(const FArguments& InArgs, FArianeEditor* InEditor);

protected:
    ::ULIS::FColor GetRawColor() const;
    EOdysseyPainterEditorColorType GetColorType() const;
    void OnColorTypeChanged( EOdysseyPainterEditorColorType iType, ECheckBoxState iState );
    EVisibility GetColorTypeVisibility() const;
    EVisibility GetRawColorWidgetsVisibility() const;
    const TArray<UOdysseyPaletteSet*>* GetPaletteSets() const;
    UOdysseyPaletteEntryColor* GetCurrentPaletteColorEntry() const;
    FGuid GetCurrentPaletteSet() const;
    void OnColorChanged( eOdysseyEventState::Type iEventState, const ::ULIS::FColor& iColor );
    void OnAddPaletteSet( UOdysseyPalette* iPalette );
    void OnRemovePaletteSet( UOdysseyPaletteSet* PaletteSet );
    void OnPaletteSetChanged( FGuid Index, UOdysseyPaletteSet* PaletteSet );
    void OnPaletteCurrentColorEntryChanged( UOdysseyPaletteEntryColor* Entry, FGuid Set );

protected:
    FArianeEditor* Editor;
};

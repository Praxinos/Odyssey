// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "OdysseyEditorTab.h"
#include "SOdysseyPalette.h"

class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorPaletteTab :
    public FOdysseyEditorTab
{
public:
    static const FName& StaticId();

public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorPaletteTab();
    FOdysseyPainterEditorPaletteTab(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyEditorTab interface
    virtual const FName& GetId() const override;
    virtual TSharedPtr<SWidget> CreateWidget() override;

protected:
    // Event Listeners
    /*FOdysseyPaletteSetEntryColor& GetPaletteCurrentColorEntry() const;
    int GetPaletteCurrentSet() const;

    void OnPaletteChanged(UOdysseyPalette* iPalette) const;
    void OnPaletteCurrentColorEntrySelected(UOdysseyPaletteEntryColor* iEntry) const;
    void OnPaletteCurrentSetSelected(int iSet) const; */

private:
    FOdysseyPainterEditor* mEditor;
};

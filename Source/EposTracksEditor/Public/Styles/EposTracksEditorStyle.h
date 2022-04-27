// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

/**
 * Implements the visual style of the epos editor.
 */
class EPOSTRACKSEDITOR_API FEposTracksEditorStyle final
    : public FSlateStyleSet
{
public:
    static void Register();
    static void Unregister();

    static const FEposTracksEditorStyle& Get();

private:
    FEposTracksEditorStyle();

    void InitSequencer();
    void InitTracks();
    void InitCommands();
    void InitToolbar();
    void InitNotes();

private:
    static TSharedPtr<FEposTracksEditorStyle> smSingleton;
};

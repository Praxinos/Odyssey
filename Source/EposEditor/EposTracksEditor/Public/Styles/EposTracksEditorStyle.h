// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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

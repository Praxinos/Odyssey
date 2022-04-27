// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

/**
 * Implements the visual style of the epos editor.
 */
class EPOSSEQUENCEEDITOR_API FEposSequenceEditorStyle final
    : public FSlateStyleSet
{
public:
    static void Register();
    static void Unregister();

    static const FEposSequenceEditorStyle& Get();

private:
    FEposSequenceEditorStyle();

    void InitSequencer();
    void InitCommands();
    void InitAbout();
    void InitViewport();

private:
    static TSharedPtr<FEposSequenceEditorStyle> smSingleton;
};

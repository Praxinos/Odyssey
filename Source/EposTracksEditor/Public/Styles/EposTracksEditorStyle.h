// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

/**
 * Implements the visual style of the epos editor.
 */
class EPOSTRACKSEDITOR_API FEposTracksEditorStyle
    : public FSlateStyleSet
{
public:
    static TSharedRef<FEposTracksEditorStyle> Get();

public:
    FEposTracksEditorStyle();
    virtual ~FEposTracksEditorStyle();

private:
    static TSharedPtr<FEposTracksEditorStyle> smSingleton;
};

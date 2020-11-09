// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

/**
 * Implements the visual style of the epos editor.
 */
class EPOSEDITOR_API FEposEditorStyle
    : public FSlateStyleSet
{
public:
    static TSharedRef<FEposEditorStyle> Get();

public:
    FEposEditorStyle();
    virtual ~FEposEditorStyle();

private:
    static TSharedPtr<FEposEditorStyle> smSingleton;
};

// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

/**
 * Implements the visual style of the epos editor.
 */
class FEposEditorStyle : public FSlateStyleSet
{
public:
    static TSharedRef<FEposEditorStyle> Get();

public:
    FEposEditorStyle();
    virtual ~FEposEditorStyle();

private:
    static TSharedPtr<FEposEditorStyle> smSingleton;
};

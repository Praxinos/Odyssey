// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

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

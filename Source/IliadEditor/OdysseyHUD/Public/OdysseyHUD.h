// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

class FOdysseyHUDElement;
class FCanvas;

class ODYSSEYHUD_API FOdysseyHUD
{
public:
    struct FDrawHUDParams
    {
        DECLARE_DELEGATE_RetVal_OneParam(FVector2D, FTextureToHUD, const FVector2D&)

        FCanvas* mCanvas;
        FTextureToHUD mTextureToHUD;
        int32 mTextureWidth;
        int32 mTextureHeight;
    };

public:
    // Construction / Destruction
    ~FOdysseyHUD();
    FOdysseyHUD();

public:
    void AddElement(TSharedPtr<FOdysseyHUDElement> iElement);
    void RemoveElement(TSharedPtr<FOdysseyHUDElement> iElement);

public:
    void DrawHUD( const FDrawHUDParams& iParams );

private:
    TArray<TSharedPtr<FOdysseyHUDElement>> mElements;
};

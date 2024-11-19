// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"

class FOdysseyHUDElement;
class FCanvas;

class ODYSSEYHUDSYSTEM_API FOdysseyHUDSystem
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
    ~FOdysseyHUDSystem();
    FOdysseyHUDSystem();

public:
    void AddElement(TSharedPtr<FOdysseyHUDElement> iElement);
    void RemoveElement(TSharedPtr<FOdysseyHUDElement> iElement);

public:
    void DrawHUD( const FDrawHUDParams& iParams );

private:
    TArray<TSharedPtr<FOdysseyHUDElement>> mElements;
};

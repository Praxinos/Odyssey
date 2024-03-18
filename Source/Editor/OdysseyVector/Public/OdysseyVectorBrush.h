#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorBrush.generated.h"


USTRUCT(BlueprintType)
struct ODYSSEYVECTOR_API FOdysseyVectorBrush
{
    GENERATED_BODY()

    UPROPERTY( EditAnywhere, Category = "Default" )
    UTexture2D* Texture;

    UPROPERTY( EditAnywhere, Category = "Default" )
    bool ColorFromBrush;

    UPROPERTY( EditAnywhere, Category = "Default" )
    bool BilinearFiltering;

    UPROPERTY( EditAnywhere, Category = "Default" )
    bool ExtendOverPath;

    UPROPERTY( EditAnywhere, Category = "Default" )
    bool Revert;

    FOdysseyVectorBrush()
    {
        width  = 0;
        height = 0;
        bitsPerPixel = 0;
        pixels = nullptr;
        texture = nullptr;
        ColorFromBrush = false;
        ExtendOverPath = true;
        Revert = false;
        BilinearFiltering = false;
    }

    FOdysseyVectorBrush( UTexture2D* iTexture )
    {
        width  = 0;
        height = 0;
        bitsPerPixel = 0;
        pixels = nullptr;
        ColorFromBrush = false;
        ExtendOverPath = true;
        Revert = false;
        BilinearFiltering = false;

        SetTexture( iTexture );
    }

    void SetTexture( UTexture2D* iTexture );
    UTexture2D* GetTexture() const;
    void Lock();
    void Unlock();

    FColor* pixels;
    uint32  width;
    uint32  height;
    uint32  bitsPerPixel;

    private:
        UTexture2D* texture;

};

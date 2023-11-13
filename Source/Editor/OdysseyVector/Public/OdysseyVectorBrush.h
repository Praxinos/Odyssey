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
    bool ColorFromBrush;

    UPROPERTY( EditAnywhere, Category = "Default" )
    bool ExtendOverPath;

    UPROPERTY( EditAnywhere, Category = "Default" )
    bool Revert;

    FOdysseyVectorBrush()
    {
        texture = nullptr;
        ColorFromBrush = false;
        ExtendOverPath = true;
        Revert = false;
    }

    FOdysseyVectorBrush( UTexture2D* iTexture )
    {
        texture = iTexture;
        ColorFromBrush = false;
        ExtendOverPath = true;
        Revert = false;
    }

    void SetTexture( UTexture2D* iTexture );
    UTexture2D* GetTexture() const;

    private:
        UTexture2D* texture;

};

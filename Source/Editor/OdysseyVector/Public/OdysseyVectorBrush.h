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

    UTexture2D* texture;

    FOdysseyVectorBrush()
    {
        texture = nullptr;
    }

    FOdysseyVectorBrush( UTexture2D* iTexture )
    {
        texture = iTexture;
    }
};

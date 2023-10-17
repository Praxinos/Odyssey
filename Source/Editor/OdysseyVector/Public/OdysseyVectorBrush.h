#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorBrush.generated.h"


USTRUCT()
struct ODYSSEYVECTOR_API FOdysseyVectorBrush
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category="Default")
    UTexture2D* texture;

    FOdysseyVectorBrush()
    {
        texture = nullptr;
    }
};

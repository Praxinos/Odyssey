#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPoint.h"

#include "OdysseyVectorHandle.generated.h"

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorHandle : public FOdysseyVectorPoint
{
    public:
        GENERATED_BODY()

    public:
        void Init( double iX, double iY );

    protected:

    public:
        ~UOdysseyVectorHandle();
        UOdysseyVectorHandle();
};

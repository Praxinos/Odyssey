#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"

#include "OdysseyVectorViewObject.generated.h"

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorViewObject : public UObject
{
    GENERATED_BODY()

    public:
        ~UOdysseyVectorViewObject();
        UOdysseyVectorViewObject();

        virtual void Update( FOdysseyVectorObject* iVectorObject );

    private:
        FObjectParam mObjectParam;
};

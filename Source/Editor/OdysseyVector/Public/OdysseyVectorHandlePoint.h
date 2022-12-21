#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorHandle.h"

#include "OdysseyVectorHandlePoint.generated.h"

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorHandlePoint : public UOdysseyVectorHandle
{
    public:
        GENERATED_BODY()

    public:
        void Init( UOdysseyVectorPoint* iParentPoint );

    private:
        UOdysseyVectorPoint* mParentPoint;

    protected:

    public:
        ~UOdysseyVectorHandlePoint();
        UOdysseyVectorHandlePoint();
        uint32 GetType();
        UOdysseyVectorPoint* GetParent();
};

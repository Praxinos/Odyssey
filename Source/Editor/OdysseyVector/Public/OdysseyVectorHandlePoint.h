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
        static UOdysseyVectorHandlePoint* New( FOdysseyVectorPoint* iParentPoint );
        void Init( FOdysseyVectorPoint* iParentPoint );

    private:
        FOdysseyVectorPoint* mParentPoint;

    protected:

    public:
        ~UOdysseyVectorHandlePoint();
        UOdysseyVectorHandlePoint();
        FOdysseyVectorPoint* GetParent();
};

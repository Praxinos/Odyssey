#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorHandle.h"

#include "OdysseyVectorHandleBucket.generated.h"

class FOdysseyVectorBucket;

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorHandleBucket : public UOdysseyVectorHandle
{
    public:
        GENERATED_BODY()

    public:
        static UOdysseyVectorHandleBucket* New( FOdysseyVectorBucket* iParentBucket );
        void Init( FOdysseyVectorBucket* iParentBucket );

    private:
        FOdysseyVectorBucket* mParentBucket;

    protected:

    public:
        ~UOdysseyVectorHandleBucket();
        UOdysseyVectorHandleBucket();
        FOdysseyVectorBucket* GetParent();
};

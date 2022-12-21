#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorHandle.h"

#include "OdysseyVectorHandleSegment.generated.h"

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorHandleSegment : public UOdysseyVectorHandle
{
    public:
        GENERATED_BODY()

    public:
        void Init( UOdysseyVectorSegment* iParentSegment, double iX, double iY );

    private:
        UOdysseyVectorSegment* mParentSegment;

    protected:

    public:
        ~UOdysseyVectorHandleSegment();
        UOdysseyVectorHandleSegment( );

        UOdysseyVectorSegment* GetParent();
        void SetX( double iX );
        void SetY( double iY );
        void Set( double iX,double iY );
};

#pragma once

#include "CoreMinimal.h"
#include <ULIS>

class FInbetweenerPoint;

class ODYSSEYVECTOR_API FInbetweenerQuad
{
    public:
        virtual ~FInbetweenerQuad(){};
        FInbetweenerQuad( );

        FInbetweenerPoint** GetPoints();
        void Link();
        void Unlink();
        bool IsLinked();

        friend class FOdysseyVectorTagInbetweener;

    public:
        static const uint32 LINKED = 1L << 0;

    protected:
        uint32 mFlags;
        FInbetweenerPoint* mPoint[4];
};

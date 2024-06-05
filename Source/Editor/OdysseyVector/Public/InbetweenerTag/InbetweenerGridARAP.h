#pragma once

#include "CoreMinimal.h"

#include <ULIS>

#include "InbetweenerGrid.h"


class FInbetweenerGridARAP : public FInbetweenerGrid
{
    public:
        virtual ~FInbetweenerGridARAP(){};
        FInbetweenerGridARAP( FOdysseyVectorTagInbetweener* iInbetweenerTag
                            , uint32 iNumQuadX
                            , uint32 iNumQuadY );

        virtual void Make(  uint32 iNumQuadX
                          , uint32 iNumQuadY ) override;

        friend class FOdysseyVectorTagInbetweener;
};

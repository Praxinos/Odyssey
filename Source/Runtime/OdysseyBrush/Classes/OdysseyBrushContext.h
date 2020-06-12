// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
#pragma once
#include "CoreMinimal.h"

class  UOdysseyBrushAssetBase;

//TODO: The brush context mechanic is deprecated, it will be removed in 4.26.

/////////////////////////////////////////////////////
// FOdysseyBrushContext
class ODYSSEYBRUSH_API FOdysseyBrushContext
{
private:
    // Private Construction
    FOdysseyBrushContext() : brush( NULL ) {}

public:
    // Singleton Getter
    static  FOdysseyBrushContext*  Instance();

public:
    // Public API
    void                        SetContext( UOdysseyBrushAssetBase* iValue );
    void                        ClearContext();
    bool                        IsValidContext()  const;
    UOdysseyBrushAssetBase*     GetContext()  const;

private:
    // Private Data Members
    UOdysseyBrushAssetBase*  brush;
};

#define  ODYSSEY_BRUSH_CONTEXT_CHECK                                                \
    UOdysseyBrushAssetBase* brush = FOdysseyBrushContext::Instance()->GetContext(); \
    check( brush )

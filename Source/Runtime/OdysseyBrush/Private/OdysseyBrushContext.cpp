// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyBrushContext.h"

/////////////////////////////////////////////////////
// FOdysseyBrushContext


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Singleton Getter
//static
FOdysseyBrushContext*
FOdysseyBrushContext::Instance()
{
    static  FOdysseyBrushContext*  context;

    if( !context )
        context = new  FOdysseyBrushContext();

    return  context;
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API
void
FOdysseyBrushContext::SetContext( UOdysseyBrushAssetBase* iValue )
{
    brush = iValue;
}


void
FOdysseyBrushContext::ClearContext()
{
    brush = NULL;
}


bool
FOdysseyBrushContext::IsValidContext()  const
{
    return  brush != NULL;
}


UOdysseyBrushAssetBase*
FOdysseyBrushContext::GetContext()  const
{
    return  brush;
}


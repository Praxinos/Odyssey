// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

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


// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "Proxies/OdysseyBrushBlock.h"

#include "OdysseySurface.h"
#include "OdysseyBrushContext.h"
#include "OdysseyBrushAssetBase.h"
#include "OdysseyBlock.h"
#include <ULIS_CORE>

/////////////////////////////////////////////////////
// UOdysseyBlockProxyFunctionLibrary


//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::Conv_TextureToOdysseyBlockProxy( UTexture2D* Texture )
{
    ODYSSEY_BRUSH_CONTEXT_CHECK
    if( !Texture ) return  FOdysseyBlockProxy::MakeNullProxy();

    FOdysseyBlockProxy prox;
    FString op = "Conv_" + Texture->GetName();
    ECacheLevel level = ECacheLevel::kSuper;

    ODYSSEY_BRUSH_CACHE_OPERATION_START( level, op )

        FOdysseyBlock* block = NewOdysseyBlockFromUTextureData( Texture );
        prox = FOdysseyBlockProxy( block, op );
        brush->StoreInPool( level, op, prox );

    ODYSSEY_BRUSH_CACHE_OPERATION_END

    return  prox;
}


//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::FillPreserveAlpha( FOdysseyBlockProxy Sample, FOdysseyBrushColor Color, ECacheLevel Cache )
{
    ODYSSEY_BRUSH_CONTEXT_CHECK
    ODYSSEY_BRUSH_BLOCK_PROXY_CHECK_RETURN_VALUE( FOdysseyBlockProxy::MakeNullProxy() )

    FOdysseyBlockProxy prox;
    FString op = "FillPreserveAlpha_" + FString::FromInt( Color.m.RGBHexValue() ) + "_" + Sample.id;
    ODYSSEY_BRUSH_CACHE_OPERATION_START( Cache, op )
        FOdysseyBlock* src = Sample.m;
        FOdysseyBlock* dst = new  FOdysseyBlock( src->Width(), src->Height(), src->GetUE4TextureSourceFormat() );
        ::ULIS::FMakeContext::CopyBlockInto( src->GetIBlock(), dst->GetIBlock() );

        ::ULIS::ParallelFor( dst->Height()
                           , [&]( int iLine ) {
                                for( int i = 0; i < dst->Width(); ++i )
                                {
                                    int alpha = dst->GetIBlock()->PixelColor( i, iLine ).Alpha();
                                    ::ULIS::CColor col = Color.m;
                                    col.SetAlpha( alpha );
                                    dst->GetIBlock()->SetPixelColor( i, iLine, col );
                                }
                            } );

        prox = FOdysseyBlockProxy( dst, op );
        brush->StoreInPool( Cache, op, prox );
    ODYSSEY_BRUSH_CACHE_OPERATION_END

    return  prox;
}


//static
int
UOdysseyBlockProxyFunctionLibrary::GetWidth( FOdysseyBlockProxy Sample )
{
    ODYSSEY_BRUSH_BLOCK_PROXY_CHECK_RETURN_VALUE( -1 )
    FOdysseyBlock* src = Sample.m;
    return  src->Width();
}


//static
int
UOdysseyBlockProxyFunctionLibrary::GetHeight( FOdysseyBlockProxy Sample )
{
    ODYSSEY_BRUSH_BLOCK_PROXY_CHECK_RETURN_VALUE( -1 )
    FOdysseyBlock* src = Sample.m;
    return  src->Height();
}


// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

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
UOdysseyBlockProxyFunctionLibrary::FillPreserveAlpha( FOdysseyBlockProxy Source
                                                    , FOdysseyBrushColor Color
                                                    , ECacheLevel Cache )
{
    ODYSSEY_BRUSH_CONTEXT_CHECK
    if( !Source.m ) return  FOdysseyBlockProxy::MakeNullProxy();

    FOdysseyBlockProxy prox;
    FString op = "FillPreserveAlpha_" + FString::FromInt( Color.m.RGBHexValue() ) + "_" + Source.id;
    ODYSSEY_BRUSH_CACHE_OPERATION_START( Cache, op )
        FOdysseyBlock* src = Source.m;
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
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::CreateBlock( int Width
                                              , int Height
                                              , const FString& ID
                                              , bool InitializeData
                                              , ECacheLevel Cache )
{
    ODYSSEY_BRUSH_CONTEXT_CHECK
    if( Width < 1 || Height < 1 )
        return  FOdysseyBlockProxy::MakeNullProxy();

    FOdysseyBlockProxy prox;
    FString op = "Create_" + ID + "_" + FString::FromInt( Width ) + "_" + FString::FromInt( Height );
    ODYSSEY_BRUSH_CACHE_OPERATION_START( Cache, op )
        FOdysseyBlock* tmp = new  FOdysseyBlock( Width, Height, brush->GetState().target_temp_buffer->GetUE4TextureSourceFormat(), nullptr, nullptr, InitializeData );
        prox = FOdysseyBlockProxy( tmp, op );
        brush->StoreInPool( Cache, op, prox );
    ODYSSEY_BRUSH_CACHE_OPERATION_END

    return  prox;
}


//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::Blend( FOdysseyBlockProxy Top
                                        , FOdysseyBlockProxy Back
                                        , int X
                                        , int Y
                                        , float Opacity
                                        , EOdysseyBlendingMode BlendingMode
                                        , EOdysseyAlphaMode AlphaMode
                                        , ECacheLevel Cache )
{
    ODYSSEY_BRUSH_CONTEXT_CHECK
    if( !Top.m )  return  FOdysseyBlockProxy::MakeNullProxy();;
    if( !Back.m )  return  FOdysseyBlockProxy::MakeNullProxy();;

    FOdysseyBlockProxy prox;
    FString op = "Blend_" + Top.id + "_" + Back.id + "_" + FString::SanitizeFloat( Opacity ) + "_" + FString::FromInt( (int32)BlendingMode ) + "_" + FString::FromInt( (int32)AlphaMode );
    ODYSSEY_BRUSH_CACHE_OPERATION_START( Cache, op )
        ::ULIS::IBlock* source  = Top.m->GetIBlock();
        ::ULIS::IBlock* back    = Back.m->GetIBlock();
        FOdysseyBlock* dst = new FOdysseyBlock( back->Width(), back->Height(), Back.m->GetUE4TextureSourceFormat(), nullptr, nullptr, false );
        ::ULIS::FMakeContext::CopyBlockInto( back, dst->GetIBlock() );
        ::ULIS::FBlendingContext::Blend( source, dst->GetIBlock(), X, Y, (::ULIS::eBlendingMode)BlendingMode, (::ULIS::eAlphaMode)AlphaMode, Opacity );
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


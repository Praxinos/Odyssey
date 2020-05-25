// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "Proxies/OdysseyBrushBlock.h"

#include "OdysseySurface.h"
#include "OdysseyBrushContext.h"
#include "OdysseyBrushAssetBase.h"
#include "OdysseyBlock.h"
#include <ULIS3>
#include "ULISLoaderModule.h"

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

    ::ul3::FPixelValue color = ::ul3::Conv( Color.GetValue(), Source.m->GetULISFormat() );
    FString colorID = FString::FromBlob( ( const uint8* )color.Ptr(), color.Depth() );
    FString op = "FillPreserveAlpha_" + colorID + "_" + Source.id;
    ODYSSEY_BRUSH_CACHE_OPERATION_START( Cache, op )
        FOdysseyBlock* src = Source.m;
        FOdysseyBlock* dst = new  FOdysseyBlock( src->Width(), src->Height(), src->GetUE4TextureSourceFormat() );
        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        ::ul3::uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_TSPEC | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
        ::ul3::Copy( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, src->GetBlock(), dst->GetBlock(), src->GetBlock()->Rect(), ::ul3::FVec2I( 0, 0 ) );
        ::ul3::FillPreserveAlpha( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, dst->GetBlock(), color, dst->GetBlock()->Rect() );
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
        ::ul3::FBlock* source  = Top.m->GetBlock();
        ::ul3::FBlock* back    = Back.m->GetBlock();
        FOdysseyBlock* dst = new FOdysseyBlock( back->Width(), back->Height(), Back.m->GetUE4TextureSourceFormat(), nullptr, nullptr, false );

        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        ::ul3::uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_TSPEC | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
        ::ul3::Copy( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, back, dst->GetBlock(), back->Rect(), ::ul3::FVec2I( 0, 0 ) );
        ::ul3::Blend( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, source, dst->GetBlock(), source->Rect(), ::ul3::FVec2F( X, Y ), ULIS3_AA, (::ul3::eBlendingMode)BlendingMode, (::ul3::eAlphaMode)AlphaMode, Opacity );

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


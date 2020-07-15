// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "Proxies/OdysseyBrushBlock.h"

#include "OdysseySurface.h"
#include "OdysseyBrushAssetBase.h"
#include "OdysseyBlock.h"
#include <ULIS3>
#include "ULISLoaderModule.h"

/////////////////////////////////////////////////////
// UOdysseyBlockProxyFunctionLibrary


//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::Conv_TextureToOdysseyBlockProxy( UTexture2D* Texture, UOdysseyBrushAssetBase* BrushContext )
{
    if( !BrushContext ) return  FOdysseyBlockProxy::MakeNullProxy();
    if( !Texture )      return  FOdysseyBlockProxy::MakeNullProxy();

    FString op = "Conv_" + Texture->GetName();
    ECacheLevel level = ECacheLevel::kSuper;

    if( BrushContext->KeyExistsInPool( level, op ) ) {
        return  BrushContext->RetrieveInPool( level, op );
    }
    else
    {
        FOdysseyBlock* block = NewOdysseyBlockFromUTextureData( Texture );
        FOdysseyBlockProxy prox( block, op );
        BrushContext->StoreInPool( level, op, prox );
        return  prox;
    }
}


//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::FillPreserveAlpha( UOdysseyBrushAssetBase* BrushContext
                                                    , FOdysseyBlockProxy Source
                                                    , FOdysseyBrushColor Color
                                                    , ECacheLevel Cache )
{
    if( !BrushContext ) return  FOdysseyBlockProxy::MakeNullProxy();
    if( !Source.m )     return  FOdysseyBlockProxy::MakeNullProxy();

    ::ul3::FPixelValue color = ::ul3::Conv( Color.GetValue(), Source.m->GetULISFormat() );
    FString colorID = FString::FromBlob( ( const uint8* )color.Ptr(), color.Depth() );
    FString op = "FillPreserveAlpha_" + colorID + "_" + Source.id;

    if( BrushContext->KeyExistsInPool( Cache, op ) ) {
        return  BrushContext->RetrieveInPool( Cache, op );
    }
    else
    {
        FOdysseyBlock* src = Source.m;
        FOdysseyBlock* dst = new  FOdysseyBlock( src->Width(), src->Height(), src->GetUE4TextureSourceFormat() );
        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        ::ul3::uint32 MT_bit = src->Height() > 256 ? ULIS3_PERF_MT : 0;
        ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
        ::ul3::Copy( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, src->GetBlock(), dst->GetBlock(), src->GetBlock()->Rect(), ::ul3::FVec2I( 0, 0 ) );
        ::ul3::FillPreserveAlpha( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, dst->GetBlock(), color, dst->GetBlock()->Rect() );
        FOdysseyBlockProxy prox( dst, op );
        BrushContext->StoreInPool( Cache, op, prox );
        return  prox;
    }

}


//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::CreateBlock( UOdysseyBrushAssetBase* BrushContext
                                              , int Width
                                              , int Height
                                              , const FString& ID
                                              , bool InitializeData
                                              , ECacheLevel Cache )
{
    if( !BrushContext )             return  FOdysseyBlockProxy::MakeNullProxy();
    if( Width < 1 || Height < 1 )   return  FOdysseyBlockProxy::MakeNullProxy();

    FString op = "Create_" + ID + "_" + FString::FromInt( Width ) + "_" + FString::FromInt( Height );

    if( BrushContext->KeyExistsInPool( Cache, op ) ) {
        return  BrushContext->RetrieveInPool( Cache, op );
    }
    else
    {
        FOdysseyBlock* tmp = new  FOdysseyBlock( Width, Height, BrushContext->GetState().target_temp_buffer->GetUE4TextureSourceFormat(), nullptr, nullptr, InitializeData );
        FOdysseyBlockProxy prox( tmp, op );
        BrushContext->StoreInPool( Cache, op, prox );
        return  prox;
    }

}


//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::Blend( UOdysseyBrushAssetBase* BrushContext
                                        , FOdysseyBlockProxy Top
                                        , FOdysseyBlockProxy Back
                                        , int X
                                        , int Y
                                        , float Opacity
                                        , EOdysseyBlendingMode BlendingMode
                                        , EOdysseyAlphaMode AlphaMode
                                        , ECacheLevel Cache )
{
    if( !BrushContext ) return  FOdysseyBlockProxy::MakeNullProxy();
    if( !Top.m )        return  FOdysseyBlockProxy::MakeNullProxy();;
    if( !Back.m )       return  FOdysseyBlockProxy::MakeNullProxy();;

    FString op = "Blend_" + Top.id + "_" + Back.id + "_" + FString::SanitizeFloat( Opacity ) + "_" + FString::FromInt( (int32)BlendingMode ) + "_" + FString::FromInt( (int32)AlphaMode );

    if( BrushContext->KeyExistsInPool( Cache, op ) ) {
        return  BrushContext->RetrieveInPool( Cache, op );
    }
    else
    {

        ::ul3::FBlock* source  = Top.m->GetBlock();
        ::ul3::FBlock* back    = Back.m->GetBlock();
        FOdysseyBlock* dst = new FOdysseyBlock( back->Width(), back->Height(), Back.m->GetUE4TextureSourceFormat(), nullptr, nullptr, false );

        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        ::ul3::uint32 MT_bit = Top.m->Height() > 256 ? ULIS3_PERF_MT : 0;
        ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42;
        ::ul3::Copy( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, back, dst->GetBlock(), back->Rect(), ::ul3::FVec2I( 0, 0 ) );
        ::ul3::Blend( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, source, dst->GetBlock(), source->Rect(), ::ul3::FVec2F( X, Y ), ULIS3_AA, static_cast< ::ul3::eBlendingMode >( BlendingMode ), static_cast< ::ul3::eAlphaMode >( AlphaMode ), Opacity );

        FOdysseyBlockProxy prox( dst, op );
        BrushContext->StoreInPool( Cache, op, prox );
        return  prox;
    }

}


//static
int
UOdysseyBlockProxyFunctionLibrary::GetWidth( FOdysseyBlockProxy Sample )
{
    if( !Sample.m ) return  0;
    return  Sample.m->Width();
}


//static
int
UOdysseyBlockProxyFunctionLibrary::GetHeight( FOdysseyBlockProxy Sample )
{
    if( !Sample.m ) return  0;
    return  Sample.m->Height();
}


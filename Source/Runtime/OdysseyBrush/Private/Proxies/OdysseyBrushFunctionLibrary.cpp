// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "Proxies/OdysseyBrushFunctionLibrary.h"
#include "OdysseyBrushAssetBase.h"
#include "Engine/Texture2D.h"
#include "OdysseySurface.h"
#include "OdysseyBlock.h"
#include <ULIS3>
#include "ULISLoaderModule.h"

#define LOCTEXT_NAMESPACE "OdysseyBrushFunctionLibrary"

//////////////////////////////////////////////////////////////////////////
// UOdysseyBrushFunctionLibrary
//--------------------------------------------------------------------------------------
//--------------------------------------------- Odyssey Brush Blueprint Callable Methods
//static
void
UOdysseyBrushFunctionLibrary::DebugStamp( UOdysseyBrushAssetBase* BrushContext ) {
    if( !BrushContext )
        return;

    int size = ::ul3::FMaths::Max( BrushContext->GetSizeModifier() * BrushContext->GetPressure(), 1.f );

    ::ul3::FBlock debug_stamp( size, size, BrushContext->GetState().target_temp_buffer->GetULISFormat() );
    ::ul3::FPixelValue color = ::ul3::Conv( BrushContext->GetState().color, ULIS3_FORMAT_RGBAF );
    color.SetAlphaF( BrushContext->GetFlowModifier() );

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = size > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42;
    ::ul3::Fill( hULIS.ThreadPool()
               , ULIS3_BLOCKING
               , perfIntent
               , hULIS.HostDeviceInfo()
               , ULIS3_NOCB
               , &debug_stamp
               , color
               , debug_stamp.Rect() );

    ::ul3::FRect invalidRect;
    invalidRect.x = BrushContext->GetX() - size / 2;
    invalidRect.y = BrushContext->GetY() - size / 2;
    invalidRect.w = size + 1;
    invalidRect.h = size + 1;

    ::ul3::Blend( hULIS.ThreadPool()
                , ULIS3_BLOCKING
                , perfIntent
                , hULIS.HostDeviceInfo()
                , ULIS3_NOCB
                , &debug_stamp
                , BrushContext->GetState().target_temp_buffer->GetBlock()
                , debug_stamp.Rect()
                , ::ul3::FVec2F( BrushContext->GetX() - size / 2, BrushContext->GetY() - size / 2 )
                , ULIS3_AA
                , ::ul3::BM_NORMAL
                , ::ul3::AM_NORMAL
                , 1.f );

    BrushContext->PushInvalidRect( invalidRect );
}


//static
void
UOdysseyBrushFunctionLibrary::SimpleStamp( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, FOdysseyPivot Pivot, float X, float Y, float Flow ) {
    if( !BrushContext ) return;
    if( !Sample.m )     return;

    FOdysseyBlock* block = Sample.m;
    ::ul3::FRect invalidRect = ComputeRectWithPivot( block, Pivot, X, Y );
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = block->Height() > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42;
    ::ul3::Blend( hULIS.ThreadPool()
                , ULIS3_BLOCKING
                , perfIntent
                , hULIS.HostDeviceInfo()
                , ULIS3_NOCB
                , block->GetBlock()
                , BrushContext->GetState().target_temp_buffer->GetBlock()
                , block->GetBlock()->Rect()
                , ::ul3::FVec2F( invalidRect.x, invalidRect.y )
                , ULIS3_NOAA
                , ::ul3::BM_NORMAL
                , ::ul3::AM_NORMAL
                , FMath::Clamp( Flow, 0.f, 1.f ) );

    BrushContext->PushInvalidRect( invalidRect );
}


//static
void
UOdysseyBrushFunctionLibrary::Stamp( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, FOdysseyPivot Pivot, float X, float Y, float Flow, EOdysseyBlendingMode BlendingMode, EOdysseyAlphaMode AlphaMode ) {
    if( !BrushContext ) return;
    if( !Sample.m )     return;

    FOdysseyBlock* block = Sample.m;
    ::ul3::FRect invalidRect = ComputeRectWithPivot( block, Pivot, X, Y );
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = block->Height() > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42;
    ::ul3::Blend( hULIS.ThreadPool()
                , ULIS3_BLOCKING
                , perfIntent
                , hULIS.HostDeviceInfo()
                , ULIS3_NOCB
                , block->GetBlock()
                , BrushContext->GetState().target_temp_buffer->GetBlock()
                , block->GetBlock()->Rect()
                , ::ul3::FVec2F( invalidRect.x, invalidRect.y )
                , ULIS3_NOAA
                , static_cast< ::ul3::eBlendingMode >( BlendingMode )
                , static_cast< ::ul3::eAlphaMode >( AlphaMode )
                , FMath::Clamp( Flow, 0.f, 1.f ) );
    BrushContext->PushInvalidRect( invalidRect );
}


//static
void
UOdysseyBrushFunctionLibrary::GenerateOrbitDelta( float AngleRad, float  Radius, float& DeltaX, float& DeltaY )
{
    float cosa = cos( AngleRad );
    float sina = sin( AngleRad );
    DeltaX = cosa * Radius;
    DeltaY = sina * Radius;
}

#undef LOCTEXT_NAMESPACE


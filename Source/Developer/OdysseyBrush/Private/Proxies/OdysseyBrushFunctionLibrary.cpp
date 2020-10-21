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
UOdysseyBrushFunctionLibrary::DebugStamp( UOdysseyBrushAssetBase* BrushContext )
{
    if( !BrushContext || !BrushContext->GetState().target_temp_buffer)
        return;

    int size = ::ul3::FMaths::Max( BrushContext->GetSizeModifier() * BrushContext->GetPressure(), 1.f );

    ::ul3::FBlock debug_stamp( size, size, BrushContext->GetState().target_temp_buffer->Format() );
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
UOdysseyBrushFunctionLibrary::SimpleStamp( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, FOdysseyPivot Pivot, float X, float Y, float Flow, bool iAntiAliasing )
{
    if( !BrushContext || !BrushContext->GetState().target_temp_buffer) return;
    if( !Sample.m )     return;

    FOdysseyBlock* block = Sample.m;
    FRectF invalidRect = ComputeRectWithPivot( block, Pivot, X, Y );    //PATCH: until ::ulis3::FRectF
    //::ul3::FRect invalidRect = ComputeRectWithPivot( block, Pivot, X, Y );
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = block->Height() > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42;

	::ul3::tFormat block_format = block->Format();
	::ul3::tFormat target_format = BrushContext->GetState().target_temp_buffer->Format();
	if (block_format == target_format)
	{
		::ul3::Blend(hULIS.ThreadPool()
			, ULIS3_BLOCKING
			, perfIntent
			, hULIS.HostDeviceInfo()
			, ULIS3_NOCB
			, block->GetBlock()
			, BrushContext->GetState().target_temp_buffer->GetBlock()
			, block->GetBlock()->Rect()
			, ::ul3::FVec2F(invalidRect.x, invalidRect.y)
			, iAntiAliasing
			, ::ul3::BM_NORMAL
			, ::ul3::AM_NORMAL
			, FMath::Clamp(Flow, 0.f, 1.f));
	}
	else
	{
		FOdysseyBlock* conv = new FOdysseyBlock(block->Width(), block->Height(), target_format);
		::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, block->GetBlock(), conv->GetBlock());
		::ul3::Blend(hULIS.ThreadPool()
			, ULIS3_BLOCKING
			, perfIntent
			, hULIS.HostDeviceInfo()
			, ULIS3_NOCB
			, conv->GetBlock()
			, BrushContext->GetState().target_temp_buffer->GetBlock()
			, conv->GetBlock()->Rect()
			, ::ul3::FVec2F(invalidRect.x, invalidRect.y)
			, iAntiAliasing
			, ::ul3::BM_NORMAL
			, ::ul3::AM_NORMAL
			, FMath::Clamp(Flow, 0.f, 1.f));

		delete conv;
	}

    ::ul3::FRect invalidRectI( FMath::FloorToInt( invalidRect.x ), FMath::FloorToInt( invalidRect.y ), FMath::CeilToInt( invalidRect.w + 2 ), FMath::CeilToInt( invalidRect.h + 2 ) );
    BrushContext->PushInvalidRect( invalidRectI );
}


//static
void
UOdysseyBrushFunctionLibrary::Stamp( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, FOdysseyPivot Pivot, float X, float Y, float Flow, bool iAntiAliasing, EOdysseyBlendingMode BlendingMode, EOdysseyAlphaMode AlphaMode )
{
    if( !BrushContext || !BrushContext->GetState().target_temp_buffer ) return;
    if( !Sample.m )     return;

    FOdysseyBlock* block = Sample.m;
    FRectF invalidRect = ComputeRectWithPivot( block, Pivot, X, Y );    //PATCH: until ::ulis3::FRectF
    //::ul3::FRect invalidRect = ComputeRectWithPivot( block, Pivot, X, Y );
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = block->Height() > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42;

	::ul3::tFormat block_format = block->Format();
	::ul3::tFormat target_format = BrushContext->GetState().target_temp_buffer->Format();
	if (block_format == target_format)
	{
		::ul3::Blend(hULIS.ThreadPool()
			, ULIS3_BLOCKING
			, perfIntent
			, hULIS.HostDeviceInfo()
			, ULIS3_NOCB
			, block->GetBlock()
			, BrushContext->GetState().target_temp_buffer->GetBlock()
			, block->GetBlock()->Rect()
			, ::ul3::FVec2F(invalidRect.x, invalidRect.y)
			, iAntiAliasing
			, static_cast<::ul3::eBlendingMode>(BlendingMode)
			, static_cast<::ul3::eAlphaMode>(AlphaMode)
			, FMath::Clamp(Flow, 0.f, 1.f));
	}
	else
	{
		FOdysseyBlock* conv = new FOdysseyBlock(block->Width(), block->Height(), target_format);
		::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, block->GetBlock(), conv->GetBlock());
		::ul3::Blend(hULIS.ThreadPool()
			, ULIS3_BLOCKING
			, perfIntent
			, hULIS.HostDeviceInfo()
			, ULIS3_NOCB
			, conv->GetBlock()
			, BrushContext->GetState().target_temp_buffer->GetBlock()
			, conv->GetBlock()->Rect()
			, ::ul3::FVec2F(invalidRect.x, invalidRect.y)
			, iAntiAliasing
			, static_cast<::ul3::eBlendingMode>(BlendingMode)
			, static_cast<::ul3::eAlphaMode>(AlphaMode)
			, FMath::Clamp(Flow, 0.f, 1.f));

		delete conv;
	}

    ::ul3::FRect invalidRectI( FMath::FloorToInt( invalidRect.x ), FMath::FloorToInt( invalidRect.y ), FMath::CeilToInt( invalidRect.w + 2 ), FMath::CeilToInt( invalidRect.h + 2 ) );
    BrushContext->PushInvalidRect( invalidRectI );
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


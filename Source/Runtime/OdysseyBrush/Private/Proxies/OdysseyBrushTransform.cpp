// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "Proxies/OdysseyBrushTransform.h"
#include "OdysseyBrushContext.h"
#include "OdysseyBrushAssetBase.h"
#include "OdysseyBlock.h"
#include <ULIS_CORE>

/////////////////////////////////////////////////////
// UOdysseyTransformProxyLibrary
//static
FOdysseyMatrix
UOdysseyTransformProxyLibrary::MakeIdentityMatrix()
{
    return  ::ULIS::FTransformContext::GetIdentityMatrix();
}


//static
FOdysseyMatrix
UOdysseyTransformProxyLibrary::MakeTranslationMatrix( float DeltaX, float DeltaY )
{
    return  ::ULIS::FTransformContext::GetTranslationMatrix(  DeltaX, DeltaY );
}


//static
FOdysseyMatrix
UOdysseyTransformProxyLibrary::MakeRotationMatrix( float Angle )
{
    return  ::ULIS::FTransformContext::GetRotationMatrix( ( Angle * 3.14159265359f ) / 180.f );
}


//static
FOdysseyMatrix
UOdysseyTransformProxyLibrary::MakeScaleMatrix( float ScaleX, float ScaleY )
{
    return  ::ULIS::FTransformContext::GetScaleMatrix( ScaleX, ScaleY );
}


//static
FOdysseyMatrix
UOdysseyTransformProxyLibrary::MakeShearMatrix( float ShearX, float ShearY )
{
    return  ::ULIS::FTransformContext::GetShearMatrix( ShearX, ShearY );
}


//static
FOdysseyMatrix
UOdysseyTransformProxyLibrary::ComposeMatrix( const FOdysseyMatrix& First, const FOdysseyMatrix& Second )
{
    return  FOdysseyMatrix( Second.mat * First.mat );
}


//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::Transform( FOdysseyBlockProxy Sample, FOdysseyMatrix Transform, EResamplingMethod ResamplingMethod, ECacheLevel Cache )
{
    ODYSSEY_BRUSH_CONTEXT_CHECK
    ODYSSEY_BRUSH_BLOCK_PROXY_CHECK_RETURN_VALUE( FOdysseyBlockProxy::MakeNullProxy() )

    FOdysseyBlockProxy prox;
    FString op = "Transform_" + Transform.ID() + "_" + Sample.id;
    ODYSSEY_BRUSH_CACHE_OPERATION_START( Cache, op )
        FOdysseyBlock* src = Sample.m;
        ::ULIS::FRect box = ::ULIS::FTransformContext::GetTransformPreviewRect( src->GetIBlock(), Transform.mat );
        FOdysseyBlock* dst = new FOdysseyBlock( box.w, box.h, src->GetUE4TextureSourceFormat() );
        ::ULIS::FTransformContext::TransformInto( src->GetIBlock(), dst->GetIBlock(), Transform.mat, (::ULIS::eResamplingMethod)ResamplingMethod );

        prox = FOdysseyBlockProxy( dst, op );
        brush->StoreInPool( Cache, op, prox );
    ODYSSEY_BRUSH_CACHE_OPERATION_END

    return  prox;
}

//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::Rotate( FOdysseyBlockProxy Sample, float Angle, EResamplingMethod ResamplingMethod, ECacheLevel Cache )
{
    ODYSSEY_BRUSH_CONTEXT_CHECK
    ODYSSEY_BRUSH_BLOCK_PROXY_CHECK_RETURN_VALUE( FOdysseyBlockProxy::MakeNullProxy() )

    FOdysseyBlockProxy prox;
    FString op = "Rotate_" + FString::SanitizeFloat( Angle ) + "_" + Sample.id;
    ODYSSEY_BRUSH_CACHE_OPERATION_START( Cache, op )
        FOdysseyBlock* src = Sample.m;
        auto mat = ::ULIS::FTransformContext::GetRotationMatrix( ( Angle * 3.14159265359f ) / 180.f );
        ::ULIS::FRect box = ::ULIS::FTransformContext::GetTransformPreviewRect( src->GetIBlock(), mat );
        FOdysseyBlock* dst = new FOdysseyBlock( box.w, box.h, src->GetUE4TextureSourceFormat() );
        ::ULIS::FTransformContext::TransformInto( src->GetIBlock(), dst->GetIBlock(), mat, (::ULIS::eResamplingMethod)ResamplingMethod );

        prox = FOdysseyBlockProxy( dst, op );
        brush->StoreInPool( Cache, op, prox );
    ODYSSEY_BRUSH_CACHE_OPERATION_END

    return  prox;
}


//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::ScaleUniform( FOdysseyBlockProxy Sample, float Scale, EResamplingMethod ResamplingMethod, ECacheLevel Cache )
{
    ODYSSEY_BRUSH_CONTEXT_CHECK
    ODYSSEY_BRUSH_BLOCK_PROXY_CHECK_RETURN_VALUE( FOdysseyBlockProxy::MakeNullProxy() )

    FOdysseyBlockProxy prox;
    FString op = "ScaleUniform_" + FString::SanitizeFloat( Scale ) + "_" + Sample.id;
    ODYSSEY_BRUSH_CACHE_OPERATION_START( Cache, op )
        FOdysseyBlock* src = Sample.m;
        auto mat = ::ULIS::FTransformContext::GetScaleMatrix( Scale, Scale );
        ::ULIS::FRect box = ::ULIS::FTransformContext::GetTransformPreviewRect( src->GetIBlock(), mat );
        FOdysseyBlock* dst = new FOdysseyBlock( box.w, box.h, src->GetUE4TextureSourceFormat() );
        ::ULIS::FTransformContext::TransformInto( src->GetIBlock(), dst->GetIBlock(), mat, (::ULIS::eResamplingMethod)ResamplingMethod );

        prox = FOdysseyBlockProxy( dst, op );
        brush->StoreInPool( Cache, op, prox );
    ODYSSEY_BRUSH_CACHE_OPERATION_END

    return  prox;
}


//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::ScaleXY( FOdysseyBlockProxy Sample, float ScaleX, float ScaleY, EResamplingMethod ResamplingMethod, ECacheLevel Cache )
{
    ODYSSEY_BRUSH_CONTEXT_CHECK
    ODYSSEY_BRUSH_BLOCK_PROXY_CHECK_RETURN_VALUE( FOdysseyBlockProxy::MakeNullProxy() )

    FOdysseyBlockProxy prox;
    FString op = "Scale_" + FString::SanitizeFloat( ScaleX ) + FString::SanitizeFloat( ScaleY ) + "_" + Sample.id;
    ODYSSEY_BRUSH_CACHE_OPERATION_START( Cache, op )
        FOdysseyBlock* src = Sample.m;
        auto mat = ::ULIS::FTransformContext::GetScaleMatrix( ScaleX, ScaleY );
        ::ULIS::FRect box = ::ULIS::FTransformContext::GetTransformPreviewRect( src->GetIBlock(), mat );
        FOdysseyBlock* dst = new FOdysseyBlock( box.w, box.h, src->GetUE4TextureSourceFormat() );
        ::ULIS::FTransformContext::TransformInto( src->GetIBlock(), dst->GetIBlock(), mat, (::ULIS::eResamplingMethod)ResamplingMethod );

        prox = FOdysseyBlockProxy( dst, op );
        brush->StoreInPool( Cache, op, prox );
    ODYSSEY_BRUSH_CACHE_OPERATION_END

    return  prox;
}


//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::Shear( FOdysseyBlockProxy Sample, float ShearX, float ShearY, EResamplingMethod ResamplingMethod, ECacheLevel Cache )
{
    ODYSSEY_BRUSH_CONTEXT_CHECK
    ODYSSEY_BRUSH_BLOCK_PROXY_CHECK_RETURN_VALUE( FOdysseyBlockProxy::MakeNullProxy() )

    FOdysseyBlockProxy prox;
    FString op = "Shear_" + FString::SanitizeFloat( ShearX ) + FString::SanitizeFloat( ShearY ) + "_" + Sample.id;
    ODYSSEY_BRUSH_CACHE_OPERATION_START( Cache, op )
        FOdysseyBlock* src = Sample.m;
        auto mat = ::ULIS::FTransformContext::GetShearMatrix( ShearX, ShearY );
        ::ULIS::FRect box = ::ULIS::FTransformContext::GetTransformPreviewRect( src->GetIBlock(), mat );
        FOdysseyBlock* dst = new FOdysseyBlock( box.w, box.h, src->GetUE4TextureSourceFormat() );
        ::ULIS::FTransformContext::TransformInto( src->GetIBlock(), dst->GetIBlock(), mat, (::ULIS::eResamplingMethod)ResamplingMethod );

        prox = FOdysseyBlockProxy( dst, op );
        brush->StoreInPool( Cache, op, prox );
    ODYSSEY_BRUSH_CACHE_OPERATION_END

    return  prox;
}


//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::ResizeUniform( FOdysseyBlockProxy Sample, float Size, EResamplingMethod ResamplingMethod, ECacheLevel Cache )
{
    ODYSSEY_BRUSH_CONTEXT_CHECK
    ODYSSEY_BRUSH_BLOCK_PROXY_CHECK_RETURN_VALUE( FOdysseyBlockProxy::MakeNullProxy() )

    FOdysseyBlockProxy prox;
    FString op = "ResizeUniform_" + FString::SanitizeFloat( Size ) + "_" + Sample.id;
    ODYSSEY_BRUSH_CACHE_OPERATION_START( Cache, op )
        FOdysseyBlock* src = Sample.m;
        int src_width  = src->Width();
        int src_height = src->Height();
        float max = FMath::Max( src_width, src_height );
        float ratio = Size / max;

        auto mat = ::ULIS::FTransformContext::GetScaleMatrix( ratio, ratio );
        ::ULIS::FRect box = ::ULIS::FTransformContext::GetTransformPreviewRect( src->GetIBlock(), mat );
        FOdysseyBlock* dst = new FOdysseyBlock( box.w, box.h, src->GetUE4TextureSourceFormat() );
        ::ULIS::FTransformContext::TransformInto( src->GetIBlock(), dst->GetIBlock(), mat, (::ULIS::eResamplingMethod)ResamplingMethod );

        prox = FOdysseyBlockProxy( dst, op );
        brush->StoreInPool( Cache, op, prox );
    ODYSSEY_BRUSH_CACHE_OPERATION_END

    return  prox;
}


//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::Resize( FOdysseyBlockProxy Sample, float SizeX, float SizeY, EResamplingMethod ResamplingMethod, ECacheLevel Cache )
{
    ODYSSEY_BRUSH_CONTEXT_CHECK
    ODYSSEY_BRUSH_BLOCK_PROXY_CHECK_RETURN_VALUE( FOdysseyBlockProxy::MakeNullProxy() )

    FOdysseyBlockProxy prox;
    FString op = "Resize_" + FString::SanitizeFloat( SizeX ) + FString::SanitizeFloat( SizeY ) + "_" + Sample.id;
    ODYSSEY_BRUSH_CACHE_OPERATION_START( Cache, op )
        FOdysseyBlock* src = Sample.m;
        float src_width  = src->Width();
        float src_height = src->Height();
        float ratioX = SizeX / src_width;
        float ratioY = SizeY / src_height;

        auto mat = ::ULIS::FTransformContext::GetScaleMatrix( ratioX, ratioY );
        ::ULIS::FRect box = ::ULIS::FTransformContext::GetTransformPreviewRect( src->GetIBlock(), mat );
        FOdysseyBlock* dst = new FOdysseyBlock( box.w, box.h, src->GetUE4TextureSourceFormat() );
        ::ULIS::FTransformContext::TransformInto( src->GetIBlock(), dst->GetIBlock(), mat, (::ULIS::eResamplingMethod)ResamplingMethod );

        prox = FOdysseyBlockProxy( dst, op );
        brush->StoreInPool( Cache, op, prox );
    ODYSSEY_BRUSH_CACHE_OPERATION_END

    return  prox;
}


//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::FlipX( FOdysseyBlockProxy Sample, EResamplingMethod ResamplingMethod, ECacheLevel Cache )
{
    return  ScaleXY( Sample, -1, 1, ResamplingMethod, Cache );
}


//stati
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::FlipY( FOdysseyBlockProxy Sample, EResamplingMethod ResamplingMethod, ECacheLevel Cache )
{
    return  ScaleXY( Sample, 1, -1, ResamplingMethod, Cache );
}


//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::FlipXY( FOdysseyBlockProxy Sample, EResamplingMethod ResamplingMethod, ECacheLevel Cache )
{
    return  ScaleXY( Sample, -1, -1, ResamplingMethod, Cache );
}


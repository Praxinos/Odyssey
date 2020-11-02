// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "Proxies/OdysseyBrushTransform.h"
#include "OdysseyBrushAssetBase.h"
#include "OdysseyBlock.h"
#include <ULIS3>
#include "ULISLoaderModule.h"

/////////////////////////////////////////////////////
// Odyssey Resampling
FOdysseyMatrix::FOdysseyMatrix()
    : m()
{}

FOdysseyMatrix::FOdysseyMatrix( const  ::ul3::FTransform2D&  iMat )
    : m( iMat )
{}

FString
FOdysseyMatrix::ID() const
{
    return  FString::FromBlob( m.Ptr(), 9 * sizeof( float ) );
}

const ::ul3::FTransform2D&
FOdysseyMatrix::GetValue() const {
    return  m;
}

/////////////////////////////////////////////////////
// UOdysseyTransformProxyLibrary
//static
FOdysseyMatrix
UOdysseyTransformProxyLibrary::MakeIdentityMatrix()
{
    return  FOdysseyMatrix( ::ul3::FTransform2D::MakeIdentityTransform() );
}


//static
FOdysseyMatrix
UOdysseyTransformProxyLibrary::MakeTranslationMatrix( float DeltaX, float DeltaY )
{
    return  FOdysseyMatrix( ::ul3::FTransform2D::MakeTranslationTransform( DeltaX, DeltaY ) );
}


//static
FOdysseyMatrix
UOdysseyTransformProxyLibrary::MakeRotationMatrix( float Deg )
{
    return  FOdysseyMatrix( ::ul3::FTransform2D::MakeRotationTransform( Deg * 3.14159265359 / 180.0 ) );
}


//static
FOdysseyMatrix
UOdysseyTransformProxyLibrary::MakeScaleMatrix( float ScaleX, float ScaleY )
{
    return  FOdysseyMatrix( ::ul3::FTransform2D::MakeScaleTransform( ScaleX, ScaleY ) );
}


//static
FOdysseyMatrix
UOdysseyTransformProxyLibrary::MakeShearMatrix( float ShearX, float ShearY )
{
    return  FOdysseyMatrix( ::ul3::FTransform2D::MakeShearTransform( ShearX, ShearY ) );
}


//static
FOdysseyMatrix
UOdysseyTransformProxyLibrary::ComposeMatrix( const FOdysseyMatrix& First, const FOdysseyMatrix& Second )
{
    return  FOdysseyMatrix( ::ul3::FTransform2D::ComposeTransforms( Second.GetValue(), First.GetValue() ) );
}


//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::Transform( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, FOdysseyMatrix Transform, EResamplingMethod ResamplingMethod ) {
    if( !BrushContext )
        return FOdysseyBlockProxy::MakeNullProxy();
    
    if( !Sample.m )
        return FOdysseyBlockProxy::MakeNullProxy();

    
    TSharedPtr<FOdysseyBlock> src = Sample.m;
    ::ul3::FRect box = ::ul3::TransformAffineMetrics( src->GetBlock()->Rect(), Transform.GetValue(), static_cast< ::ul3::eResamplingMethod >( ResamplingMethod ) );
    if( box.Area() <= 0 )
        return FOdysseyBlockProxy::MakeNullProxy();

    
	TSharedPtr<FOdysseyBlock> dst = MakeShareable(new  FOdysseyBlock( box.w, box.h, src->Format() ));
    ::ul3::ClearRaw(dst->GetBlock());

    ::ul3::FTransform2D fixedTransform( ::ul3::FTransform2D::ComposeTransforms( ::ul3::FTransform2D::MakeTranslationTransform( static_cast< float >( -box.x ), static_cast< float >( -box.y ) ), Transform.GetValue() ) );

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = dst->Height() > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;

    ::ul3::TransformAffine( hULIS.ThreadPool()
                            , ULIS3_BLOCKING
                            , perfIntent
                            , hULIS.HostDeviceInfo()
                            , ULIS3_NOCB
                            , src->GetBlock()
                            , dst->GetBlock()
                            , src->GetBlock()->Rect()
                            , fixedTransform
                            , static_cast< ::ul3::eResamplingMethod >( ResamplingMethod ) );

    return FOdysseyBlockProxy(dst);
}

//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::Rotate( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, float Angle, EResamplingMethod ResamplingMethod ) {
    if( !BrushContext )
        return FOdysseyBlockProxy::MakeNullProxy();
    
    if( !Sample.m )
        return FOdysseyBlockProxy::MakeNullProxy();

    
    TSharedPtr<FOdysseyBlock> src = Sample.m;
    ::ul3::FTransform2D mat( ::ul3::FTransform2D::MakeRotationTransform( ::ul3::FMaths::DegToRadF( Angle ) ) );
    ::ul3::FRect box = ::ul3::TransformAffineMetrics( src->GetBlock()->Rect(), mat, static_cast< ::ul3::eResamplingMethod >( ResamplingMethod ) );
    if( box.Area() <= 0 )
        return FOdysseyBlockProxy::MakeNullProxy();

    
	TSharedPtr<FOdysseyBlock> dst = MakeShareable(new  FOdysseyBlock( box.w, box.h, src->Format() ));
    ::ul3::ClearRaw(dst->GetBlock());

    ::ul3::FTransform2D fixedTransform( ::ul3::FTransform2D::ComposeTransforms( ::ul3::FTransform2D::MakeTranslationTransform( static_cast< float >( -box.x ), static_cast< float >( -box.y ) ), mat ) );

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = dst->Height() > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;

    ::ul3::TransformAffine( hULIS.ThreadPool()
                            , ULIS3_BLOCKING
                            , perfIntent
                            , hULIS.HostDeviceInfo()
                            , ULIS3_NOCB
                            , src->GetBlock()
                            , dst->GetBlock()
                            , src->GetBlock()->Rect()
                            , fixedTransform
                            , static_cast< ::ul3::eResamplingMethod >( ResamplingMethod ) );

    return FOdysseyBlockProxy(dst);
}


//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::ScaleUniform( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, float Scale, EResamplingMethod ResamplingMethod ) {
    if( !BrushContext )
        return FOdysseyBlockProxy::MakeNullProxy();
    if( !Sample.m )
        return FOdysseyBlockProxy::MakeNullProxy();

    TSharedPtr<FOdysseyBlock> src = Sample.m;
    ::ul3::FTransform2D mat( ::ul3::FTransform2D::MakeScaleTransform( Scale, Scale ) );
    ::ul3::FRect box = ::ul3::TransformAffineMetrics( src->GetBlock()->Rect(), mat, static_cast< ::ul3::eResamplingMethod >( ResamplingMethod ) );
    if( box.Area() <= 0 )
        return FOdysseyBlockProxy::MakeNullProxy();

    
	TSharedPtr<FOdysseyBlock> dst = MakeShareable(new  FOdysseyBlock( box.w, box.h, src->Format() ));
    ::ul3::ClearRaw(dst->GetBlock());

    ::ul3::FTransform2D fixedTransform( ::ul3::FTransform2D::ComposeTransforms( ::ul3::FTransform2D::MakeTranslationTransform( static_cast< float >( -box.x ), static_cast< float >( -box.y ) ), mat ) );
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = dst->Height() > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;

    ::ul3::TransformAffine( hULIS.ThreadPool()
                            , ULIS3_BLOCKING
                            , perfIntent
                            , hULIS.HostDeviceInfo()
                            , ULIS3_NOCB
                            , src->GetBlock()
                            , dst->GetBlock()
                            , src->GetBlock()->Rect()
                            , fixedTransform
                            , static_cast< ::ul3::eResamplingMethod >( ResamplingMethod ) );

    return FOdysseyBlockProxy(dst);
}


//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::ScaleXY( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, float ScaleX, float ScaleY, EResamplingMethod ResamplingMethod ) {
    if( !BrushContext )
        return FOdysseyBlockProxy::MakeNullProxy();
    if( !Sample.m )
        return FOdysseyBlockProxy::MakeNullProxy();
		
    TSharedPtr<FOdysseyBlock> src = Sample.m;
    ::ul3::FTransform2D mat( ::ul3::FTransform2D::MakeScaleTransform( ScaleX, ScaleY ) );
    ::ul3::FRect box = ::ul3::TransformAffineMetrics( src->GetBlock()->Rect(), mat, static_cast< ::ul3::eResamplingMethod >( ResamplingMethod ) );
    if( box.Area() <= 0 )
        return FOdysseyBlockProxy::MakeNullProxy();

    
	TSharedPtr<FOdysseyBlock> dst = MakeShareable(new  FOdysseyBlock( box.w, box.h, src->Format() ));
    ::ul3::ClearRaw(dst->GetBlock());

    ::ul3::FTransform2D fixedTransform( ::ul3::FTransform2D::ComposeTransforms( ::ul3::FTransform2D::MakeTranslationTransform( static_cast< float >( -box.x ), static_cast< float >( -box.y ) ), mat ) );

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = dst->Height() > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;

    ::ul3::TransformAffine( hULIS.ThreadPool()
                            , ULIS3_BLOCKING
                            , perfIntent
                            , hULIS.HostDeviceInfo()
                            , ULIS3_NOCB
                            , src->GetBlock()
                            , dst->GetBlock()
                            , src->GetBlock()->Rect()
                            , fixedTransform
                            , static_cast< ::ul3::eResamplingMethod >( ResamplingMethod ) );

    return FOdysseyBlockProxy(dst);
}


//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::Shear( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, float ShearX, float ShearY, EResamplingMethod ResamplingMethod ) {
    if( !BrushContext )
        return FOdysseyBlockProxy::MakeNullProxy();

    if( !Sample.m )
        return FOdysseyBlockProxy::MakeNullProxy();

    TSharedPtr<FOdysseyBlock> src = Sample.m;
    ::ul3::FTransform2D mat( ::ul3::FTransform2D::MakeShearTransform( ShearX, ShearY ) );
    ::ul3::FRect box = ::ul3::TransformAffineMetrics( src->GetBlock()->Rect(), mat, static_cast< ::ul3::eResamplingMethod >( ResamplingMethod ) );
    if( box.Area() <= 0 )
        return FOdysseyBlockProxy::MakeNullProxy();

    
	TSharedPtr<FOdysseyBlock> dst = MakeShareable(new  FOdysseyBlock( box.w, box.h, src->Format() ));
    ::ul3::ClearRaw(dst->GetBlock());

    ::ul3::FTransform2D fixedTransform( ::ul3::FTransform2D::ComposeTransforms( ::ul3::FTransform2D::MakeTranslationTransform( static_cast< float >( -box.x ), static_cast< float >( -box.y ) ), mat ) );
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = dst->Height() > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;

    ::ul3::TransformAffine( hULIS.ThreadPool()
                            , ULIS3_BLOCKING
                            , perfIntent
                            , hULIS.HostDeviceInfo()
                            , ULIS3_NOCB
                            , src->GetBlock()
                            , dst->GetBlock()
                            , src->GetBlock()->Rect()
                            , fixedTransform
                            , static_cast< ::ul3::eResamplingMethod >( ResamplingMethod ) );

    return FOdysseyBlockProxy(dst);
}


//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::ResizeUniform( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, float Size, EResamplingMethod ResamplingMethod ) {
    if( !BrushContext )
        return FOdysseyBlockProxy::MakeNullProxy();

    if( !Sample.m )
        return FOdysseyBlockProxy::MakeNullProxy();

    TSharedPtr<FOdysseyBlock> src = Sample.m;
    int src_width  = src->Width();
    int src_height = src->Height();
    float max = FMath::Max( src_width, src_height );
    float ratio = Size / max;

    ::ul3::FTransform2D mat( ::ul3::FTransform2D::MakeScaleTransform( ratio, ratio ) );
    ::ul3::FRect box = ::ul3::TransformAffineMetrics( src->GetBlock()->Rect(), mat, static_cast< ::ul3::eResamplingMethod >( ResamplingMethod ) );
    if( box.Area() <= 0 )
        return FOdysseyBlockProxy::MakeNullProxy();

    
	TSharedPtr<FOdysseyBlock> dst = MakeShareable(new  FOdysseyBlock( box.w, box.h, src->Format() ));
    ::ul3::ClearRaw(dst->GetBlock());
    
    ::ul3::FTransform2D fixedTransform( ::ul3::FTransform2D::ComposeTransforms( ::ul3::FTransform2D::MakeTranslationTransform( static_cast< float >( -box.x ), static_cast< float >( -box.y ) ), mat ) );

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = dst->Height() > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;

    ::ul3::TransformAffine( hULIS.ThreadPool()
                            , ULIS3_BLOCKING
                            , perfIntent
                            , hULIS.HostDeviceInfo()
                            , ULIS3_NOCB
                            , src->GetBlock()
                            , dst->GetBlock()
                            , src->GetBlock()->Rect()
                            , fixedTransform
                            , static_cast< ::ul3::eResamplingMethod >( ResamplingMethod ) );

    return FOdysseyBlockProxy(dst);

}


//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::Resize( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, float SizeX, float SizeY, EResamplingMethod ResamplingMethod ) {
    if( !BrushContext )
        return FOdysseyBlockProxy::MakeNullProxy();

    if( !Sample.m )
        return FOdysseyBlockProxy::MakeNullProxy();

    TSharedPtr<FOdysseyBlock> src = Sample.m;
    float src_width  = src->Width();
    float src_height = src->Height();
    float ratioX = SizeX / src_width;
    float ratioY = SizeY / src_height;

    ::ul3::FTransform2D mat( ::ul3::FTransform2D::MakeScaleTransform( ratioX, ratioY ) );
    ::ul3::FRect box = ::ul3::TransformAffineMetrics( src->GetBlock()->Rect(), mat, static_cast< ::ul3::eResamplingMethod >( ResamplingMethod ) );
    if( box.Area() <= 0 )
        return FOdysseyBlockProxy::MakeNullProxy();

    
	TSharedPtr<FOdysseyBlock> dst = MakeShareable(new  FOdysseyBlock( box.w, box.h, src->Format() ));
    ::ul3::ClearRaw(dst->GetBlock());

    ::ul3::FTransform2D fixedTransform( ::ul3::FTransform2D::ComposeTransforms( ::ul3::FTransform2D::MakeTranslationTransform( static_cast< float >( -box.x ), static_cast< float >( -box.y ) ), mat ) );
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = dst->Height() > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;

    ::ul3::TransformAffine( hULIS.ThreadPool()
                            , ULIS3_BLOCKING
                            , perfIntent
                            , hULIS.HostDeviceInfo()
                            , ULIS3_NOCB
                            , src->GetBlock()
                            , dst->GetBlock()
                            , src->GetBlock()->Rect()
                            , fixedTransform
                            , static_cast< ::ul3::eResamplingMethod >( ResamplingMethod ) );

    return FOdysseyBlockProxy(dst);
}


//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::FlipX( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, EResamplingMethod ResamplingMethod ) {
    return  ScaleXY( BrushContext, Sample, -1, 1, ResamplingMethod );
}


//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::FlipY( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, EResamplingMethod ResamplingMethod ) {
    return  ScaleXY( BrushContext, Sample, 1, -1, ResamplingMethod );
}


//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::FlipXY( UOdysseyBrushAssetBase* BrushContext, FOdysseyBlockProxy Sample, EResamplingMethod ResamplingMethod ) {
    return  ScaleXY( BrushContext, Sample, -1, -1, ResamplingMethod );
}


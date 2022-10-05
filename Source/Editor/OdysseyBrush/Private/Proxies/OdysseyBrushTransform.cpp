// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Proxies/OdysseyBrushTransform.h"
#include "OdysseyBrushAssetBase.h"
#include <ULIS>
#include "ULISLoaderModule.h"

/////////////////////////////////////////////////////
// Odyssey Resampling
FOdysseyMatrix::FOdysseyMatrix()
    : m()
{}

FOdysseyMatrix::FOdysseyMatrix( const ::ULIS::FMat3F& iMat )
    : m( iMat )
{}

/////////////////////////////////////////////////////
// UOdysseyTransformProxyLibrary
//static
FOdysseyMatrix
UOdysseyTransformProxyLibrary::MakeIdentityMatrix()
{
    return  FOdysseyMatrix( ::ULIS::FMat3F() );
}


//static
FOdysseyMatrix
UOdysseyTransformProxyLibrary::MakeTranslationMatrix( float DeltaX, float DeltaY )
{
    return  FOdysseyMatrix( ::ULIS::FMat3F::MakeTranslationMatrix( DeltaX, DeltaY ) );
}


//static
FOdysseyMatrix
UOdysseyTransformProxyLibrary::MakeRotationMatrix( float Deg )
{
    return  FOdysseyMatrix( ::ULIS::FMat3F::MakeRotationMatrix( ::ULIS::FMath::DegToRadF( Deg ) ) );
}


//static
FOdysseyMatrix
UOdysseyTransformProxyLibrary::MakeScaleMatrix( float ScaleX, float ScaleY )
{
    return  FOdysseyMatrix( ::ULIS::FMat3F::MakeScaleMatrix( ScaleX, ScaleY ) );
}


//static
FOdysseyMatrix
UOdysseyTransformProxyLibrary::MakeShearMatrix( float ShearX, float ShearY )
{
    return  FOdysseyMatrix( ::ULIS::FMat3F::MakeSkewMatrix( ShearX, ShearY ) );
}

//static
FOdysseyMatrix
UOdysseyTransformProxyLibrary::MakePerspectiveMatrix( 
      FVector2D SrcA, FVector2D SrcB, FVector2D SrcC, FVector2D SrcD
    , FVector2D DstA, FVector2D DstB, FVector2D DstC, FVector2D DstD 
)
{
    ::ULIS::FVec2F src[4] = { ::ULIS::FVec2F( SrcA.X, SrcA.Y ), ::ULIS::FVec2F( SrcB.X, SrcB.Y ), ::ULIS::FVec2F( SrcC.X, SrcC.Y ), ::ULIS::FVec2F( SrcD.X, SrcD.Y ) };
    ::ULIS::FVec2F dst[4] = { ::ULIS::FVec2F( DstA.X, DstA.Y ), ::ULIS::FVec2F( DstB.X, DstB.Y ), ::ULIS::FVec2F( DstC.X, DstC.Y ), ::ULIS::FVec2F( DstD.X, DstD.Y ) };
    return FOdysseyMatrix( ::ULIS::FMat3F::MakeHomography( src, dst ) );
}


//static
FOdysseyMatrix
UOdysseyTransformProxyLibrary::ComposeMatrix( const FOdysseyMatrix& First, const FOdysseyMatrix& Second )
{
    // TODO: Check non commutative order here in case we broke something in brushes
    return  FOdysseyMatrix( Second.m * First.m );
}


//static
FOdysseyBrushRect
UOdysseyTransformProxyLibrary::GetMatrixResultRect( const FOdysseyMatrix& Matrix, const FOdysseyBrushRect& Rectangle, EResamplingMethod ResamplingMethod )
{
    return FOdysseyBrushRect( ::ULIS::FContext::TransformAffineMetrics( Rectangle.GetValue(), Matrix.m ) );
}

//static
FOdysseyBrushRect
UOdysseyTransformProxyLibrary::GetPerspectiveMatrixResultRect( const FOdysseyMatrix& PerspectiveMatrix, const FOdysseyBrushRect& Rectangle, EResamplingMethod ResamplingMethod )
{
    return FOdysseyBrushRect( ::ULIS::FContext::TransformPerspectiveMetrics( Rectangle.GetValue(), PerspectiveMatrix.m ) );
}

//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::Transform( FOdysseyBlockProxy Sample, FOdysseyMatrix Transform, int OutputWidth, int OutputHeight, EResamplingMethod ResamplingMethod )
{
    if( !Sample.IsValid() )
        return FOdysseyBlockProxy::MakeNullProxy();

    if( OutputWidth <= 0 || OutputHeight <= 0 )
        return FOdysseyBlockProxy::MakeNullProxy();

    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > src_shared = Sample.GetBlock();
    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > dst_shared = MakeShareable( new ::ULIS::FBlock( OutputWidth, OutputHeight, src_shared->Format() ));

    ::ULIS::FEvent eventClear;
    ::ULIS::FEvent eventTransform;
    {
        using namespace ::ULIS;
        ::ULIS::FBlock& src = *src_shared;
        ::ULIS::FBlock& dst = *dst_shared;
        FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( src.Format() );
        ctx.Clear( dst, FRectI::Auto, FSchedulePolicy::CacheEfficient, 1, &Sample.GetEvent(), &eventClear );

        ctx.TransformAffine(
              src
            , dst
            , FRectI::Auto
            , Transform.m
            , eResamplingMethod( ResamplingMethod )
            , eBorderMode::Border_Transparent
            , ::ULIS::FColor::Transparent
            , FSchedulePolicy::MultiScanlines
            , 1
            , &eventClear
            , &eventTransform
        );
        ctx.Flush();
    }
    return FOdysseyBlockProxy::MakeProxy( dst_shared, 1, &eventTransform, 1, &Sample);
    // An old implementation we can keep in case we need to use it again later.
    // It wasn't updated to the newer ULIS versions:
    //::ULIS::FMat3F fixedTransform( ::ULIS::FMat3F::ComposeTransforms( ::ULIS::FMat3F::MakeTranslationMatrix( static_cast< float >( -box.x ), static_cast< float >( -box.y ) ), Transform.GetValue() ) );
}

//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::Rotate( FOdysseyBlockProxy Sample, float Angle, EResamplingMethod ResamplingMethod )
{
    if( !Sample.IsValid() )
        return FOdysseyBlockProxy::MakeNullProxy();

    // TODO: There is a lot of back and forth that we could optimize away in this function, namely, the fixed transform
    // is processed twice and it is not needed.
    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > src_shared = Sample.GetBlock();
    using namespace ::ULIS;
    ::ULIS::FBlock& src = *src_shared;
    float w = static_cast< float >( src.Width() );
    float h = static_cast< float >( src.Height() );

    ::ULIS::FMat3F fixedTransform(
          ::ULIS::FMat3F::MakeRotationMatrix( ::ULIS::FMath::DegToRadF( Angle ) )
        * ::ULIS::FMat3F::MakeTranslationMatrix( -w / 2, -h / 2.f )
    );

    // This is pointless because the src will not be transformed into a null area destination since it has a valid size.
    ::ULIS::FRectI box = ::ULIS::FContext::TransformAffineMetrics( src.Rect(), fixedTransform );
    if( box.Area() <= 0 )
        return FOdysseyBlockProxy::MakeNullProxy();

    // Fixing the transform to translate the result in the positive range
    fixedTransform = ::ULIS::FMat3F::MakeTranslationMatrix( static_cast<float>( -box.x ), static_cast<float>( -box.y ) ) * fixedTransform;
    box = ::ULIS::FContext::TransformAffineMetrics( src.Rect(), fixedTransform );

    // This should never happen too
    if (box.Area() <= 0)
        return FOdysseyBlockProxy::MakeNullProxy();

    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > dst_shared = MakeShareable(new ::ULIS::FBlock( box.w, box.h, src.Format() ));
    ::ULIS::FBlock& dst = *dst_shared;

    FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( src.Format() );
    ::ULIS::FEvent eventClear;
    ctx.Clear( dst, FRectI::Auto, FSchedulePolicy::CacheEfficient, 1, &Sample.GetEvent(), &eventClear );

    ::ULIS::FEvent eventTransform;
    ctx.TransformAffine(
          src
        , dst
        , FRectI::Auto
        , fixedTransform
        , eResamplingMethod( ResamplingMethod )
        , eBorderMode::Border_Transparent
        , ::ULIS::FColor::Transparent
        , FSchedulePolicy::MultiScanlines
        , 1
        , &eventClear
        , &eventTransform
    );
    ctx.Flush();

    return FOdysseyBlockProxy::MakeProxy( dst_shared, 1, &eventTransform, 1, &Sample);
}


//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::ScaleUniform( FOdysseyBlockProxy Sample, float Scale, EResamplingMethod ResamplingMethod )
{
    if( !Sample.IsValid() )
        return FOdysseyBlockProxy::MakeNullProxy();

    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > src_shared = Sample.GetBlock();
    using namespace ::ULIS;
    ::ULIS::FBlock& src = *src_shared;
    ::ULIS::FMat3F mat( ::ULIS::FMat3F::MakeScaleMatrix( Scale, Scale ) );
    ::ULIS::FRectI box = ::ULIS::FContext::TransformAffineMetrics( src.Rect(), mat );

    if( box.Area() <= 0 )
        return FOdysseyBlockProxy::MakeNullProxy();

    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > dst_shared = MakeShareable(new ::ULIS::FBlock( box.w, box.h, src.Format() ));
    ::ULIS::FBlock& dst = *dst_shared;

    ::ULIS::FMat3F fixedTransform( ::ULIS::FMat3F::MakeTranslationMatrix( static_cast< float >( -box.x ), static_cast< float >( -box.y ) ) * mat );
    FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( src.Format() );
    ::ULIS::FEvent eventClear;
    ctx.Clear( dst, FRectI::Auto, FSchedulePolicy::CacheEfficient, 1, &Sample.GetEvent(), &eventClear );
    ::ULIS::FEvent eventTransform;
    ctx.TransformAffine(
          src
        , dst
        , FRectI::Auto
        , fixedTransform
        , eResamplingMethod( ResamplingMethod )
        , eBorderMode::Border_Transparent
        , ::ULIS::FColor::Transparent
        , FSchedulePolicy::MultiScanlines
        , 1
        , &eventClear
        , &eventTransform
    );
    ctx.Flush();
    return FOdysseyBlockProxy::MakeProxy( dst_shared, 1, &eventTransform, 1, &Sample);
}


//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::ScaleXY( FOdysseyBlockProxy Sample, float ScaleX, float ScaleY, EResamplingMethod ResamplingMethod )
{
    if( !Sample.IsValid() )
        return FOdysseyBlockProxy::MakeNullProxy();

    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > src_shared = Sample.GetBlock();
    using namespace ::ULIS;
    ::ULIS::FBlock& src = *src_shared;

    ::ULIS::FMat3F mat( ::ULIS::FMat3F::MakeScaleMatrix( ScaleX, ScaleY ) );
    ::ULIS::FRectI box = ::ULIS::FContext::TransformAffineMetrics( src.Rect(), mat );

    if( box.Area() <= 0 )
        return FOdysseyBlockProxy::MakeNullProxy();

    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > dst_shared = MakeShareable(new ::ULIS::FBlock( box.w, box.h, src.Format() ));
    ::ULIS::FBlock& dst = *dst_shared;

    ::ULIS::FMat3F fixedTransform( ::ULIS::FMat3F::MakeTranslationMatrix( static_cast< float >( -box.x ), static_cast< float >( -box.y ) ) * mat );
    FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( src.Format() );
    ::ULIS::FEvent eventClear;
    ctx.Clear( dst, FRectI::Auto, FSchedulePolicy::CacheEfficient, 1, &Sample.GetEvent(), &eventClear );
    ::ULIS::FEvent eventTransform;
    ctx.TransformAffine(
          src
        , dst
        , FRectI::Auto
        , fixedTransform
        , eResamplingMethod( ResamplingMethod )
        , eBorderMode::Border_Transparent
        , ::ULIS::FColor::Transparent
        , FSchedulePolicy::MultiScanlines
        , 1
        , &eventClear
        , &eventTransform
    );
    ctx.Flush();
    return FOdysseyBlockProxy::MakeProxy( dst_shared, 1, &eventTransform, 1, &Sample);
}


//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::Shear( FOdysseyBlockProxy Sample, float ShearX, float ShearY, EResamplingMethod ResamplingMethod )
{
    if( !Sample.IsValid() )
        return FOdysseyBlockProxy::MakeNullProxy();

    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > src_shared = Sample.GetBlock();
    using namespace ::ULIS;
    ::ULIS::FBlock& src = *src_shared;

    ::ULIS::FMat3F mat( ::ULIS::FMat3F::MakeSkewMatrix( ShearX, ShearY ) );
    ::ULIS::FRectI box = ::ULIS::FContext::TransformAffineMetrics( src.Rect(), mat );
    if( box.Area() <= 0 )
        return FOdysseyBlockProxy::MakeNullProxy();

    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > dst_shared = MakeShareable(new ::ULIS::FBlock( box.w, box.h, src.Format() ));
    ::ULIS::FBlock& dst = *dst_shared;

    ::ULIS::FMat3F fixedTransform( ::ULIS::FMat3F::MakeTranslationMatrix( static_cast< float >( -box.x ), static_cast< float >( -box.y ) ) * mat );
    FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( src.Format() );
    ::ULIS::FEvent eventClear;
    ctx.Clear( dst, FRectI::Auto, FSchedulePolicy::CacheEfficient, 1, &Sample.GetEvent(), &eventClear );
    ::ULIS::FEvent eventTransform;
    ctx.TransformAffine(
          src
        , dst
        , FRectI::Auto
        , fixedTransform
        , eResamplingMethod( ResamplingMethod )
        , eBorderMode::Border_Transparent
        , ::ULIS::FColor::Transparent
        , FSchedulePolicy::MultiScanlines
        , 1
        , &eventClear
        , &eventTransform
    );
    ctx.Flush();
    return FOdysseyBlockProxy::MakeProxy( dst_shared, 1, &eventTransform, 1, &Sample);
}


//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::ResizeUniform( FOdysseyBlockProxy Sample, float Size, EResamplingMethod ResamplingMethod )
{
    if( !Sample.IsValid() )
        return FOdysseyBlockProxy::MakeNullProxy();

    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > src_shared = Sample.GetBlock();
    using namespace ::ULIS;
    ::ULIS::FBlock& src = *src_shared;

    int src_width  = src.Width();
    int src_height = src.Height();
    float max = ::ULIS::FMath::Max( src_width, src_height );
    float ratio = Size / max;

    ::ULIS::FMat3F fixedTransform(
              ::ULIS::FMat3F::MakeScaleMatrix( ratio, ratio )
            * ::ULIS::FMat3F::MakeTranslationMatrix( static_cast< float >( -src_width / 2.f ), static_cast< float >( -src_height / 2.f ) )
    );

    ::ULIS::FRectI box = ::ULIS::FContext::TransformAffineMetrics( src.Rect(), fixedTransform );

    if( box.Area() <= 0 )
        return FOdysseyBlockProxy::MakeNullProxy();

    fixedTransform = ::ULIS::FMat3F::MakeTranslationMatrix( static_cast< float >( -box.x ), static_cast< float >( -box.y ) ) * fixedTransform;

    box = ::ULIS::FContext::TransformAffineMetrics(src.Rect(), fixedTransform );

    if (box.Area() <= 0)
        return FOdysseyBlockProxy::MakeNullProxy();

    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > dst_shared = MakeShareable(new ::ULIS::FBlock( box.w, box.h, src.Format() ));
    ::ULIS::FBlock& dst = *dst_shared;

    FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( src.Format() );
    ::ULIS::FEvent eventClear;
    ctx.Clear( dst, FRectI::Auto, FSchedulePolicy::CacheEfficient, 1, &Sample.GetEvent(), &eventClear );
    ::ULIS::FEvent eventTransform;
    ctx.TransformAffine(
          src
        , dst
        , FRectI::Auto
        , fixedTransform
        , eResamplingMethod( ResamplingMethod )
        , eBorderMode::Border_Transparent
        , ::ULIS::FColor::Transparent
        , FSchedulePolicy::MultiScanlines
        , 1
        , &eventClear
        , &eventTransform
    );
    ctx.Flush();
    return FOdysseyBlockProxy::MakeProxy( dst_shared, 1, &eventTransform, 1, &Sample);
}


//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::Resize( FOdysseyBlockProxy Sample, float SizeX, float SizeY, EResamplingMethod ResamplingMethod )
{
    if( !Sample.IsValid() )
        return FOdysseyBlockProxy::MakeNullProxy();

    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > src_shared = Sample.GetBlock();
    using namespace ::ULIS;
    ::ULIS::FBlock& src = *src_shared;

    float src_width  = src.Width();
    float src_height = src.Height();
    float ratioX = SizeX / src_width;
    float ratioY = SizeY / src_height;

    ::ULIS::FMat3F fixedTransform(
              ::ULIS::FMat3F::MakeScaleMatrix( ratioX, ratioY )
            * ::ULIS::FMat3F::MakeTranslationMatrix( static_cast< float >( -src_width / 2.f ), static_cast< float >( -src_height / 2.f ) )
    );

    ::ULIS::FRectI box = ::ULIS::FContext::TransformAffineMetrics( src.Rect(), fixedTransform );
    if( box.Area() <= 0 )
        return FOdysseyBlockProxy::MakeNullProxy();

    fixedTransform = ::ULIS::FMat3F::MakeTranslationMatrix( static_cast< float >( -box.x ), static_cast< float >( -box.y ) ) * fixedTransform;

    box = ::ULIS::FContext::TransformAffineMetrics(src.Rect(), fixedTransform );

    if (box.Area() <= 0)
        return FOdysseyBlockProxy::MakeNullProxy();

    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > dst_shared = MakeShareable(new ::ULIS::FBlock( box.w, box.h, src.Format() ));
    ::ULIS::FBlock& dst = *dst_shared;

    FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( src.Format() );
    ::ULIS::FEvent eventClear;
    ctx.Clear( dst, FRectI::Auto, FSchedulePolicy::CacheEfficient, 1, &Sample.GetEvent(), &eventClear );
    ::ULIS::FEvent eventTransform;
    ctx.TransformAffine(
          src
        , dst
        , FRectI::Auto
        , fixedTransform
        , eResamplingMethod( ResamplingMethod )
        , eBorderMode::Border_Transparent
        , ::ULIS::FColor::Transparent
        , FSchedulePolicy::MultiScanlines
        , 1
        , &eventClear
        , &eventTransform
    );
    ctx.Flush();
    return FOdysseyBlockProxy::MakeProxy( dst_shared, 1, &eventTransform, 1, &Sample);
}


//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::FlipX( FOdysseyBlockProxy Sample, EResamplingMethod ResamplingMethod )
{
    return  ScaleXY( Sample, -1, 1, ResamplingMethod );
}


//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::FlipY( FOdysseyBlockProxy Sample, EResamplingMethod ResamplingMethod )
{
    return  ScaleXY( Sample, 1, -1, ResamplingMethod );
}


//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::FlipXY( FOdysseyBlockProxy Sample, EResamplingMethod ResamplingMethod )
{
    return  ScaleXY( Sample, -1, -1, ResamplingMethod );
}

//static
FOdysseyBlockProxy
UOdysseyTransformProxyLibrary::Perspective( FOdysseyBlockProxy Sample, FOdysseyMatrix PerspectiveMatrix, int OutputWidth, int OutputHeight, EResamplingMethod ResamplingMethod )
{
    if( !Sample.IsValid() )
        return FOdysseyBlockProxy::MakeNullProxy();

    if( OutputWidth <= 0 || OutputHeight <= 0 )
        return FOdysseyBlockProxy::MakeNullProxy();

    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > src_shared = Sample.GetBlock();
    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > dst_shared = MakeShareable(new ::ULIS::FBlock( OutputWidth, OutputHeight, src_shared->Format() ));
    ::ULIS::FEvent eventClear;
    ::ULIS::FEvent eventTransform;
    {
        using namespace ::ULIS;
        ::ULIS::FBlock& src = *src_shared;
        ::ULIS::FBlock& dst = *dst_shared;
        FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( src.Format() );
        ctx.Clear( dst, FRectI::Auto, FSchedulePolicy::CacheEfficient, 1, &Sample.GetEvent(), &eventClear );
        ctx.TransformPerspective(
              src
            , dst
            , FRectI::Auto
            , PerspectiveMatrix.m
            , eResamplingMethod( ResamplingMethod )
            , eBorderMode::Border_Transparent
            , ::ULIS::FColor::Transparent
            , FSchedulePolicy::MultiScanlines
            , 1
            , &eventClear
            , &eventTransform
        );
        ctx.Flush();
    }
    return FOdysseyBlockProxy::MakeProxy(dst_shared, 1, &eventTransform, 1, &Sample);
}

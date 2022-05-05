// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Proxies/OdysseyBrushBlock.h"

#include "Engine/Font.h"
#include "UObject/StrongObjectPtr.h"

#include "OdysseySurfaceTexture2DEditable.h"
#include "OdysseyBrushAssetBase.h"
#include <ULIS>
#include "ULISLoaderModule.h"

class FOdysseyBlockProxy_Internal : public TSharedFromThis<FOdysseyBlockProxy_Internal, ESPMode::ThreadSafe>
{

    class FBlockRetainer
    {
    public:
        FBlockRetainer(const TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>& iBlock, int iNumDeps, FOdysseyBlockProxy* iDependencies)
            : mBlock(iBlock)
            , mDeps()
        {
            for (int i = 0; i < iNumDeps; i++)
            {
                mDeps.Add(iDependencies[i].GetBlock());
            }
        }
    private:

        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>   mBlock;
        TArray<TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>> mDeps;
    };

public:
    FOdysseyBlockProxy_Internal()
        : mBlock(nullptr)
    {
    }

public:
    void Init( const TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>& iBlock, int iNumEvents, ::ULIS::FEvent* iEvents, int iNumDeps, FOdysseyBlockProxy* iDependencies )
    {
        mBlock = iBlock;

        FBlockRetainer* retainer = new FBlockRetainer(iBlock, iNumDeps, iDependencies);
        mEvent = ::ULIS::FEvent(
            ::ULIS::FOnEventComplete(
                [retainer](const ::ULIS::FRectI& iRect)
                {
                    delete retainer;
                }
            )
        );

        //dummy context
        if( iNumEvents == 0 )
        {
            ::ULIS::FContext::MarkEventFinished( &mEvent );
        }
        else
        {
            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::eFormat::Format_RGBA8);
            ctx.Dummy_OP(iNumEvents, iEvents, &mEvent);
            ctx.Flush();
        }
    }


public:
    //Returns wether this FOdysseyBlockProxy is Valid or NULL
    //Always check if a FOdysseyBlockProxy is valid before using it
    bool
    IsValid()
    {
        return mBlock != nullptr;
    }

    //Returns the block the proxy is holding
    const TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>&
    GetBlock()
    {
        return mBlock;
    }

    //Returns the Event on which to wait for the block to be "done"
    const ::ULIS::FEvent&
    GetEvent()
    {
        return mEvent;
    }

private:
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>   mBlock;
    ::ULIS::FEvent  mEvent;
};


FOdysseyBlockProxy::~FOdysseyBlockProxy()
{
}

FOdysseyBlockProxy::FOdysseyBlockProxy()
    : m(nullptr)
{
}

FOdysseyBlockProxy::FOdysseyBlockProxy( const TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>& iBlock, int iNumEvents, ::ULIS::FEvent* iEvents, int iNumDeps, FOdysseyBlockProxy* iDependencies )
    : m(MakeShareable(new FOdysseyBlockProxy_Internal()))
{
    m->Init(iBlock, iNumEvents, iEvents, iNumDeps, iDependencies);
}

FOdysseyBlockProxy
FOdysseyBlockProxy::MakeNullProxy()
{
    return  FOdysseyBlockProxy();
}

FOdysseyBlockProxy FOdysseyBlockProxy::MakeProxy(const TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>& iBlock, int iNumEvents, ::ULIS::FEvent* iEvents, int iNumDeps, FOdysseyBlockProxy* iDependencies)
{
    return  FOdysseyBlockProxy(iBlock, iNumEvents, iEvents, iNumDeps, iDependencies);
}

//Returns wether this FOdysseyBlockProxy is Valid or NULL
//Always check if a FOdysseyBlockProxy is valid before using it
bool
FOdysseyBlockProxy::IsValid()
{
    return m && m->IsValid();
}

//Returns the block the proxy is holding
const TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>&
FOdysseyBlockProxy::GetBlock()
{
    return m->GetBlock();
}

//Returns the Event on which to wait for the block to be "done"
const ::ULIS::FEvent&
FOdysseyBlockProxy::GetEvent()
{
    return m->GetEvent();
}

//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::Conv_TextureToOdysseyBlockProxy( UTexture2D* Texture, EOdysseyColorModel ColorModel, EOdysseyChannelDepth ChannelDepth )
{
    if( !Texture )
        return  FOdysseyBlockProxy::MakeNullProxy();

    ::ULIS::eFormat format = ULISFormatFromModelAndDepth( ColorModel, ChannelDepth );
    TSharedRef< ::ULIS::FBlock, ESPMode::ThreadSafe > dst = MakeShareable( NewBlockFromUTextureData( Texture, format ) );
    return  FOdysseyBlockProxy::MakeProxy( dst );
}

//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::ConvertToFormat(FOdysseyBlockProxy Block, EOdysseyColorModel ColorModel, EOdysseyChannelDepth ChannelDepth)
{
    if( !Block.IsValid() )
        return  FOdysseyBlockProxy::MakeNullProxy();

    ::ULIS::eFormat format = ULISFormatFromModelAndDepth( ColorModel, ChannelDepth );

    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > block = Block.GetBlock();
    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > conv = MakeShareable( new ::ULIS::FBlock(block->Width(), block->Height(), format ));
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( format );

    ::ULIS::FEvent eventConvert;
    ctx.ConvertFormat( *block, *conv, ::ULIS::FRectI::Auto, ::ULIS::FVec2I(0), ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 1, &Block.GetEvent(), &eventConvert); // Auto fallback to copy if same format
    ctx.Flush();

    return  FOdysseyBlockProxy::MakeProxy( conv, 1, &eventConvert, 1, &Block);
}

//static
EOdysseyColorModel
UOdysseyBlockProxyFunctionLibrary::GetColorModel(FOdysseyBlockProxy Block)
{
    if( !Block.IsValid() )
        return  EOdysseyColorModel::kRGBA;

    return  OdysseyColorModelFromULISFormat(Block.GetBlock()->Format());
}

//static
EOdysseyChannelDepth
UOdysseyBlockProxyFunctionLibrary::GetChannelDepth(FOdysseyBlockProxy Block)
{
    if( !Block.IsValid())
        return  EOdysseyChannelDepth::k8;

    return  OdysseyChannelDepthFromULISFormat(Block.GetBlock()->Format());
}

//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::FillPreserveAlpha( FOdysseyBlockProxy Source, FOdysseyBrushColor Color )
{
    if( !Source.IsValid())
        return  FOdysseyBlockProxy::MakeNullProxy();

    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > src = Source.GetBlock();
    ::ULIS::eFormat format = src->Format();
    ::ULIS::FColor color = Color.GetValue().ToFormat( format );
    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > dst = MakeShareable( new ::ULIS::FBlock( src->Width(), src->Height(), format ));
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( format );
    
    ::ULIS::FEvent eventCopy;
    ctx.Copy( *src, *dst, ::ULIS::FRectI::Auto, ::ULIS::FVec2I( 0 ), ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 1, &Source.GetEvent(), &eventCopy );
    
    ::ULIS::FEvent eventFill;
    ctx.FillPreserveAlpha( *dst, color, ::ULIS::FRectI::Auto, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 1, &eventCopy, &eventFill);
    ctx.Flush();

    return  FOdysseyBlockProxy::MakeProxy(dst, 1, &eventFill, 1, &Source);
}

//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::Fill(
      FOdysseyBlockProxy Block
    , FOdysseyBrushColor Color
    , FOdysseyBrushRect Area
    , bool PreserveAlpha
    , EOdysseyColorModel ColorModel
    , EOdysseyChannelDepth ChannelDepth
)
{
    if (!Block.IsValid())
        return  FOdysseyBlockProxy::MakeNullProxy();

    ::ULIS::eFormat format = ULISFormatFromModelAndDepth(ColorModel, ChannelDepth);

    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > block = Block.GetBlock();
    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > dst = MakeShareable( new ::ULIS::FBlock(block->Width(), block->Height(), format ));
    ::ULIS::FColor color = Color.GetValue().ToFormat( format );
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( format );
    ::ULIS::FEvent eventConvert;
    if( Area.IsInitialized() || PreserveAlpha) {
        ctx.ConvertFormat(
            *block,
            *dst,
            ::ULIS::FRectI::Auto, ::ULIS::FVec2I(0),
            ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
            1,   
            &Block.GetEvent(),
            &eventConvert
        );
    }
    else
    {
        ctx.Dummy_OP(1, &Block.GetEvent(), &eventConvert);
    }

    ::ULIS::FEvent eventFill;
    if( !Area.IsInitialized() || Area.Width() > 0 || Area.Height() > 0 )
    {
        if (PreserveAlpha)
        {
            ctx.FillPreserveAlpha(
                *dst,
                color,
                Area.IsInitialized() ? Area.GetValue() : dst->Rect(),
                ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                1,
                &eventConvert,
                &eventFill
            );
        }
        else
        {
            ctx.Fill(
                *dst,
                color, Area.IsInitialized() ? Area.GetValue() : dst->Rect(),
                ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                1,
                &eventConvert,
                &eventFill
            );
        }
    }
    else
    {
        ctx.Dummy_OP(1, &eventConvert, &eventFill);
    }

    ctx.Flush();
    return  FOdysseyBlockProxy::MakeProxy(dst, 1, &eventFill, 1, &Block);
}

//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::BlendColor(
      FOdysseyBrushColor Color
    , FOdysseyBlockProxy Sample
    , FOdysseyBrushRect Area
    , float Opacity
    , EOdysseyColorModel ColorModel
    , EOdysseyChannelDepth ChannelDepth
    , EOdysseyBlendingMode BlendingMode
    , EOdysseyAlphaMode AlphaMode
)
{
    if( !Sample.IsValid() )
        return  FOdysseyBlockProxy::MakeNullProxy();

    ::ULIS::eFormat format = ULISFormatFromModelAndDepth(ColorModel, ChannelDepth);
    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > block = Sample.GetBlock();
    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > dst = MakeShareable( new ::ULIS::FBlock (block->Width(), block->Height(), format ));
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( format );
    ::ULIS::FEvent eventConvert;
    ctx.ConvertFormat( *block, *dst, ::ULIS::FRectI::Auto, ::ULIS::FVec2I(0), ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 1, &Sample.GetEvent(), &eventConvert);

    if (Area.IsInitialized() && (Area.Width() <= 0 || Area.Height() <= 0))
    {
        ctx.Flush();
        return  FOdysseyBlockProxy::MakeProxy( dst, 1, &eventConvert, 1, &Sample );
    }

    ::ULIS::FEvent eventBlend;
    ctx.BlendColor(
            Color.GetValue()
        , *dst
        , Area.IsInitialized() ? Area.GetValue() : dst->Rect()
        , ::ULIS::eBlendMode( BlendingMode )
        , ::ULIS::eAlphaMode( AlphaMode )
        , Opacity
        , ::ULIS::FSchedulePolicy::AsyncCacheEfficient
        , 1
        , &eventConvert
        , &eventBlend
    );


    ctx.Flush();

    return  FOdysseyBlockProxy::MakeProxy( dst, 1, &eventBlend, 1, &Sample);
}


//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::CreateBlock(
      int Width
    , int Height
    , EOdysseyColorModel ColorModel
    , EOdysseyChannelDepth ChannelDepth
    , bool InitializeData
)
{
    if( Width < 1 || Height < 1 )
        return  FOdysseyBlockProxy::MakeNullProxy();

    ::ULIS::eFormat format = ULISFormatFromModelAndDepth( ColorModel, ChannelDepth );
    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > dst = MakeShareable( new ::ULIS::FBlock( Width, Height, format, nullptr, ::ULIS::FOnInvalidBlock() ));
    if (InitializeData)
    {
        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
        ctx.Clear(*dst);
        ctx.Finish();
    }
    return FOdysseyBlockProxy::MakeProxy(dst);
}

//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::CropBlock(
      FOdysseyBlockProxy Block
    , FOdysseyBrushRect Area
)
{
    if( !Block.IsValid() )
        return  FOdysseyBlockProxy::MakeNullProxy();

    if( Area.IsInitialized() && ( Area.Width() <= 0 || Area.Height() <= 0 ) )
        return  FOdysseyBlockProxy::MakeNullProxy();

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = Block.GetBlock();
    int w = Area.IsInitialized() ? Area.Width() : block->Width();
    int h = Area.IsInitialized() ? Area.Height() : block->Height();
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block->Format() );
    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > dst = MakeShareable( new ::ULIS::FBlock( w, h, block->Format() ));

    ::ULIS::FEvent eventCopy;
    ctx.Copy(
          *block
        , *dst
        , Area.IsInitialized() ? Area.GetValue() : block->Rect()
        , ::ULIS::FVec2I(0)
        , ::ULIS::FSchedulePolicy::AsyncCacheEfficient
        , 1
        , &Block.GetEvent()
        , &eventCopy
    );

    ctx.Flush();
    return  FOdysseyBlockProxy::MakeProxy( dst, 1, &eventCopy, 1, &Block);
}

//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::Blend(
      FOdysseyBlockProxy Top
    , FOdysseyBlockProxy Back
    , FOdysseyBrushRect TopArea
    , int OffsetX
    , int OffsetY
    , float Opacity
    , EOdysseyColorModel ColorModel
    , EOdysseyChannelDepth ChannelDepth
    , EOdysseyBlendingMode BlendingMode
    , EOdysseyAlphaMode AlphaMode
)
{
    //Top and back must be valid
    if( !Top.IsValid() || !Back.IsValid() )
        return  FOdysseyBlockProxy::MakeNullProxy();

    //If TopArea is Initialized, it must contain positive non-zero width and height
    if( TopArea.IsInitialized() && ( TopArea.Width() <= 0 || TopArea.Height() <= 0 ) )
        return  FOdysseyBlockProxy::MakeNullProxy();

    // Retrieve blocks and events for Top and Back
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> back = Back.GetBlock();
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> top = Top.GetBlock();
    ::ULIS::FEvent eventInputs[] = { Top.GetEvent(), Back.GetEvent() };

    //Retrieve ULIS destination format
    ::ULIS::eFormat format = ULISFormatFromModelAndDepth( ColorModel, ChannelDepth );
    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > dst = MakeShareable( new ::ULIS::FBlock(back->Width(), back->Height(), format));
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( format );

    //Initialize the source block as being the top block
    ::ULIS::FBlock* src = top.Get();

    // Pre-process Top ( optional conv, optional area )
    ::ULIS::FEvent eventConvertTop;
    if( src->Format() != format || TopArea.IsInitialized() )
    {
        //If source format and destination format are different, we need to use a temporary block with the right format
        //If TopArea is Initialize we also need to use a temporary block with the right size

        //Create the temporary block with the right size immediately (using TopArea if Initialized)
        //The temporary block becoimes the src
        const int w = TopArea.IsInitialized() ? TopArea.Width() : src->Width();
        const int h = TopArea.IsInitialized() ? TopArea.Height() : src->Height();
        src = new ::ULIS::FBlock( w, h, format );

        // Make a con
        ctx.ConvertFormat(
              *top
            , *src
            , TopArea.IsInitialized() ? TopArea.GetValue() : ::ULIS::FRectI::Auto
            , ::ULIS::FVec2I( 0 )
            , ::ULIS::FSchedulePolicy::AsyncCacheEfficient
            , 2
            , eventInputs
            , &eventConvertTop
        );
    }
    else
    {
        ctx.Dummy_OP( 2, eventInputs, &eventConvertTop);
    }

    // Pre-process Back ( mandatory full rect copy/conv )
    ::ULIS::FEvent eventConvertBack;
    ctx.ConvertFormat(
          *back
        , *dst
        , ::ULIS::FRectI::Auto
        , ::ULIS::FVec2I( 0 )
        , ::ULIS::FSchedulePolicy::AsyncCacheEfficient
        , 1
        , &eventConvertTop
        , &eventConvertBack
    );

    //src destruction callback
    ::ULIS::FEvent eventBlend(
        ::ULIS::FOnEventComplete(
            [top, src](const ::ULIS::FRectI& iRect)
            {
                // cleanup source conv if necessary
                if (src != top.Get())
                    delete src;
            }
        )
    );

    ctx.BlendAA(
          *src
        , *dst
        , src->Rect()
        , ::ULIS::FVec2F( OffsetX, OffsetY )
        , ::ULIS::eBlendMode( BlendingMode )
        , ::ULIS::eAlphaMode( AlphaMode )
        , Opacity
        , ::ULIS::FSchedulePolicy::AsyncCacheEfficient
        , 1
        , &eventConvertBack
        , &eventBlend
    );

    ctx.Flush();

    FOdysseyBlockProxy deps[] = { Top, Back };
    return  FOdysseyBlockProxy::MakeProxy( dst, 1, &eventBlend, 2, deps);
}

//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::AdjustAlpha(
      FOdysseyBlockProxy Block
    , UCurveFloat* Curve
    , bool PreserveNullAlpha
)
{
    // TODO: There is a bug related to accessing channels as 8bit depth in this function.
    if( !Block.IsValid() )
        return  FOdysseyBlockProxy::MakeNullProxy();

    if( !Curve )
    {
        Curve = NewObject<UCurveFloat>();
        Curve->FloatCurve.AddKey(0.0f, 0.0f);
        Curve->FloatCurve.AddKey(1.0f, 1.0f);
    }

    TStrongObjectPtr<UCurveFloat>* curve = new TStrongObjectPtr<UCurveFloat>(Curve);

    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > block = Block.GetBlock();
    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > dst = MakeShareable( new ::ULIS::FBlock(block->Width(), block->Height(), block->Format() ));
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block->Format() );

    ::ULIS::FEvent eventFilter(
        ::ULIS::FOnEventComplete(
            [curve](const ::ULIS::FRectI& iRect)
            {
                AsyncTask(ENamedThreads::GameThread, [curve]() {
                    delete curve;
                });
            }
        )
    );

    // No need to copy before, we can do it during the Filter with memcpy
    ctx.FilterInto(
        [Curve, PreserveNullAlpha ]( const ::ULIS::FBlock& iSrcBlock, const uint8* iSrcPtr, ::ULIS::FBlock& iDstBlock, uint8* iDstPtr )
        {
            ::ULIS::FPixel srcProxy( iSrcPtr, iSrcBlock.Format() );
            ::ULIS::FPixel dstProxy( iDstPtr, iDstBlock.Format() );
            memcpy( iDstPtr, iSrcPtr, srcProxy.BytesPerPixel() );

            if( PreserveNullAlpha && srcProxy.AlphaF() == 0.0f )
                dstProxy.SetAlphaF( 0.0f ); // Warning: this will fail if the format is not 8 bit depth !
            else
                dstProxy.SetAlphaF( Curve->GetFloatValue( srcProxy.AlphaF() ) ); // Warning: this will fail if the format is not 8 bit depth !
        }
        , *block
        , *dst
        , ::ULIS::FRectI::Auto
        , ::ULIS::FVec2I( 0 )
        , ::ULIS::FSchedulePolicy::AsyncCacheEfficient
        , 1
        , &Block.GetEvent()
        , &eventFilter
    );

    ctx.Flush();
    return  FOdysseyBlockProxy::MakeProxy( dst, 1, &eventFilter, 1, &Block);
}

#define ADJUST(format, filterFunc, adjustEvent)                                                                                                     \
    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > block = Block.GetBlock();                                                                      \
    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > dst = MakeShareable( new ::ULIS::FBlock(block->Width(), block->Height(), block->Format() ));    \
                                                                                                                        \
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block->Format() );                                \
                                                                                                                        \
    ::ULIS::FBlock* src = block.Get();                                                                                  \
    ::ULIS::FBlock* filterDst = dst.Get();                                                                              \
                                                                                                                        \
    EOdysseyChannelDepth channelDepth = OdysseyChannelDepthFromULISFormat( src->Format() );                             \
                                                                                                                        \
    ::ULIS::FEvent eventConvertSrc;                                                                                     \
    if( src->Format() != format )                                                                                       \
    {                                                                                                                   \
        src = new ::ULIS::FBlock(block->Width(), block->Height(), format );                                             \
        ctx.ConvertFormat(                                                                                              \
            *block,                                                                                                     \
            *src,                                                                                                       \
            ::ULIS::FRectI::Auto,                                                                                       \
            ::ULIS::FVec2I(0),                                                                                          \
            ::ULIS::FSchedulePolicy::AsyncCacheEfficient,                                                               \
            1,                                                                                                          \
            &Block.GetEvent(),                                                                                          \
            &eventConvertSrc);                                                                                          \
        filterDst = src;                                                                                                \
    }                                                                                                                   \
    else                                                                                                                \
    {                                                                                                                   \
        ctx.Dummy_OP(1, &Block.GetEvent(), &eventConvertSrc);                                                           \
    }                                                                                                                   \
                                                                                                                        \
    ::ULIS::FEvent eventFilter;                                                                                         \
    ctx.FilterInto(                                                                                                     \
        filterFunc                                                                                                      \
        , *block                                                                                                        \
        , *dst                                                                                                          \
        , ::ULIS::FRectI::Auto                                                                                          \
        , ::ULIS::FVec2I( 0 )                                                                                           \
        , ::ULIS::FSchedulePolicy::AsyncCacheEfficient                                                                  \
        , 1                                                                                                             \
        , &eventConvertSrc                                                                                              \
        , &eventFilter                                                                                                  \
    );                                                                                                                  \
                                                                                                                        \
    ::ULIS::FEvent eventConvertDst(                                                                                     \
        ::ULIS::FOnEventComplete(                                                                                       \
        [src, block](const ::ULIS::FRectI& iRect)                                                                       \
            {                                                                                                           \
                if (src != block.Get())                                                                                       \
                    delete  src;                                                                                        \
            }                                                                                                           \
        )                                                                                                               \
    );                                                                                                                  \
    if( filterDst != dst.Get() )                                                                                              \
    {                                                                                                                   \
        ctx.ConvertFormat(                                                                                              \
            *filterDst,                                                                                                 \
            *dst,                                                                                                       \
            ::ULIS::FRectI::Auto,                                                                                       \
            ::ULIS::FVec2I(0),                                                                                          \
            ::ULIS::FSchedulePolicy::AsyncCacheEfficient,                                                               \
            1,                                                                                                          \
            &eventFilter,                                                                                               \
            &adjustEvent);                                                                                              \
    }                                                                                                                   \
    else                                                                                                                \
    {                                                                                                                   \
        ctx.Dummy_OP(1, &eventFilter, &adjustEvent);                                                                    \
    }                                                                                                                   \
                                                                                                                        \
    ctx.Flush();                                                                                                        \
    return  FOdysseyBlockProxy::MakeProxy( dst, 1, &adjustEvent, 1, &Block);

#define ADJUSTEVENT(ObjectType, ...)                                                                    \
    TArray<TStrongObjectPtr<ObjectType>>* arrayPtr = new TArray<TStrongObjectPtr<ObjectType>>();        \
    for (auto obj : { __VA_ARGS__ })                                                                    \
    {                                                                                                   \
        arrayPtr->Emplace(obj);                                                                         \
    }                                                                                                   \
    ::ULIS::FEvent adjustEvent(                                                                         \
        ::ULIS::FOnEventComplete(                                                                       \
            [arrayPtr](const ::ULIS::FRectI& iRect)                                                     \
            {                                                                                           \
                AsyncTask(ENamedThreads::GameThread, [arrayPtr]() {                                     \
                    delete arrayPtr;                                                                    \
                });                                                                                     \
            }                                                                                           \
        )                                                                                               \
    );

//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::AdjustRGBA(
      FOdysseyBlockProxy Block
    , UCurveFloat* CurveR
    , UCurveFloat* CurveG
    , UCurveFloat* CurveB
    , UCurveFloat* CurveAlpha
    , bool PreserveNullAlpha
)
{
    if( !Block.IsValid() )
        return  FOdysseyBlockProxy::MakeNullProxy();

    TStrongObjectPtr< UCurveFloat > defaultCurve( NewObject< UCurveFloat >() ); //Retains the pointer
    defaultCurve->FloatCurve.AddKey(0.0f, 0.0f);
    defaultCurve->FloatCurve.AddKey(1.0f, 1.0f);

    if(!CurveR) CurveR = defaultCurve.Get();
    if(!CurveG) CurveG = defaultCurve.Get();
    if(!CurveB) CurveB = defaultCurve.Get();
    if(!CurveAlpha) CurveAlpha = defaultCurve.Get();

    auto filterFunc = [CurveR, CurveG, CurveB, CurveAlpha, PreserveNullAlpha]( const ::ULIS::FBlock& iSrcBlock, const uint8* iSrcPtr, ::ULIS::FBlock& iDstBlock, uint8* iDstPtr ) {
        ::ULIS::FPixel srcProxy( iSrcPtr, iSrcBlock.Format() );
        ::ULIS::FPixel dstProxy( iDstPtr, iDstBlock.Format() );
        dstProxy.SetRF( CurveR->GetFloatValue( srcProxy.RF() ) );
        dstProxy.SetGF( CurveG->GetFloatValue( srcProxy.GF() ) );
        dstProxy.SetBF( CurveB->GetFloatValue( srcProxy.BF() ) );
        if( PreserveNullAlpha && srcProxy.AlphaF() == 0.0f )
            dstProxy.SetAlphaF( 0.0f );
        else
            dstProxy.SetAlphaF( CurveAlpha->GetFloatValue( srcProxy.AlphaF() ) );
    };

    ADJUSTEVENT(UCurveFloat, CurveR, CurveG, CurveB, CurveAlpha);
    ADJUST(::ULIS::Format_RGBAF, filterFunc, adjustEvent)
}


//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::AdjustGreyA(
      FOdysseyBlockProxy Block
    , UCurveFloat* CurveGrey
    , UCurveFloat* CurveAlpha
    , bool PreserveNullAlpha
)
{
    if( !Block.IsValid() )
        return  FOdysseyBlockProxy::MakeNullProxy();

    TStrongObjectPtr<UCurveFloat> defaultCurve(NewObject<UCurveFloat>()); //Retains the pointer
    defaultCurve->FloatCurve.AddKey(0.0f, 0.0f);
    defaultCurve->FloatCurve.AddKey(1.0f, 1.0f);

    if(!CurveGrey) CurveGrey = defaultCurve.Get();
    if(!CurveAlpha) CurveAlpha = defaultCurve.Get();

    auto filterFUnc = [CurveGrey, CurveAlpha, PreserveNullAlpha]( const ::ULIS::FBlock& iSrcBlock, const uint8* iSrcPtr, ::ULIS::FBlock& iDstBlock, uint8* iDstPtr ) {
        ::ULIS::FPixel srcProxy( iSrcPtr, iSrcBlock.Format() );
        ::ULIS::FPixel dstProxy( iDstPtr, iDstBlock.Format() );
        dstProxy.SetGreyF( CurveGrey->GetFloatValue(srcProxy.GreyF() ) );
        if( PreserveNullAlpha && srcProxy.AlphaF() == 0.0f )
            dstProxy.SetAlphaF( 0.0f );
        else
            dstProxy.SetAlphaF( CurveAlpha->GetFloatValue( srcProxy.AlphaF() ) );
    };

    ADJUSTEVENT(UCurveFloat, CurveGrey, CurveAlpha);
    ADJUST(::ULIS::Format_GAF, filterFUnc, adjustEvent)
}


//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::AdjustHSVA(
      FOdysseyBlockProxy Block
    , UCurveFloat* CurveH
    , UCurveFloat* CurveS
    , UCurveFloat* CurveV
    , UCurveFloat* CurveAlpha
    , bool PreserveNullAlpha
)
{
    if( !Block.IsValid() )
        return  FOdysseyBlockProxy::MakeNullProxy();

    TStrongObjectPtr<UCurveFloat> defaultCurve(NewObject<UCurveFloat>()); //Retains the pointer
    defaultCurve->FloatCurve.AddKey(0.0f, 0.0f);
    defaultCurve->FloatCurve.AddKey(1.0f, 1.0f);

    if(!CurveH) CurveH = defaultCurve.Get();
    if(!CurveS) CurveS = defaultCurve.Get();
    if(!CurveV) CurveV = defaultCurve.Get();
    if(!CurveAlpha) CurveAlpha = defaultCurve.Get();
    
    auto filterFunc = [CurveH, CurveS, CurveV, CurveAlpha, PreserveNullAlpha]( const ::ULIS::FBlock& iSrcBlock, const uint8* iSrcPtr, ::ULIS::FBlock& iDstBlock, uint8* iDstPtr ) {
        ::ULIS::FPixel srcProxy(iSrcPtr, iSrcBlock.Format());
        ::ULIS::FPixel dstProxy(iDstPtr, iDstBlock.Format());

        dstProxy.SetHueF(CurveH->GetFloatValue(srcProxy.HueF()));
        dstProxy.SetSaturationF(CurveS->GetFloatValue(srcProxy.SaturationF()));
        dstProxy.SetValueF(CurveV->GetFloatValue(srcProxy.ValueF()));

        if(PreserveNullAlpha && srcProxy.AlphaF() == 0.0f)
            dstProxy.SetAlphaF(0.0f);
        else
            dstProxy.SetAlphaF(CurveAlpha->GetFloatValue(srcProxy.AlphaF()));
    };

    ADJUSTEVENT(UCurveFloat, CurveH, CurveS, CurveV, CurveAlpha);
    ADJUST(::ULIS::Format_HSVAF, filterFunc, adjustEvent)
}


//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::AdjustHSLA(
      FOdysseyBlockProxy Block
    , UCurveFloat* CurveH
    , UCurveFloat* CurveS
    , UCurveFloat* CurveL
    , UCurveFloat* CurveAlpha
    , bool PreserveNullAlpha
)
{
    if( !Block.IsValid() )
        return  FOdysseyBlockProxy::MakeNullProxy();

    TStrongObjectPtr<UCurveFloat> defaultCurve(NewObject<UCurveFloat>()); //Retains the pointer
    defaultCurve->FloatCurve.AddKey(0.0f, 0.0f);
    defaultCurve->FloatCurve.AddKey(1.0f, 1.0f);

    if(!CurveH) CurveH = defaultCurve.Get();
    if(!CurveS) CurveS = defaultCurve.Get();
    if(!CurveL) CurveL = defaultCurve.Get();
    if(!CurveAlpha) CurveAlpha = defaultCurve.Get();
    
    auto filterFunc = [CurveH, CurveS, CurveL, CurveAlpha, PreserveNullAlpha]( const ::ULIS::FBlock& iSrcBlock, const uint8* iSrcPtr, ::ULIS::FBlock& iDstBlock, uint8* iDstPtr ) {
        ::ULIS::FPixel srcProxy(iSrcPtr, iSrcBlock.Format());
        ::ULIS::FPixel dstProxy(iDstPtr, iDstBlock.Format());

        dstProxy.SetHueF(CurveH->GetFloatValue(srcProxy.HueF()));
        dstProxy.SetSaturationF(CurveS->GetFloatValue(srcProxy.SaturationF()));
        dstProxy.SetLightnessF(CurveL->GetFloatValue(srcProxy.LightnessF()));

        if(PreserveNullAlpha && srcProxy.AlphaF() == 0.0f)
            dstProxy.SetAlphaF(0.0f);
        else
            dstProxy.SetAlphaF(CurveAlpha->GetFloatValue(srcProxy.AlphaF()));
    };

    ADJUSTEVENT(UCurveFloat, CurveH, CurveS, CurveL, CurveAlpha);
    ADJUST(::ULIS::Format_HSLAF, filterFunc, adjustEvent)
}


//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::AdjustCMYKA(
      FOdysseyBlockProxy Block
    , UCurveFloat* CurveC
    , UCurveFloat* CurveM
    , UCurveFloat* CurveY
    , UCurveFloat* CurveK
    , UCurveFloat* CurveAlpha
    , bool PreserveNullAlpha
)
{
    if( !Block.IsValid() )
        return  FOdysseyBlockProxy::MakeNullProxy();

    TStrongObjectPtr<UCurveFloat> defaultCurve(NewObject<UCurveFloat>()); //Retains the pointer
    defaultCurve->FloatCurve.AddKey(0.0f, 0.0f);
    defaultCurve->FloatCurve.AddKey(1.0f, 1.0f);

    if(!CurveC) CurveC = defaultCurve.Get();
    if(!CurveM) CurveM = defaultCurve.Get();
    if(!CurveY) CurveY = defaultCurve.Get();
    if(!CurveK) CurveK = defaultCurve.Get();
    if(!CurveAlpha) CurveAlpha = defaultCurve.Get();

    auto filterFunc = [CurveC, CurveM, CurveY, CurveK, CurveAlpha, PreserveNullAlpha]( const ::ULIS::FBlock& iSrcBlock, const uint8* iSrcPtr, ::ULIS::FBlock& iDstBlock, uint8* iDstPtr ) {
        ::ULIS::FPixel srcProxy(iSrcPtr, iSrcBlock.Format());
        ::ULIS::FPixel dstProxy(iDstPtr, iDstBlock.Format());

        dstProxy.SetCyanF(CurveC->GetFloatValue(srcProxy.CyanF()));
        dstProxy.SetMagentaF(CurveM->GetFloatValue(srcProxy.MagentaF()));
        dstProxy.SetYellowF(CurveY->GetFloatValue(srcProxy.YellowF()));
        dstProxy.SetKeyF(CurveK->GetFloatValue(srcProxy.KeyF()));

        if(PreserveNullAlpha && srcProxy.AlphaF() == 0.0f)
            dstProxy.SetAlphaF(0.0f);
        else
            dstProxy.SetAlphaF(CurveAlpha->GetFloatValue(srcProxy.AlphaF()));
    };

    ADJUSTEVENT(UCurveFloat, CurveC, CurveM, CurveY, CurveK, CurveAlpha);
    ADJUST(::ULIS::Format_CMYKAF, filterFunc, adjustEvent)
}


//static
FOdysseyBlockProxy
UOdysseyBlockProxyFunctionLibrary::AdjustLabA(
      FOdysseyBlockProxy Block
    , UCurveFloat* CurveL
    , UCurveFloat* CurveA
    , UCurveFloat* CurveB
    , UCurveFloat* CurveAlpha
    , bool PreserveNullAlpha
)
{
    if( !Block.IsValid() )
        return  FOdysseyBlockProxy::MakeNullProxy();

    TStrongObjectPtr<UCurveFloat> defaultCurve(NewObject<UCurveFloat>()); //Retains the pointer
    defaultCurve->FloatCurve.AddKey(0.0f, 0.0f);
    defaultCurve->FloatCurve.AddKey(1.0f, 1.0f);

    if(!CurveL) CurveL = defaultCurve.Get();
    if(!CurveA) CurveA = defaultCurve.Get();
    if(!CurveB) CurveB = defaultCurve.Get();
    if(!CurveAlpha) CurveAlpha = defaultCurve.Get();
    
    auto filterFunc = [CurveL, CurveA, CurveB, CurveAlpha, PreserveNullAlpha]( const ::ULIS::FBlock& iSrcBlock, const uint8* iSrcPtr, ::ULIS::FBlock& iDstBlock, uint8* iDstPtr ) {
        ::ULIS::FPixel srcProxy(iSrcPtr, iSrcBlock.Format());
        ::ULIS::FPixel dstProxy(iDstPtr, iDstBlock.Format());

        dstProxy.SetLF(CurveL->GetFloatValue(srcProxy.LF()));
        dstProxy.SetaF(CurveA->GetFloatValue(srcProxy.aF()));
        dstProxy.SetbF(CurveB->GetFloatValue(srcProxy.bF()));

        if(PreserveNullAlpha && srcProxy.AlphaF() == 0.0f)
            dstProxy.SetAlphaF(0.0f);
        else
            dstProxy.SetAlphaF(CurveAlpha->GetFloatValue(srcProxy.AlphaF()));
    };


    ADJUSTEVENT(UCurveFloat, CurveL, CurveA, CurveB, CurveAlpha);
    ADJUST(::ULIS::Format_LabAF, filterFunc, adjustEvent)
}

//static
int
UOdysseyBlockProxyFunctionLibrary::GetWidth( FOdysseyBlockProxy Sample )
{
    if( !Sample.IsValid() )
        return 0;

    return Sample.GetBlock()->Width();
}


//static
int
UOdysseyBlockProxyFunctionLibrary::GetHeight( FOdysseyBlockProxy Sample )
{
    if( !Sample.IsValid() )
        return 0;

    return Sample.GetBlock()->Height();
}

//static
TArray< FOdysseyBlockProxy >
UOdysseyBlockProxyFunctionLibrary::GetFontBlocks( const UFont* iFont, EOdysseyColorModel ColorModel, EOdysseyChannelDepth ChannelDepth)
{
    TArray< FOdysseyBlockProxy > blocks;
    if( iFont->FontCacheType == EFontCacheType::Runtime )
        return blocks;
    
    check( iFont->Textures.Num() )

    ::ULIS::eFormat format = ULISFormatFromModelAndDepth( ColorModel, ChannelDepth );

    for( auto texture : iFont->Textures )
    {
        TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > block = MakeShareable(NewBlockFromUTextureData( texture, format));
        blocks.Add( FOdysseyBlockProxy::MakeProxy( block ) );
    }
    
    return blocks;
}

//static
TArray< FOdysseyFontCharacter >
UOdysseyBlockProxyFunctionLibrary::GetFontCharacterInfo( const UFont* iFont, const FString& iString )
{
    TArray< FOdysseyFontCharacter > font_characters;
    if( iFont->FontCacheType == EFontCacheType::Runtime )
        return  font_characters;
    
    check( iFont->Characters.Num() )
    
    for( auto character : iString )
    {
        TCHAR index_of_character = iFont->RemapChar( character );
    
        const FFontCharacter& font_char = iFont->Characters[index_of_character];

        FOdysseyFontCharacter font_character;
        font_character.StartU = font_char.StartU;
        font_character.StartV = font_char.StartV;
        font_character.USize = font_char.USize;
        font_character.VSize = font_char.VSize;
        font_character.TextureIndex = font_char.TextureIndex;
        font_character.VerticalOffset = font_char.VerticalOffset;

        font_characters.Add( font_character );
    }
    
    return  font_characters;
}

//static
void
UOdysseyBlockProxyFunctionLibrary::GetCharactersSize( const UFont* iFont, const FString& iString, TArray<float>& oWidth, TArray<float>& oHeight )
{
    oWidth.Empty();
    oHeight.Empty();

    for( auto character : iString )
    {
        float width;
        float height;
        iFont->GetCharSize( character, width, height );
        oWidth.Add( width );
        oHeight.Add( height );
    }
}

//static
bool
UOdysseyBlockProxyFunctionLibrary::GetColorAtPosition( FOdysseyBlockProxy iBlock, float iX, float iY, FOdysseyBrushColor& oColor )
{
    if (!iBlock.IsValid())
        return false;
    
    int x = FMath::FloorToInt( iX );
    int y = FMath::FloorToInt( iY );

    //Wait for any operation on the block to be done before reading it
    iBlock.GetEvent().Wait();
    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > block = iBlock.GetBlock();
    if( !block )
        return  false;

    if( x < 0 || x >= block->Width() )
        return  false;
    if( y < 0 || y >= block->Height() )
        return  false;

    ::ULIS::FColor p = block->Color( x, y );

    FOdysseyBrushColor color( p );
    oColor = color;

    return true;
}

//static
FOdysseyBrushRect
UOdysseyBlockProxyFunctionLibrary::GetRect(FOdysseyBlockProxy Block)
{
    if( !Block.IsValid() )
        return  FOdysseyBrushRect();

    return  FOdysseyBrushRect(Block.GetBlock()->Rect());
}

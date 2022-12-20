// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureLayerImageVector.h"

#include "OdysseyTextureLayerStack.h"
#include "OdysseyPixelFormat.h"
#include "EditorStyleSet.h"
#include "ULISLoaderModule.h"
#include "ULISUtils.h"
#include "OdysseyVectorCircle.h"

#include "blend2d.h"

#define LOCTEXT_NAMESPACE "UOdysseyTextureLayerImageVector"

UOdysseyTextureLayerImageVector::~UOdysseyTextureLayerImageVector()
{

}

UOdysseyTextureLayerImageVector::UOdysseyTextureLayerImageVector()
{
	LayerTypeName = LOCTEXT("LayerTypeName", "Vector Image Layer");
    Icon = *FOdysseyStyle::GetBrush( "OdysseyLayerStack.ImageLayer16");



}

void
UOdysseyTextureLayerImageVector::OnInvalidBlock( const ::ULIS::FBlock* iBlock
                                               , const ::ULIS::FRectI* iRects
                                               , const uint32 iNumRects
                                               , void* iInfo )
{

}

void
UOdysseyTextureLayerImageVector::OnCleanupData( uint8* iData, void* iInfo )
{

}

FOdysseyVectorEngine*
UOdysseyTextureLayerImageVector::GetVectorEngine()
{
    return mVEngine;
}

void
UOdysseyTextureLayerImageVector::OnCreated_Implementation()
{
    BLImageData imgData;

    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
    if(!layerStack)
        return;

    UTexture2D* texture = layerStack->GetTexture();
    ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
    //let's ensure the format has alpha, so add alpha channel of needed
    format = static_cast< ::ULIS::eFormat >(format | ULIS_W_ALPHA( 1 ) );

    mVEngine = new FOdysseyVectorEngine( (double)texture->Source.GetSizeX()
                                       , (double)texture->Source.GetSizeY() );

    mVEngine->GetBLImage().getData( &imgData );

    mBlock = new ::ULIS::FBlock( static_cast<uint8*>(imgData.pixelData)
                               , texture->Source.GetSizeX()
                               , texture->Source.GetSizeY()
                               , ::ULIS::eFormat::Format_RGBA8
                               , nullptr
                               , ::ULIS::FOnInvalidBlock(&OnInvalidBlock, static_cast<void*>(this))
                               , ::ULIS::FOnCleanupData(&OnCleanupData, static_cast<void*>(this)) );
}

TArray<::ULIS::FEvent>
UOdysseyTextureLayerImageVector::RenderImage(::ULIS::FBlock* ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!IsActivated)
        return iWaitList;

    if (!ioBlock)
        return iWaitList;

    // TODO: set region of interest as parameter ?
    mVEngine->Render();

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mBlock->Format());

    TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(ioBlock, mBlock->Format(), iRect, iPos, iWaitList,
        [this, &ctx](::ULIS::FBlock* ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
        {
            ::ULIS::FEvent eventBlend;

            ctx.Blend(
                *mBlock,
                *ioDest,
                iRect,
                iPos,
                ::ULIS::eBlendMode(/*BlendMode*/::ULIS::eBlendMode::Blend_Normal),
                ::ULIS::Alpha_Normal,
                /*Opacity*/1.0f,
                ::ULIS::FSchedulePolicy::/*CacheEfficient*/AsyncCacheEfficient,
                iWaitList.Num(),
                iWaitList.GetData(),
                &eventBlend
            );

            return { eventBlend };
        }
    );

    ctx.Flush();
    ctx.Finish();

    return eventConvertAndExecute;
}

#undef LOCTEXT_NAMESPACE

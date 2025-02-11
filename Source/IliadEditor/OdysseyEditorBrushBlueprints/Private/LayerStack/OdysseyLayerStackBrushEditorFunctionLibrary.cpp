// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyLayerStackBrushEditorFunctionLibrary.h"

#include "OdysseyBrushAssetBase.h"
#include <ULIS>
#include "ULISLoaderModule.h"
#include "OdysseyLayerStack.h"
#include "BrushContext/OdysseyLayerStackEditorBrushContext.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "OdysseyPixelFormat.h"
#include "OdysseyAnimation.h"

//---
namespace
{
    static
    UOdysseyLayerStack*
    GetLayerStack( UOdysseyBrushAssetBase* BrushInstance )
    {
        if( !BrushInstance )
            return nullptr;

        FOdysseyLayerStackEditorBrushContext* context = BrushInstance->GetContext<FOdysseyLayerStackEditorBrushContext>("FOdysseyLayerStackEditorBrushContext");
        if (!context)
            return nullptr;

        //---

        return context->GetLayerStack();
    }

    FOdysseyBlockProxy
    GetBlockOfLayer( UOdysseyLayer* iLayer, FOdysseyBrushRect Area )
    {
        UOdysseyTextureLayer* textureLayer = Cast<UOdysseyTextureLayer>(iLayer);
        if( textureLayer )
        {
            UTexture2D* texture = Cast<UOdysseyTextureLayerStack>(textureLayer->GetLayerStack())->GetTexture();
            if (!texture)
                return FOdysseyBlockProxy::MakeNullProxy();

            ::ULIS::FRectI textureRect = ::ULIS::FRectI::FromXYWH(0, 0, texture->GetSizeX(), texture->GetSizeY());
            ::ULIS::FRectI given_rect = Area.IsInitialized() ? Area.GetValue() : textureRect;
            //be sure we copy only the needed part //TODO: Should be done directly in ULIS
            ::ULIS::FRectI src_rect = given_rect & textureRect;
            ::ULIS::FVec2I dst_pos(src_rect.x - given_rect.x, src_rect.y - given_rect.y);
            ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
            TSharedPtr<::ULIS::FBlock> dst = MakeShareable(new ::ULIS::FBlock( given_rect.w, given_rect.h, format ));

            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
            ::ULIS::FEvent eventClear;
            ctx.Clear(*dst, ::ULIS::FRectI::Auto, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &eventClear);

            TSharedPtr<IOdysseyImageRenderer> imageRenderer = textureLayer->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, 0);
            imageRenderer->Init();

            ::ULIS::FRectI dstRect = ::ULIS::FRectI::FromXYWH(dst_pos.x, dst_pos.y, given_rect.w - dst_pos.x, given_rect.h - dst_pos.y);
            FOdysseyImageRendererCopyParams params(dst, { dstRect }, src_rect.Position() - dst_pos);
            TArray<::ULIS::FEvent> eventCopy = imageRenderer->Copy(params, { eventClear });
            ctx.Flush();

            return FOdysseyBlockProxy::MakeProxy(dst, eventCopy.Num(), eventCopy.GetData());
        }

        UOdysseyAnimationLayer* animationLayer = Cast<UOdysseyAnimationLayer>(iLayer);
        if (animationLayer)
        {
            UOdysseyAnimation* animation = animationLayer->GetAnimation();
            if (!animation)
                return FOdysseyBlockProxy::MakeNullProxy();

            ::ULIS::FRectI animationRect = ::ULIS::FRectI::FromXYWH(0, 0, animation->GetWidth(), animation->GetHeight());
            ::ULIS::FRectI given_rect = Area.IsInitialized() ? Area.GetValue() : animationRect;
            //be sure we copy only the needed part //TODO: Should be done directly in ULIS
            ::ULIS::FRectI src_rect = given_rect & animationRect;
            ::ULIS::FVec2I dst_pos(src_rect.x - given_rect.x, src_rect.y - given_rect.y);
            TSharedPtr<::ULIS::FBlock> dst = MakeShareable(new ::ULIS::FBlock( given_rect.w, given_rect.h, animation->GetFormat() ));

            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(animation->GetFormat());
            ::ULIS::FEvent eventClear;
            ctx.Clear(*dst, ::ULIS::FRectI::Auto, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &eventClear);

            TSharedPtr<IOdysseyImageRenderer> imageRenderer = animationLayer->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, animation->CurrentFrame);
            imageRenderer->Init();

            ::ULIS::FRectI dstRect = ::ULIS::FRectI::FromXYWH(dst_pos.x, dst_pos.y, given_rect.w - dst_pos.x, given_rect.h - dst_pos.y);
            FOdysseyImageRendererCopyParams params(dst, { dstRect }, src_rect.Position() - dst_pos);
            TArray<::ULIS::FEvent> eventCopy = imageRenderer->Copy(params, { eventClear });
            ctx.Flush();

            return FOdysseyBlockProxy::MakeProxy(dst, eventCopy.Num(), eventCopy.GetData());
        }

        //---

        return FOdysseyBlockProxy::MakeNullProxy();
    }
}

//static
FOdysseyBlockProxy
UOdysseyLayerStackBrushEditorFunctionLibrary::GetBlockOfLayerByIndex( UOdysseyBrushAssetBase* BrushInstance, int iIndex, FOdysseyBrushRect Area )
{
    if( !BrushInstance )
        return FOdysseyBlockProxy::MakeNullProxy();

    if( Area.IsInitialized() && (Area.Width() <= 0 || Area.Height() <= 0) )
        return FOdysseyBlockProxy::MakeNullProxy();

    //---

    UOdysseyLayerStack* layerstack = GetLayerStack( BrushInstance );
    if( !layerstack )
        return FOdysseyBlockProxy::MakeNullProxy();

    TArray<UOdysseyLayer*> layers = layerstack->GetLayers();
    if (iIndex < 0 || iIndex >= layers.Num())
        return FOdysseyBlockProxy::MakeNullProxy();

    UOdysseyLayer* layer = layers[iIndex];
    if( !layer )
        return FOdysseyBlockProxy::MakeNullProxy();

    return GetBlockOfLayer(layer, Area);
}

//static
FOdysseyBlockProxy
UOdysseyLayerStackBrushEditorFunctionLibrary::GetBlockOfLayerByName( UOdysseyBrushAssetBase* BrushInstance, const FString& iName, FOdysseyBrushRect Area )
{
    if( !BrushInstance )
        return FOdysseyBlockProxy::MakeNullProxy();

    if( Area.IsInitialized() && (Area.Width() <= 0 || Area.Height() <= 0) )
        return FOdysseyBlockProxy::MakeNullProxy();

    //---

    UOdysseyLayerStack* layerstack = GetLayerStack( BrushInstance );
    if( !layerstack )
        return FOdysseyBlockProxy::MakeNullProxy();

    TArray<UOdysseyLayer*> layers = layerstack->GetLayers();
    for (UOdysseyLayer* layer : layers)
    {
        if( layer->Name.ToString() == iName )
            return GetBlockOfLayer(layer, Area);
    }
    return FOdysseyBlockProxy::MakeNullProxy();
}

//static
FOdysseyBlockProxy
UOdysseyLayerStackBrushEditorFunctionLibrary::GetBlockOfCurrentLayer( UOdysseyBrushAssetBase* BrushInstance, FOdysseyBrushRect Area )
{

    if( !BrushInstance )
        return FOdysseyBlockProxy::MakeNullProxy();

    if( Area.IsInitialized() && (Area.Width() <= 0 || Area.Height() <= 0) )
        return FOdysseyBlockProxy::MakeNullProxy();

    //---

    UOdysseyLayerStack* layerstack = GetLayerStack( BrushInstance );
    if( !layerstack )
        return FOdysseyBlockProxy::MakeNullProxy();

    UOdysseyLayer* layer = layerstack->CurrentLayer.Get();
    if (!layer)
        return FOdysseyBlockProxy::MakeNullProxy();

    return GetBlockOfLayer(layer, Area);
}

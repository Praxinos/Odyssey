// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyLayerStackBrushEditorFunctionLibrary.h"

#include "OdysseyBrushAssetBase.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ImageUtils.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include <ULIS>
#include "ULISLoaderModule.h"
#include "ULISUtils.h"
#include "OdysseyLayerStack.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationPlayer.h"
#include "OdysseyTextureLayer.h"
#include "OdysseyTextureLayerStack.h"
#include "OdysseyPixelFormat.h"
#include "OdysseyAnimation.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorBrushContext.h"
#include "OdysseyPainterEditorAnimationSource.h"

//---
namespace
{
    static
    UOdysseyLayerStack*
    GetLayerStack( UOdysseyBrushAssetBase* BrushInstance )
    {
        if( !BrushInstance )
            return nullptr;

        FOdysseyPainterEditorBrushContext* context = BrushInstance->GetContext<FOdysseyPainterEditorBrushContext>("FOdysseyPainterEditorBrushContext");
        if (!context)
            return nullptr;

        FOdysseyPainterEditor* editor = context->Editor();
        if (!editor)
            return nullptr;

        //---

        return editor->LayerStack();
    }

    static
    UOdysseyAnimationPlayer*
    GetAnimationPlayer( UOdysseyBrushAssetBase* BrushInstance )
    {
        if( !BrushInstance )
            return nullptr;

        FOdysseyPainterEditorBrushContext* context = BrushInstance->GetContext<FOdysseyPainterEditorBrushContext>("FOdysseyPainterEditorBrushContext");
        if (!context)
            return nullptr;

        FOdysseyPainterEditor* editor = context->Editor();
        if (!editor)
            return nullptr;

        return editor->GetAnimationPlayer();
    }

    FOdysseyBlockProxy
    GetBlockOfLayer( UOdysseyBrushAssetBase* BrushInstance, UOdysseyLayer* iLayer, FOdysseyBrushRect Area )
    {
        FFrameNumber frame(0);

        ::ULIS::FRectI rect;
        if (Area.IsInitialized())
        {
            rect = Area.GetValue();
        }
        else
        {
            UOdysseyTextureLayer* textureLayer = Cast<UOdysseyTextureLayer>(iLayer);
            if( textureLayer )
            {
                UTexture2D* texture = Cast<UOdysseyTextureLayerStack>(textureLayer->GetLayerStack())->GetTexture();
                if (!texture)
                    return FOdysseyBlockProxy::MakeNullProxy();

                rect = ::ULIS::FRectI::FromXYWH(0, 0, texture->GetSizeX(), texture->GetSizeY());
            }

            UOdysseyAnimationLayer* animationLayer = Cast<UOdysseyAnimationLayer>(iLayer);
            if (animationLayer)
            {
                UOdysseyAnimation* animation = animationLayer->GetAnimation();
                if (!animation)
                    return FOdysseyBlockProxy::MakeNullProxy();

                UOdysseyAnimationPlayer* player = GetAnimationPlayer(BrushInstance);
                if (!player)
                    return FOdysseyBlockProxy::MakeNullProxy();

                frame = player->GetCurrentFrame().FrameNumber;
                rect = ::ULIS::FRectI::FromXYWH(0, 0, animation->GetWidth(), animation->GetHeight());
            }
        }

        TStrongObjectPtr<UTextureRenderTarget2D> renderTarget(NewObject<UTextureRenderTarget2D>());
        renderTarget->RenderTargetFormat = RTF_RGBA8_SRGB;
        renderTarget->InitAutoFormat(rect.w, rect.h);

        iLayer->Render_GameThread(renderTarget.Get(), frame, EOdysseyRenderingType::Render, ::ULISUtils::ToIntRect(rect), FIntPoint(0, 0));

        FImage OutImage;
        if (!FImageUtils::GetRenderTargetImage(renderTarget.Get(), OutImage))
            return FOdysseyBlockProxy::MakeNullProxy();

        ::ULIS::eFormat format = ULISFormatForRawImageFormat(OutImage.Format);
        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
        TSharedPtr<::ULIS::FBlock> dst = MakeShareable(new ::ULIS::FBlock( rect.w, rect.h, format ));
        CopyImageToBlock(OutImage, dst.Get());

        return FOdysseyBlockProxy::MakeProxy(dst, 0, nullptr);
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

    return GetBlockOfLayer(BrushInstance, layer, Area);
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
        if( layer->GetLayerName().ToString() == iName )
            return GetBlockOfLayer(BrushInstance, layer, Area);
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

    UOdysseyLayer* layer = layerstack->GetCurrentLayer();
    if (!layer)
        return FOdysseyBlockProxy::MakeNullProxy();

    return GetBlockOfLayer(BrushInstance, layer, Area);
}

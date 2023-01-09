// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureLayerStack.h"
#include "OdysseyTextureLayerRoot.h"
#include "OdysseyTextureLayerFolder.h"
#include "OdysseyRasterBlock.h"
#include "TextureCompiler.h"
#include "UObject/ObjectSaveContext.h"
#include "OdysseyTextureLayerImageRaster.h"

UOdysseyTextureLayerStack::FOnRenderImageChanged&
UOdysseyTextureLayerStack::OnRenderImageChanged()
{
    static FOnRenderImageChanged onRenderImageChanged;
    return onRenderImageChanged;
}

UOdysseyTextureLayerStack*
UOdysseyTextureLayerStack::CreateFromTexture(UTexture2D* iTexture, UObject* iOuter)
{
    UOdysseyTextureLayerStack* layerStack = NewObject<UOdysseyTextureLayerStack>(iOuter, "LayerStack", RF_Public | RF_Transactional);

    //Add first layer image
    UOdysseyTextureLayerImageRaster* layer = Cast<UOdysseyTextureLayerImageRaster>(layerStack->CreateLayer(UOdysseyTextureLayerImageRaster::StaticClass()));

    //Fill LayerImage with content of Texture
    UOdysseyRasterBlock* rasterBlock = layer->GetRasterBlock();
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = rasterBlock->GetBlock();
    FillOdysseyBlockFromUTextureData(block.Get(), iTexture, block->Format());

    //Set the layer as Current Layer
    layerStack->AddLayersToHierarchy({ layer }, layerStack->LayerRoot, 0);
    layerStack->CurrentLayer = TSoftObjectPtr<UOdysseyLayer>(layer);
    rasterBlock->Update(block, { block->Rect() }, false);

    return layerStack;
}

UOdysseyTextureLayerStack::UOdysseyTextureLayerStack()
{
    CompatibleLayers.Add(UOdysseyTextureLayerFolder::StaticClass());
    CompatibleLayers.Add(UOdysseyTextureLayerImageRaster::StaticClass());

    LayerRootClass = UOdysseyTextureLayerRoot::StaticClass();
}

UTexture2D*
UOdysseyTextureLayerStack::GetTexture() const
{
    UObject* outer = GetOuter();
    while(outer)
    {
        if (outer->GetClass() == UTexture2D::StaticClass())
            return Cast<UTexture2D>(outer);

		outer = outer->GetOuter();
    }

    return nullptr;    
}

TArray<::ULIS::FEvent>
UOdysseyTextureLayerStack::RenderImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!ioBlock)
        return iWaitList;

    TArray<UOdysseyLayer*> children = GetRootLayers();
    if (children.Num() <= 0)
        return iWaitList;

    //ensure we use a sourceBlock with an alpha channel
    ::ULIS::eFormat format = static_cast< ::ULIS::eFormat >( ioBlock->Format() | ULIS_W_ALPHA( 1 ) );

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

    //Convert the destination if needed and Blend the folderBlock
    TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(ioBlock, format, iRect, iPos, iWaitList,
        [this, &children](TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
        {
            return RenderLayersImage(children, ioDest, iRect, iPos, iWaitList);
        }
    );
    ctx.Flush();

    return eventConvertAndExecute;
}

TArray<::ULIS::FEvent>
UOdysseyTextureLayerStack::RenderLayersImage(TArray<UOdysseyLayer*> iLayers, TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!ioBlock)
        return iWaitList;

    if (iLayers.Num() <= 0)
        return iWaitList;

    //Clear the block before blending on it
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( ioBlock->Format() );
    ::ULIS::FEvent eventClearBlock;
    ctx.Clear( *ioBlock, ::ULIS::FRectI::FromXYWH(iPos.x, iPos.y, iRect.w, iRect.h), ::ULIS::FSchedulePolicy::AsyncCacheEfficient, iWaitList.Num(), iWaitList.GetData(), &eventClearBlock );

    TArray<::ULIS::FEvent> lastEvent = { eventClearBlock };
    for (int i = iLayers.Num() - 1; i >= 0 ; i--)
    {
        UOdysseyTextureLayer* layer = Cast<UOdysseyTextureLayer>(iLayers[i]);
        if (!layer)
            continue;

        lastEvent = layer->RenderImage(ioBlock, iRect, iPos, lastEvent);
    }
    
    ctx.Flush();

    return lastEvent;
}

void
UOdysseyTextureLayerStack::OnRootLayerRenderImageChanged(UOdysseyTextureLayer* iLayer, const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
    OnRenderImageChanged().Broadcast(this, iRects, iIsInteractive);

    if (!iIsInteractive)
        return;

    //PATCH BEGIN: because Unreal Undo does not make package dirty correctly
    MarkPackageDirty();
    //PATCH END:

    if ( mIsEditing )
    {
        FastUpdateTexture(iRects);
    }
    else
    {
        UTexture2D* texture = GetTexture();
        if ( !texture )
            return;

        FTextureCompilingManager::Get().FinishCompilation({ texture });

        ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = MakeShareable(NewBlockFromUTextureData(texture, format));

        for ( int i = 0; i < iRects.Num(); i++ )
            RenderImage(block, iRects[i], iRects[i].Position(), TArray<::ULIS::FEvent>());

        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block->Format());
        ctx.Finish();

        CopyBlockDataIntoUTexture(block.Get(), texture);

        texture->UpdateResource();
    }
}

TSharedPtr<FOdysseySurfaceTexture2DEditable>
UOdysseyTextureLayerStack::GetSurface() const
{
    return mTextureFastUpdateSurface;
}

void
UOdysseyTextureLayerStack::StartEdit()
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    mIsEditing = true;
    mTextureFastUpdateSurface = MakeShared<FOdysseySurfaceTexture2DEditable>(texture);
    ActivateTextureFastUpdate();

    FCoreUObjectDelegates::OnPreObjectPropertyChanged.AddUObject(this, &UOdysseyTextureLayerStack::OnPreGlobalObjectPropertyChanged);
    UPackage::PreSavePackageWithContextEvent.AddUObject(this, &UOdysseyTextureLayerStack::OnPackagePreSave);
    UPackage::PackageSavedWithContextEvent.AddUObject(this, &UOdysseyTextureLayerStack::OnPackageSaved);
}

void
UOdysseyTextureLayerStack::StopEdit()
{
    FCoreUObjectDelegates::OnPreObjectPropertyChanged.RemoveAll(this);
    UPackage::PreSavePackageWithContextEvent.RemoveAll(this);
    UPackage::PackageSavedWithContextEvent.RemoveAll(this);
    InactivateTextureFastUpdate();
    UpdateTextureSource();
    mIsEditing = false;
    mTextureFastUpdateSurface = nullptr;
}

void
UOdysseyTextureLayerStack::ActivateTextureFastUpdate()
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    FTextureFormatSettings textureFormatSettings;
    texture->GetLayerFormatSettings(0, textureFormatSettings);

    // Create new Texture Properties Backup
    mTextureCompressionNone = textureFormatSettings.CompressionNone;

    // Overwrite Texture properties
    textureFormatSettings.CompressionNone = 1;
    texture->SetLayerFormatSettings(0, textureFormatSettings);

    texture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({ texture });
    texture->TemporarilyDisableStreaming(); //needed to be able to draw on previously streamed textures, avoids using NoMipMaps
}

void
UOdysseyTextureLayerStack::InactivateTextureFastUpdate()
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    FTextureFormatSettings textureFormatSettings;
    texture->GetLayerFormatSettings(0, textureFormatSettings);
    textureFormatSettings.CompressionNone = mTextureCompressionNone;
    texture->SetLayerFormatSettings(0, textureFormatSettings);
    UpdateTextureSource();
    texture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({ texture });
}

void
UOdysseyTextureLayerStack::FastUpdateTexture(const TArray<::ULIS::FRectI>& iRects)
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    FTextureCompilingManager::Get().FinishCompilation({ texture });

    for ( int i = 0; i < iRects.Num(); i++ )
        RenderImage(mTextureFastUpdateSurface->Block(), iRects[i], iRects[i].Position(), TArray<::ULIS::FEvent>());

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mTextureFastUpdateSurface->Block()->Format());
    ctx.Finish();

    mTextureFastUpdateSurface->Invalidate(iRects);
}

void
UOdysseyTextureLayerStack::UpdateTextureSource()
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    CopyBlockDataIntoUTexture(mTextureFastUpdateSurface->Block().Get(), texture);
}

void
UOdysseyTextureLayerStack::OnPreGlobalObjectPropertyChanged(UObject* iObject, const FEditPropertyChain& iEditPropertyChain)
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    if ( texture != Cast<UTexture2D>(iObject) )
        return;

    UpdateTextureSource();
    // Texture->UpdateResource();
    //FTextureCompilingManager::Get().FinishCompilation({Texture});
}

void
UOdysseyTextureLayerStack::OnPackagePreSave(UPackage* iPackage, FObjectPreSaveContext ObjectSaveContext)
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    UPackage* package = CastChecked<UPackage>(texture->GetOuter());
    if ( package != iPackage )
        return;

    InactivateTextureFastUpdate();
}

void
UOdysseyTextureLayerStack::OnPackageSaved(const FString& iPackageFilename, UPackage* iPackage, FObjectPostSaveContext ObjectSaveContext)
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    UPackage* package = CastChecked<UPackage>(texture->GetOuter());
    if ( package != iPackage )
        return;

    ActivateTextureFastUpdate();
}
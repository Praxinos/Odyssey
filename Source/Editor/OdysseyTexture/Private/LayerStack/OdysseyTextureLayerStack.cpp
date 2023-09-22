// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureLayerStack.h"
#include "OdysseyTextureLayerRoot.h"
#include "OdysseyTextureLayerFolder.h"
#include "OdysseyRasterBlock.h"
#include "TextureCompiler.h"
#include "UObject/ObjectSaveContext.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseyRectUtils.h"
#include "LayerStack/OdysseyTextureLayerStackImageRenderer.h"

UOdysseyTextureLayerStack*
UOdysseyTextureLayerStack::CreateEmptyFromTexture(UTexture2D* iTexture, UObject* iOuter)
{
    UOdysseyTextureLayerStack* layerStack = NewObject<UOdysseyTextureLayerStack>(iOuter, "LayerStack", RF_Public | RF_Transactional);
    layerStack->mInvalidTileMap = FULISInvalidTileMap(64, iTexture->Source.GetSizeX(), iTexture->Source.GetSizeY());
    return layerStack;
}

UOdysseyTextureLayerStack*
UOdysseyTextureLayerStack::CreateFromTexture(UTexture2D* iTexture, UObject* iOuter)
{
    UOdysseyTextureLayerStack* layerStack = NewObject<UOdysseyTextureLayerStack>(iOuter, "LayerStack", RF_Public | RF_Transactional);
    layerStack->mInvalidTileMap = FULISInvalidTileMap(64, iTexture->Source.GetSizeX(), iTexture->Source.GetSizeY());

    //Add first layer image
    UOdysseyTextureLayerImageRaster* layer = Cast<UOdysseyTextureLayerImageRaster>(layerStack->CreateLayer(UOdysseyTextureLayerImageRaster::StaticClass()));

    //Fill LayerImage with content of Texture
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = layer->GetRasterBlock();
    FOdysseyRasterBlockMutator rasterBlockMutator(rasterBlock, false);
    rasterBlockMutator.EditTilesFromRects(
        { ::ULIS::FRectI::FromXYWH(0, 0, rasterBlock->GetWidth(), rasterBlock->GetHeight()) },
        FOdysseyRasterBlockMutator::FEditDelegate::CreateLambda(
            [&](TSharedPtr<::ULIS::FBlock> iBlock, const FULISInvalidTileMap& iTileMap) -> TArray<ULIS::FEvent>
            {
                FillOdysseyBlockFromUTextureData(iBlock.Get(), iTexture, iBlock->Format());
                return {};
            }
        )
    );
    rasterBlockMutator.Commit();

    //Set the layer as Current Layer
    layerStack->AddLayersToHierarchy({ layer }, layerStack->LayerRoot, 0);
    layerStack->CurrentLayer = TSoftObjectPtr<UOdysseyLayer>(layer);

    return layerStack;
}

UOdysseyTextureLayerStack::~UOdysseyTextureLayerStack()
{
    FOdysseyImageRenderingAbility::OnImageRenderingChangedDelegate().RemoveAll(this);
}

UOdysseyTextureLayerStack::UOdysseyTextureLayerStack()
{
    CompatibleLayers.Add(UOdysseyTextureLayerFolder::StaticClass());
    CompatibleLayers.Add(UOdysseyTextureLayerImageRaster::StaticClass());
    CompatibleLayers.Add(UOdysseyTextureLayerImageVector::StaticClass());

    LayerRootClass = UOdysseyTextureLayerRoot::StaticClass();

    FOdysseyImageRenderingAbility::OnImageRenderingChangedDelegate().AddUObject(this, &UOdysseyTextureLayerStack::OnImageRenderingChanged);
}

void
UOdysseyTextureLayerStack::PostLoad()
{
    Super::PostLoad();
    UTexture2D* texture = GetTexture();
    if (texture)
        mInvalidTileMap = FULISInvalidTileMap(64, texture->Source.GetSizeX(), texture->Source.GetSizeY());
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

TSharedPtr<IOdysseyImageRenderer>
UOdysseyTextureLayerStack::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType) const
{
    return MakeShared<FOdysseyTextureLayerStackImageRenderer>(this, iRenderType, GetImageRenderingRects());
}

TArray<FGuid>
UOdysseyTextureLayerStack::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType) const
{
    TArray<FGuid> idComposition = { GetImageRenderingId() };
    UOdysseyTextureLayer* layerRoot = Cast<UOdysseyTextureLayer>(LayerRoot);
        if ( !layerRoot )
            return idComposition;
    
        idComposition.Append(layerRoot->GetImageRenderingComposition(iRenderType));
        return idComposition;
}

TArray<::ULIS::FRectI>
UOdysseyTextureLayerStack::GetImageRenderingRects() const
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return {};

    return { ::ULIS::FRectI::FromXYWH(0, 0, texture->Source.GetSizeX(), texture->Source.GetSizeY()) };
}

void
UOdysseyTextureLayerStack::OnImageRenderingChanged(const FOdysseyImageRenderingChangedEvent& iEvent)
{
    const FGuid& eventId =  iEvent.GetId();
    TArray<FGuid> composition = GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Editor);
    if (!composition.Contains(eventId))
        return;

    if (!iEvent.IsInteractive())
    {
        //PATCH BEGIN: because Unreal Undo does not make package dirty correctly
        MarkPackageDirty();
        //PATCH END:
    }

    if (iEvent.GetType() == FOdysseyImageRenderingChangedEvent::eEventType::kValueChange)
    {
        mInvalidTileMap.Invalidate(iEvent.GetRects());
    }
    else if (iEvent.GetType() == FOdysseyImageRenderingChangedEvent::eEventType::kCompositionChange)
    {
        mInvalidTileMap.Invalidate();
    }

    if (mTextureUpdateMode == EOdysseyTextureLayerStackTextureUpdateMode::Instantaneous)
        UpdateTexture();
}

TSharedPtr<FOdysseySurfaceTexture2DEditable>
UOdysseyTextureLayerStack::GetSurface() const
{
    return mTextureFastUpdateSurface;
}

void
UOdysseyTextureLayerStack::InactivateTextureFastUpdate()
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    FCoreUObjectDelegates::OnPreObjectPropertyChanged.RemoveAll(this);
    UPackage::PreSavePackageWithContextEvent.RemoveAll(this);
    UPackage::PackageSavedWithContextEvent.RemoveAll(this);
    CompressTexture();
    texture->UpdateResource();
    mTextureFastUpdateSurface = nullptr;
    mRenderer = nullptr;
}

void
UOdysseyTextureLayerStack::ActivateTextureFastUpdate()
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    mTextureFastUpdateSurface = MakeShared<FOdysseySurfaceTexture2DEditable>(texture);
    UncompressTexture();

    //mRenderer = BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render);

    FCoreUObjectDelegates::OnPreObjectPropertyChanged.AddUObject(this, &UOdysseyTextureLayerStack::OnPreGlobalObjectPropertyChanged);
    UPackage::PreSavePackageWithContextEvent.AddUObject(this, &UOdysseyTextureLayerStack::OnPackagePreSave);
    UPackage::PackageSavedWithContextEvent.AddUObject(this, &UOdysseyTextureLayerStack::OnPackageSaved);
}

void
UOdysseyTextureLayerStack::UncompressTexture()
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

    //needed because we need the texture resource to be uncompressed and ready for edition
    texture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({ texture });
    texture->TemporarilyDisableStreaming(); //needed to be able to draw on previously streamed textures, avoids using NoMipMaps
}

void
UOdysseyTextureLayerStack::CompressTexture()
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    FTextureFormatSettings textureFormatSettings;
    texture->GetLayerFormatSettings(0, textureFormatSettings);
    textureFormatSettings.CompressionNone = mTextureCompressionNone;
    texture->SetLayerFormatSettings(0, textureFormatSettings);
    UpdateTextureSource();
    //texture->UpdateResource();
    //FTextureCompilingManager::Get().FinishCompilation({ texture });
}

void
UOdysseyTextureLayerStack::FastUpdateTexture(const TArray<::ULIS::FRectI>& iRects)
{
    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    FTextureCompilingManager::Get().FinishCompilation({ texture });

    mRenderer = BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render); //TODO: should depend on a variable or something ?
    mRenderer->Init();
    mRenderer->Copy(mTextureFastUpdateSurface->Block(), iRects, {});

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

    CompressTexture();
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

    UncompressTexture();
}

void
UOdysseyTextureLayerStack::SetTextureUpdateMode(EOdysseyTextureLayerStackTextureUpdateMode iMode)
{
    mTextureUpdateMode = iMode;
    if (mTextureUpdateMode == EOdysseyTextureLayerStackTextureUpdateMode::Instantaneous)
        UpdateTexture();
}

EOdysseyTextureLayerStackTextureUpdateMode
UOdysseyTextureLayerStack::GetTextureUpdateMode()
{
    return mTextureUpdateMode;
}

void
UOdysseyTextureLayerStack::UpdateTexture(bool iForceRefresh)
{
    if (iForceRefresh)
        mInvalidTileMap.Invalidate();

    if (mInvalidTileMap.InvalidTiles().IsEmpty())
        return;

    UTexture2D* texture = GetTexture();
    if ( !texture )
        return;

    if ( mTextureFastUpdateSurface.IsValid() )
    {
        FastUpdateTexture(mInvalidTileMap.InvalidRects());
    }
    else
    {
        FTextureCompilingManager::Get().FinishCompilation({ texture });

        ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = MakeShareable(NewBlockFromUTextureData(texture, format));

        TArray<::ULIS::FRectI> invalidRects = mInvalidTileMap.InvalidRects();
        TSharedPtr<IOdysseyImageRenderer> renderer = BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render); //TODO: should depend on a variable or something ?
        renderer->Init();
        renderer->Copy(block, invalidRects, {});

        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block->Format());
        ctx.Finish();

        CopyBlockDataIntoUTexture(block.Get(), texture);

        texture->UpdateResource();
    }

    mInvalidTileMap.Clear();
}

void
UOdysseyTextureLayerStack::Tick(float DeltaTime)
{
    if (mTextureUpdateMode != EOdysseyTextureLayerStackTextureUpdateMode::OnTick)
        return;

    UpdateTexture();
}
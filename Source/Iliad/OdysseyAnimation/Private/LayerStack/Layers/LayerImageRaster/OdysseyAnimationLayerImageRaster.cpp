// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationLayerImageRaster.h"

#include "OdysseyLayerCellImageStagger.h"
#include "OdysseyAnimationCellImageRaster.h"

#if WITH_EDITOR
#include "OdysseyPixelFormat.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"
#include "OdysseyStyle.h"
#include "OdysseyAnimationCellsContainerImport.h"
#include "OdysseyAnimationLayerImageRasterImport.h"
#include "OdysseyLighttable.h"
#include "OdysseyRasterBlockMutator.h"
#include "OdysseyAnimation.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyMediaRaster.h"
#include "ScopedTransaction.h"
#include "UObject/DevObjectVersion.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "ULISUtils.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ImageUtils.h"
#include "OdysseyBlendShader.h"
#include "ScreenPass.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "TextureResource.h"
#endif

#define LOCTEXT_NAMESPACE "Animation"

void
UOdysseyAnimationLayerImageRaster::PostInitProperties()
{
    Super::PostInitProperties();

    LayerTypeName = LOCTEXT("layer-image-raster.type", "Raster Layer");
#if WITH_EDITOR
    Icon = FSlateIcon("OdysseyStyle", "OdysseyLayerStack.LayerBitmap16");
#endif
    DefaultCellClass = UOdysseyAnimationCellImageRaster::StaticClass();

    SupportedCellTypes.Add(UOdysseyAnimationCellImageRaster::StaticClass());
    SupportedCellTypes.Add(UOdysseyLayerCellImageStagger::StaticClass());
}

#if WITH_EDITOR
struct FOdysseyAnimationLayerImageRasterObjectVersion
{
    enum Type
    {
        // Before any version changes were made
        BeforeCustomVersionWasAdded,

        // Reworked how anim blueprint root nodes are recovered
        RefactorCellsToUObject,

        // -----<new versions can be added above this line>-------------------------------------------------
        VersionPlusOne,
        LatestVersion = VersionPlusOne - 1
    };

    // The GUID for this custom version number
    const static FGuid GUID;

private:
    FOdysseyAnimationLayerImageRasterObjectVersion() {}
};

const FGuid FOdysseyAnimationLayerImageRasterObjectVersion::GUID(0x4D889B46, 0x1B9849F6, 0xBA63C098, 0x5741EEF9);
FDevVersionRegistration GRegisterOdysseyAnimationLayerImageRasterObjectVersion(FOdysseyAnimationLayerImageRasterObjectVersion::GUID, FOdysseyAnimationLayerImageRasterObjectVersion::LatestVersion, TEXT("OdysseyAnimationLayerImageRaster"));

void
UOdysseyAnimationLayerImageRaster::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);

    Ar.UsingCustomVersion(FOdysseyAnimationLayerImageRasterObjectVersion::GUID);

    if( Ar.IsLoading() && Ar.CustomVer(FOdysseyAnimationLayerImageRasterObjectVersion::GUID) < FOdysseyAnimationLayerImageRasterObjectVersion::RefactorCellsToUObject)
    {
        if (!FOdysseyAnimationLayerImageRasterImport::Read( this, Ar ))
        {
            //Old Style No Chunk Loading
            //DEPRECATED: Keep for compatibility with Odyssey 2.0 and prior
            if (!FOdysseyAnimationCellsContainerImport::Read( this, Ar ))
            {
                //Old Style No Chunk Loading
                //Load or Save the offset
                Ar << CellsOffset;

                //Empty Cells to prepare for loading
                Cells.Empty();

                //Load or Save number of cells
                int32 numCells = 0;
                Ar << numCells;

                for ( int i = 0; i < numCells; i++ )
                {
                    //Load the cell type
                    FName cellType;
                    Ar << cellType;

                    if (cellType == TEXT("FOdysseyAnimationCellImageRaster"))
                    {
                        UOdysseyAnimationCellImageRaster* cell = NewObject<UOdysseyAnimationCellImageRaster>(this, UOdysseyAnimationCellImageRaster::StaticClass(), NAME_None, RF_Public | RF_Transactional);
                        Cells.Add(cell);
                        cell->OldSerialize(Ar);
                    }
                    else if (cellType == TEXT("FOdysseyAnimationCellImageStagger"))
                    {
                        UOdysseyLayerCellImageStagger* cell = NewObject<UOdysseyLayerCellImageStagger>(this, UOdysseyLayerCellImageStagger::StaticClass(), NAME_None, RF_Public | RF_Transactional);
                        Cells.Add(cell);
                        cell->OldSerialize(Ar);
                    }
                }
                UpdateCellsIndexInLayer();
            }
        }
    }
}

void
UOdysseyAnimationLayerImageRaster::Merge(const TArray<UOdysseyLayer*>& iLayers)
{
    UOdysseyAnimation* animation = GetAnimation();
    if ( !animation )
        return;

    if( !IsEditable() )
        return;

    Modify();

    //Get all frame ranges and combine them
    TArray<FInt32Range> frameRanges = {};
    for (int layerIndex = 0; layerIndex < iLayers.Num(); layerIndex++)
    {
        UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(iLayers[layerIndex]);
        if ( !layer )
            continue;

        frameRanges.Add(layer->GetFrameRange());
    }
    FInt32Range frameRange = FInt32Range::Hull(frameRanges);

    //Deduce offset from frame ranges
    SetCellsOffset(frameRange.GetLowerBoundValue());

    //Get cell ranges from each frame ImageRenderAbility composition
    int startFrame = frameRange.GetLowerBound().IsInclusive() ? frameRange.GetLowerBoundValue() : frameRange.GetLowerBoundValue() + 1;
    int endFrame = frameRange.GetUpperBound().IsInclusive() ? frameRange.GetUpperBoundValue() : frameRange.GetUpperBoundValue() - 1;
    TArray<FGuid> previousIds;
    TArray<FInt32Range> cellRanges;
    for (int frameIndex = startFrame; frameIndex <= endFrame; frameIndex++)
    {
        TArray<FGuid> currentIds;
        for (int layerIndex = 0; layerIndex < iLayers.Num(); layerIndex++)
        {
            UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(iLayers[layerIndex]);
            if ( !layer )
                continue;

            currentIds.Append(layer->GetRenderingComposition(EOdysseyRenderingType::Render, frameIndex));
        }

        //Do we need a new cell
        if (currentIds != previousIds)
        {
            //do we already have a cell at this position
            cellRanges.Add(FInt32Range::Inclusive(frameIndex, frameIndex));
            previousIds = currentIds;
        }
        else
        {
            cellRanges.Last().SetUpperBoundValue(frameIndex);
        }
    }

    RemoveCells(Cells);
    TArray<UOdysseyLayerCell*> cells = AddCells(UOdysseyAnimationCellImageRaster::StaticClass(), 0, cellRanges.Num());

    FIntRect rect = GetDefaultRenderRect();
    TStrongObjectPtr<UTextureRenderTarget2D> layerRenderTarget(CreateRenderingRenderTarget());
    TStrongObjectPtr<UTextureRenderTarget2D> destinationRenderTarget(CreateRenderingRenderTarget());

    for (int i = 0; i < cellRanges.Num(); i++)
    {
        const FInt32Range& cellRange = cellRanges[i];
        UOdysseyAnimationCellImageRaster* cell = Cast<UOdysseyAnimationCellImageRaster>(cells[i]);
        int frame = cellRange.GetLowerBoundValue();

        cell->SetExposure(cellRange.GetUpperBoundValue() - cellRange.GetLowerBoundValue() + 1);

        //Clear the destination rendertarget before blending on it
        ENQUEUE_RENDER_COMMAND(IOdysseyTextureRenderingAbility_RenderRectAtRect)(
            [destinationRenderTarget, rect](FRHICommandListImmediate& RHICmdList)
            {
                FRDGBuilder graphBuilder(RHICmdList);
                FRDGTextureRef destinationTexture = destinationRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );
                AddClearRenderTargetPass(graphBuilder, destinationTexture, FLinearColor::Transparent, rect);
                graphBuilder.Execute();
            }
        );

        for (int layerIndex = 0; layerIndex < iLayers.Num(); layerIndex++)
        {
            UOdysseyLayer* layer = iLayers[layerIndex];
            if ( !layer )
                continue;

            layer->Render_GameThread(
                layerRenderTarget.Get(),
                FFrameNumber(frame),
                EOdysseyRenderingType::Render
            );

            const ERHIFeatureLevel::Type featureLevel = GMaxRHIFeatureLevel;

            ENQUEUE_RENDER_COMMAND(IOdysseyTextureRenderingAbility_RenderRectAtRect)(
                [layerRenderTarget, destinationRenderTarget, featureLevel, rect, layer](FRHICommandListImmediate& RHICmdList)
                {
                    FRDGBuilder graphBuilder(RHICmdList);

                    FRDGTextureRef layerTexture = layerRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );
                    FRDGTextureRef destinationTexture = destinationRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );

                    FOdysseyBlendShader::BlendRect(
                        graphBuilder,
                        featureLevel,
                        destinationTexture,
                        layerTexture,
                        destinationTexture,
                        rect,
                        rect,
                        FMatrix::Identity,
                        layer->GetBlendMode(),
                        EOdysseyAlphaMode::kNormal,
                        layer->GetOpacity(),
                        EOdysseyAntiAliasing::NearestNeighbor
                    );

                    graphBuilder.Execute();
                }
            );
        }

        FImage OutImage;
        if (!FImageUtils::GetRenderTargetImage(destinationRenderTarget.Get(), OutImage))
            continue;

        ::ULIS::eFormat format = ULISFormatForRawImageFormat(OutImage.Format);

        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
        TSharedPtr<::ULIS::FBlock> block = MakeShareable(new ::ULIS::FBlock( rect.Width(), rect.Height(), format ));
        CopyImageToBlock(OutImage, block.Get());

        FOdysseyRasterBlockMutator mutator(cell->GetRasterBlock());
        mutator.Copy(block, {::ULISUtils::ToULISRectI(rect)});
        mutator.Commit();
    }
}

TSharedPtr<IOdysseyMedia>
UOdysseyAnimationLayerImageRaster::GetCellMediaRaster(uint32 iFrameIndex) const
{
    UOdysseyLayerCell* cell = GetCellAtFrame(iFrameIndex);
    if (!cell)
        return nullptr;

    FInt32Range frameRange = cell->GetFrameRange();
    int cellFrame = frameRange.GetLowerBoundValue();

    FOdysseyMediaProvider provider = cell->GetMediaProvider();
    if (!provider.HasMedia<FOdysseyMediaRaster>())
        return nullptr;

    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasterList = provider.GetOrCreateMedias<FOdysseyMediaRaster>();
    if (mediaRasterList.Num() <= 0)
        return nullptr;

    return mediaRasterList[0];
}

FOdysseyMediaProvider
UOdysseyAnimationLayerImageRaster::GetMediaProvider(uint32 iFrameIndex) const
{
    FOdysseyMediaProvider provider;

    bool isActive = IsActivatedRecursively();
    bool isLocked = IsLockedRecursively();
    provider.IsLocked(!isActive || isLocked);

    TSharedPtr<IOdysseyMedia> cellMediaRaster = GetCellMediaRaster(iFrameIndex);
    if (cellMediaRaster)
    {
        provider.Add(cellMediaRaster);
    }
    else
    {
        FOdysseyMediaProvider::FCreateMediaDelegate createMediaRasterDelegate = FOdysseyMediaProvider::FCreateMediaDelegate::CreateUObject(const_cast<UOdysseyAnimationLayerImageRaster*>(this), &UOdysseyAnimationLayerImageRaster::CreateMediaRaster, (int)iFrameIndex);
        provider.Add(FOdysseyMediaRaster::StaticId(), createMediaRasterDelegate);
    }
    return provider;
}

TSharedPtr<IOdysseyMedia>
UOdysseyAnimationLayerImageRaster::CreateMediaRaster(int iFrameIndex)
{
    //Create the cell if needed
    AutoCreateCell(iFrameIndex);
    return GetCellMediaRaster(iFrameIndex);
}

void
UOdysseyAnimationLayerImageRaster::AutoCreateCell(int iFrameIndex)
{
    if( !IsEditable() )
        return;

    //Check if iFrameIndex is Out Of Range
    FInt32Range range = GetFrameRange();

    //special case when there is no cells in the layer and current frame is exactly at cells offset
    if (iFrameIndex == range.GetLowerBoundValue() && iFrameIndex == range.GetUpperBoundValue())
    {
        //Add a frame at current frame and extend it
        FScopedTransaction transaction(LOCTEXT("layer-image-raster.create-cell-transaction", "Create Cell"));
        Modify();
        UOdysseyLayerCell* cell = AddCell(UOdysseyAnimationCellImageRaster::StaticClass(), 0);
        return;
    }

    if ( iFrameIndex < range.GetLowerBoundValue())
    {
        //Add a frame at current frame and extend it
        FScopedTransaction transaction( LOCTEXT( "layer-image-raster.create-cell-transaction", "Create Cell" ) );
        Modify();
        UOdysseyLayerCell* cell = AddCell(UOdysseyAnimationCellImageRaster::StaticClass(), 0);
        cell->SetExposure(range.GetLowerBoundValue() - iFrameIndex);
        SetCellsOffset(GetCellsOffset() - cell->GetExposure());
        return;
    }

    if ( iFrameIndex > range.GetUpperBoundValue())
    {
        FScopedTransaction transaction( LOCTEXT( "layer-image-raster.create-cell-transaction", "Create Cell" ) );
        Modify();
        UOdysseyLayerCell* lastCell = Cells.IsEmpty() ? nullptr : Cells.Last();
        int cellExposure = iFrameIndex - CellsOffset + 1;
        if(lastCell)
        {
            cellExposure = 1;
            int lastCellExposure = lastCell->GetExposure() + iFrameIndex - range.GetUpperBoundValue() - 1;
            lastCell->SetExposure(lastCellExposure);
        }
        UOdysseyLayerCell* cell = AddCell(UOdysseyAnimationCellImageRaster::StaticClass());
        cell->SetExposure(cellExposure);
        return;
    }
}

void
UOdysseyAnimationLayerImageRaster::SetIsAlphaLocked(bool Value)
{
    if( !IsEditable() )
        return;

    Modify();
    bIsAlphaLocked = Value;
}

bool
UOdysseyAnimationLayerImageRaster::IsAlphaLocked() const
{
    return bIsAlphaLocked;
}

#endif
#undef LOCTEXT_NAMESPACE

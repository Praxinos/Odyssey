// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"

#include "OdysseyPixelFormat.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStagger.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainerImport.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRasterImageRenderer.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRasterImport.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "OdysseyRasterBlockMutator.h"
#include "OdysseyAnimationCurrentFrameMutator.h"
#include "OdysseyAnimation.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyMediaRaster.h"
#include "ScopedTransaction.h"
#include "UObject/DevObjectVersion.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "Animation"



void
UOdysseyAnimationLayerImageRaster::PostInitProperties()
{
    Super::PostInitProperties();
    
    LayerTypeName = LOCTEXT("layer-image-raster.type", "Raster Image Layer");
    Icon = FSlateIcon("OdysseyStyle", "OdysseyLayerStack.LayerBitmap16");
    DefaultCellClass = UOdysseyAnimationCellImageRaster::StaticClass();

    SupportedCellTypes.Add(UOdysseyAnimationCellImageRaster::StaticClass());
    SupportedCellTypes.Add(UOdysseyAnimationCellImageStagger::StaticClass());
}

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
                        UOdysseyAnimationCellImageStagger* cell = NewObject<UOdysseyAnimationCellImageStagger>(this, UOdysseyAnimationCellImageStagger::StaticClass(), NAME_None, RF_Public | RF_Transactional);
                        Cells.Add(cell);
                        cell->OldSerialize(Ar);
                    }
                }
                UpdateCellsIndexInLayer();
            }
        }
    }
}

TSharedPtr<IOdysseyImageRenderer>
UOdysseyAnimationLayerImageRaster::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter) const
{
    if (iFilter.IsBound() && !iFilter.Execute(this))
        return nullptr;
    
    return MakeShared<FOdysseyAnimationLayerImageRasterImageRenderer>(this, iFrame, iRenderType, GetImageRenderingRects(), iFilter);
}

TArray<FGuid>
UOdysseyAnimationLayerImageRaster::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const
{
    TArray<FGuid> idComposition = { GetImageRenderingId() };

    bool showLighttable = iRenderType == IOdysseyImageRenderer::eRenderType::Editor && Lighttable.bIsActivated;
    if (showLighttable && Lighttable.DisplayPosition == EOdysseyLightTableDisplayPosition::UnderLayer )
        idComposition.Append(GetLighttableImageRenderingComposition(iFrameIndex));

    int frame = iFrameIndex;
    FInt32Range frameRange = GetFrameRange();
    if (iFrameIndex < frameRange.GetLowerBoundValue())
    {
        frame = GetPreBehaviourFrame(PreBehaviour, iFrameIndex);
    }
    else if (iFrameIndex > frameRange.GetUpperBoundValue())
    {
        frame = GetPostBehaviourFrame(PostBehaviour, iFrameIndex);
    }

    UOdysseyAnimationCell* cell = GetCellAtFrame(frame);
    if (cell)
    {
        int cellFrame = frame - cell->GetFrameRange().GetLowerBoundValue();
        idComposition.Append(cell->GetImageRenderingComposition(iRenderType, cellFrame));
    }

    if (showLighttable && Lighttable.DisplayPosition == EOdysseyLightTableDisplayPosition::AboveLayer )
        idComposition.Append(GetLighttableImageRenderingComposition(iFrameIndex));

    return idComposition;
}

void
UOdysseyAnimationLayerImageRaster::Merge(const TArray<UOdysseyLayer*>& iLayers)
{
    if (IsLocked)
        return;

    UOdysseyAnimation* animation = GetAnimation();
    if ( !animation )
        return;

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
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset), frameRange.GetLowerBoundValue());

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

            currentIds.Append(layer->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, frameIndex));
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

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    
    RemoveCells(Cells);
    TArray<UOdysseyAnimationCell*> cells = AddCells(UOdysseyAnimationCellImageRaster::StaticClass(), 0, cellRanges.Num());
    
    for (int i = 0; i < cellRanges.Num(); i++)
    {
        const FInt32Range& cellRange = cellRanges[i];
        UOdysseyAnimationCellImageRaster*cell = Cast<UOdysseyAnimationCellImageRaster>(cells[i]);
        FOdysseyObjectEditorUtils::SetPropertyValue(cell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), cellRange.GetUpperBoundValue() - cellRange.GetLowerBoundValue() + 1);
        
        TSharedPtr<FOdysseyRasterBlock> rasterBlock = cell->GetRasterBlock();
        FOdysseyRasterBlockMutator blockMutator(rasterBlock);

        int frame = cellRange.GetLowerBoundValue();
        ::ULIS::FRectI rect = ::ULIS::FRectI::FromXYWH(0, 0, rasterBlock->GetWidth(), rasterBlock->GetHeight());

        blockMutator.EditTilesFromRects(
            { rect },
            [&](TSharedPtr<::ULIS::FBlock> iBlock, const FULISInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
            {
                TArray<::ULIS::FEvent> lastEvent;
                for (int layerIndex = 0; layerIndex < iLayers.Num(); layerIndex++)
                {
                    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(iLayers[layerIndex]);
                    if ( !layer )
                        continue;

                    TSharedPtr<IOdysseyImageRenderer> renderer = layer->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, frame);
                    renderer->Init();

                    FOdysseyImageRendererBlendParams params(iBlock, { rect });
                    params.mBlendMode = (::ULIS::eBlendMode)layer->BlendMode;
                    params.mOpacity = layer->Opacity;
                    lastEvent = renderer->Blend(params, lastEvent);
                }

                return lastEvent;
            }
        );
    }

    ctx.Finish();
}

TSharedPtr<IOdysseyMedia>
UOdysseyAnimationLayerImageRaster::GetCellMediaRaster(uint32 iFrameIndex) const
{
    UOdysseyAnimationCell* cell = GetCellAtFrame(iFrameIndex);
    if (!cell)
        return nullptr;
    
    FInt32Range frameRange = cell->GetFrameRange();
    int cellFrame = frameRange.GetLowerBoundValue();

    FOdysseyMediaProvider provider = cell->GetMediaProvider(cellFrame);
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
    if (IsLocked)
        return;

    FScopedTransaction transaction(LOCTEXT("layer-image-raster.create-cell-transaction", "Create Cell"));

    UOdysseyAnimation* animation = GetAnimation();
    //Check if iFrameIndex is Out Of Range
    FInt32Range range = GetFrameRange();

    //special case when there is no cells in the layer and current frame is exactly at cells offset
    if (iFrameIndex == range.GetLowerBoundValue() && iFrameIndex == range.GetUpperBoundValue())
    {
        //Add a frame at current frame and extend it
        Modify();
        UOdysseyAnimationCell* cell = AddCell(UOdysseyAnimationCellImageRaster::StaticClass(), 0);

        FOdysseyAnimationCurrentFrameMutator currentFrameMutator(animation);
        currentFrameMutator.Set(animation->CurrentFrame);
        currentFrameMutator.Commit();
        return;
    }

    if ( iFrameIndex < range.GetLowerBoundValue())
    {
        //Add a frame at current frame and extend it
        Modify();
        UOdysseyAnimationCell* cell = AddCell(UOdysseyAnimationCellImageRaster::StaticClass(), 0);
        FOdysseyObjectEditorUtils::SetPropertyValue(cell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), range.GetLowerBoundValue() - iFrameIndex);
        FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset), CellsOffset - cell->Exposure);
        
        FOdysseyAnimationCurrentFrameMutator currentFrameMutator(animation);
        currentFrameMutator.Set(animation->CurrentFrame);
        currentFrameMutator.Commit();
        return;
    }

    if ( iFrameIndex > range.GetUpperBoundValue())
    {
        Modify();

        UOdysseyAnimationCell* lastCell = Cells.IsEmpty() ? nullptr : Cells.Last();
        int cellExposure = iFrameIndex - CellsOffset + 1;
        if(lastCell)
        {
            cellExposure = 1;
            int lastCellExposure = lastCell->Exposure + iFrameIndex - range.GetUpperBoundValue() - 1;
            FOdysseyObjectEditorUtils::SetPropertyValue(lastCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), lastCellExposure);
        }
        UOdysseyAnimationCell* cell = AddCell(UOdysseyAnimationCellImageRaster::StaticClass());
        FOdysseyObjectEditorUtils::SetPropertyValue(cell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), cellExposure);
        
        FOdysseyAnimationCurrentFrameMutator currentFrameMutator(animation);
        currentFrameMutator.Set(animation->CurrentFrame);
        currentFrameMutator.Commit();
        return;
    }
}

void
UOdysseyAnimationLayerImageRaster::IsAlphaLockedBlueprintSetter(bool Value)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayerImageRaster, IsAlphaLocked), Value);
}

#undef LOCTEXT_NAMESPACE

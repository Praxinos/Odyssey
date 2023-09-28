// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"

#include "OdysseyPixelFormat.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRasterImageRenderer.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "OdysseyRasterBlockMutator.h"
#include "OdysseyLayerFunctionLibrary.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRasterExport.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRasterImport.h"

#define LOCTEXT_NAMESPACE "UOdysseyAnimationLayerImageRaster"

UOdysseyAnimationLayerImageRaster::FOnBlendModeChanged&
UOdysseyAnimationLayerImageRaster::OnBlendModeChanged()
{
    static FOnBlendModeChanged onBlendModeChanged;
    return onBlendModeChanged;
}

UOdysseyAnimationLayerImageRaster::FOnOpacityChanged&
UOdysseyAnimationLayerImageRaster::OnOpacityChanged()
{
    static FOnOpacityChanged onOpacityChanged;
    return onOpacityChanged;
}

/* UOdysseyAnimationLayerImageRaster::FOnCellsChanged&
UOdysseyAnimationLayerImageRaster::OnCellsChanged()
{
    static FOnCellsChanged onCellsChanged;
    return onCellsChanged;
} */

UOdysseyAnimationLayerImageRaster::~UOdysseyAnimationLayerImageRaster()
{
}

UOdysseyAnimationLayerImageRaster::UOdysseyAnimationLayerImageRaster()
    : mCellsContainer(MakeShared<FOdysseyAnimationCellsContainer>())
{
	LayerTypeName = LOCTEXT("LayerTypeName", "Raster Image Layer");
    Icon = *FOdysseyStyle::GetBrush( "OdysseyLayerStack.ImageLayer16");
}

void
UOdysseyAnimationLayerImageRaster::OnCreated_Implementation()
{
}

FInt32Range
UOdysseyAnimationLayerImageRaster::GetFrameRange() const
{
    return mCellsContainer->GetFrameRange();
}

/* int
UOdysseyAnimationLayerImageRaster::GetCellsCount() const
{
    return mCells.Num();
}

bool
UOdysseyAnimationLayerImageRaster::GetCellIndexAtFrame(int iFrameIndex, int& oCellIndex, int& oCellFrameIndex) const
{
    if( iFrameIndex < mOffset )
        return false;

    int frameIndex = mOffset;
    for (int i = 0; i < mCells.Num(); i++)
    {
        TSharedPtr<FOdysseyAnimationCell> cell = mCells[i];

        if ( frameIndex + cell->GetLength() - 1 >= iFrameIndex)
        {
            oCellIndex = i;
            oCellFrameIndex = iFrameIndex - frameIndex;
            return true;
        }

        frameIndex += cell->GetLength();
    }

    return false;
}

bool
UOdysseyAnimationLayerImageRaster::GetCellFrameRange(int iIndex, FInt32Range& oFrameRange) const
{
    if (iIndex < 0 || iIndex >= mCells.Num())
        return false;

    uint32 startFrame = mOffset;
    for (int i = 0; i < iIndex; i++ )
    {
        startFrame += mCells[i]->GetLength();
    }
    oFrameRange = FInt32Range::Inclusive(startFrame, startFrame + mCells[iIndex]->GetLength() - 1);
    return true;
}

bool
UOdysseyAnimationLayerImageRaster::GetCellLength(int iIndex, int& oLength) const
{
    if (iIndex < 0 || iIndex >= mCells.Num())
        return false;

    oLength = mCells[iIndex]->GetLength();
    return true;
}

bool
UOdysseyAnimationLayerImageRaster::GetCellType(int iIndex, FName& oType) const
{
    if (iIndex < 0 || iIndex >= mCells.Num())
        return false;

    oType = mCells[iIndex]->GetType();
    return true;
} */

TSharedPtr<FOdysseyAnimationLightTable>
UOdysseyAnimationLayerImageRaster::GetLightTable() const
{
    return mLightTable;
}

/*
int
UOdysseyAnimationLayerImageRaster::GetOffset() const
{
    return mOffset;
}
*/

TSharedRef<FOdysseyAnimationCellsContainer>
UOdysseyAnimationLayerImageRaster::GetCellsContainer() const
{
    return mCellsContainer;
}

/* TSharedPtr<FOdysseyAnimationCell>
UOdysseyAnimationLayerImageRaster::GetCell(int iIndex) const
{
    if ( iIndex < 0 || iIndex >= mCells.Num() )
        return nullptr;

    return mCells[iIndex];
}

TSharedPtr<FOdysseyAnimationCell> 
UOdysseyAnimationLayerImageRaster::GetCellAtFrame(int iFrameIndex, int& oCelFrameIndex) const
{
    int celIndex = INDEX_NONE;
    if ( !GetCellIndexAtFrame(iFrameIndex, celIndex, oCelFrameIndex) )
        return nullptr;

    return mCells[celIndex];
} */

void
UOdysseyAnimationLayerImageRaster::Merge(const TArray<UOdysseyLayer*>& iLayers)
{
    UOdysseyAnimation* animation = GetAnimation();
    if ( !animation )
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("Layer Image Raster", "Merge Layers"));
#endif

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
    int offset = frameRange.GetLowerBoundValue();

    //Get cell ranges from each frame ImageRenderAbility composition
    int startFrame = frameRange.GetUpperBound().IsInclusive() ? frameRange.GetLowerBoundValue() : frameRange.GetLowerBoundValue() + 1;
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

    TArray<TSharedPtr<FOdysseyAnimationCell>> cells;
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    
    for (const FInt32Range& cellRange : cellRanges)
    {
        TSharedPtr<FOdysseyAnimationCellImageRaster> cell = FOdysseyAnimationCellImageRaster::Create(this, animation->Width(), animation->Height(), animation->Format());
        cell->SetLength(cellRange.GetUpperBoundValue() - cellRange.GetLowerBoundValue() + 1);

        TSharedPtr<FOdysseyRasterBlock> rasterBlock = cell->GetRasterBlock();
        FOdysseyRasterBlockMutator blockMutator(rasterBlock);

        int frame = cellRange.GetLowerBoundValue();
        ::ULIS::FRectI rect = ::ULIS::FRectI::FromXYWH(0, 0, rasterBlock->GetWidth(), rasterBlock->GetHeight());

        blockMutator.EditTilesFromRects(
            { rect },
            FOdysseyRasterBlockMutator::FEditDelegate::CreateLambda(
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
                        lastEvent = renderer->Blend(iBlock, layer->GetImageRenderingBlendMode(), layer->GetImageRenderingOpacity(), rect, lastEvent);
                    }

                    return lastEvent;
                }
            )
        );

        cells.Add(cell);
    }

    ctx.Finish();
    
    FOdysseyAnimationCellsMutator mutator(this, mCellsContainer);
    mutator.Remove(0, mCellsContainer->GetCells().Num());
    mutator.Add(cells);
    mutator.SetOffset(offset);
    mutator.Commit();
}

void
UOdysseyAnimationLayerImageRaster::IsLightTableActivatedChanged()
{   
    UOdysseyAnimation* animation = GetAnimation();
    if ( !animation )
        return;

    ImageRenderingChanged();
}

void
UOdysseyAnimationLayerImageRaster::OpacityChanged()
{
    OnOpacityChanged().Broadcast(this);

    ImageRenderingChanged();
}

void
UOdysseyAnimationLayerImageRaster::BlendModeChanged()
{
    OnBlendModeChanged().Broadcast(this);
    
    ImageRenderingChanged();
}

void
UOdysseyAnimationLayerImageRaster::OnCellsChanged()
{
    //OnCellsChanged().Broadcast(this);
    ImageRenderingCompositionChanged();
    UOdysseyLayer::OnMediaChanged().Broadcast();
}

void
UOdysseyAnimationLayerImageRaster::PropertyChanged(const FName& iPropertyName)
{
    Super::PropertyChanged(iPropertyName);

    if (iPropertyName == "BlendMode")
        BlendModeChanged();
    if (iPropertyName == "Opacity")
        OpacityChanged();
    if (iPropertyName == "bIsLightTableActivated")
        IsLightTableActivatedChanged();
}

TSharedPtr<FOdysseyAnimationCell>
UOdysseyAnimationLayerImageRaster::CreateCell( const FName& iCellType, bool iForSerialization)
{
    if (iCellType == FOdysseyAnimationCellImageRaster::StaticType())
    {
        if (iForSerialization)
            return MakeShared<FOdysseyAnimationCellImageRaster>(this);

        UOdysseyAnimation* animation = GetAnimation();
        return FOdysseyAnimationCellImageRaster::Create(this, animation->Width(), animation->Height(), animation->Format());
    }
    return nullptr;
}

void
UOdysseyAnimationLayerImageRaster::PostInitProperties()
{
    Super::PostInitProperties();

    mCellsContainer->CreateCellDelegate().BindUObject(this, &UOdysseyAnimationLayerImageRaster::CreateCell);
    mCellsContainer->OnCellsChanged().AddUObject(this, &UOdysseyAnimationLayerImageRaster::OnCellsChanged);
    mLightTable = MakeShared<FOdysseyAnimationLightTable>(this);
}

void
UOdysseyAnimationLayerImageRaster::PostDuplicate(bool bDuplicateForPIE)
{
    Super::PostDuplicate(bDuplicateForPIE);
    mCellsContainer->PostDuplicate();
}

void
UOdysseyAnimationLayerImageRaster::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);

    if( Ar.IsSaving() )
    {
        FOdysseyAnimationLayerImageRasterExport::Write( this, Ar );
    }

    if( Ar.IsLoading() )
    {
        if (!FOdysseyAnimationLayerImageRasterImport::Read( this, Ar ))
        {
            //Old Style No Chunk Loading
            mCellsContainer->Serialize(Ar);
        }
    }
}

TSharedPtr<IOdysseyImageRenderer>
UOdysseyAnimationLayerImageRaster::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame) const
{
    return MakeShared<FOdysseyAnimationLayerImageRasterImageRenderer>(this, iFrame, iRenderType, GetImageRenderingRects());
}

TArray<FGuid>
UOdysseyAnimationLayerImageRaster::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const
{
    TArray<FGuid> idComposition = { GetImageRenderingId() };

    if (iRenderType == IOdysseyImageRenderer::eRenderType::Editor && bIsLightTableActivated && mLightTable->GetDisplayPosition() == EOdysseyLightTableDisplayPosition::UnderLayer )
    {
        idComposition.Append(mLightTable->GetImageRenderingComposition(iRenderType, iFrameIndex));
    }

    //TODO: GetCell from mCellsContainer
    int celFrameIndex = mCellsContainer->GetCellFrameAtFrame(iFrameIndex);
    TSharedPtr<FOdysseyAnimationCell> cell = mCellsContainer->GetCellAtFrame(iFrameIndex);
    if (cell)
    {
        idComposition.Append(cell->GetImageRenderingComposition(iRenderType, celFrameIndex));
    }

    if (iRenderType == IOdysseyImageRenderer::eRenderType::Editor && bIsLightTableActivated && mLightTable->GetDisplayPosition() == EOdysseyLightTableDisplayPosition::AboveLayer )
    {
        idComposition.Append(mLightTable->GetImageRenderingComposition(iRenderType, iFrameIndex));
    }

    return idComposition;
}

::ULIS::eBlendMode
UOdysseyAnimationLayerImageRaster::GetImageRenderingBlendMode() const
{
    return (::ULIS::eBlendMode)BlendMode;
}

float
UOdysseyAnimationLayerImageRaster::GetImageRenderingOpacity() const
{
    return Opacity;
}

TSharedPtr<IOdysseyMedia>
UOdysseyAnimationLayerImageRaster::GetCellMediaRaster(uint32 iFrameIndex) const
{
	int celFrameIndex = mCellsContainer->GetCellFrameAtFrame(iFrameIndex);
    TSharedPtr<FOdysseyAnimationCell> cell = mCellsContainer->GetCellAtFrame(iFrameIndex);
    if (!cell || celFrameIndex != 0)
        return nullptr;

    FOdysseyMediaProvider provider = cell->GetMediaProvider(celFrameIndex);
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
	TSharedPtr<IOdysseyMedia> cellMediaRaster = GetCellMediaRaster(iFrameIndex);
	if (cellMediaRaster)
	{
		bool isActive = UOdysseyLayerFunctionLibrary::IsLayerActivatedInStack(this);
		bool isLocked = UOdysseyLayerFunctionLibrary::IsLayerLockedInStack(this);
		cellMediaRaster->IsLocked(!isActive || isLocked);
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

    int celFrameIndex = mCellsContainer->GetCellFrameAtFrame(iFrameIndex);
    TSharedPtr<FOdysseyAnimationCell> cell = mCellsContainer->GetCellAtFrame(iFrameIndex);
    if (!cell)
        return nullptr;

    FOdysseyMediaProvider provider = cell->GetMediaProvider(celFrameIndex);
    if (!provider.HasMedia<FOdysseyMediaRaster>())
        return nullptr;

    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasterList = provider.GetOrCreateMedias<FOdysseyMediaRaster>();
    if (mediaRasterList.Num() <= 0)
        return nullptr;

    TSharedPtr<FOdysseyMediaRaster> mediaRaster = mediaRasterList[0];

    bool isActive = UOdysseyLayerFunctionLibrary::IsLayerActivatedInStack(this);
    bool isLocked = UOdysseyLayerFunctionLibrary::IsLayerLockedInStack(this);

    mediaRaster->IsLocked(!isActive || isLocked);
    return mediaRaster;
}

void
UOdysseyAnimationLayerImageRaster::AutoCreateCell(int iFrameIndex)
{
    UOdysseyAnimation* animation = GetAnimation();
    //Check if iFrameIndex is Out Of Range
    FInt32Range range = mCellsContainer->GetFrameRange();
    if ( iFrameIndex < range.GetLowerBoundValue())
    {
        if (!bAutoAddCells) //If out of range is not allowed, return
            return;

        int length = range.GetLowerBoundValue() - iFrameIndex;

        //Add a frame at current frame and extend it 
        TSharedPtr<FOdysseyAnimationCellImageRaster> cell = FOdysseyAnimationCellImageRaster::Create(this, animation->Width(), animation->Height(), animation->Format());
        cell->SetLength(length);

        FOdysseyAnimationCellsMutator mutator(this, mCellsContainer);
        mutator.Add({cell}, 0);
        mutator.SetOffset(mCellsContainer->GetOffset() - length);
        mutator.Commit();
        return;
    }

    if ( iFrameIndex > range.GetUpperBoundValue())
    {
        if (!bAutoAddCells) //If out of range is not allowed, return
            return;

        //Add a frame at current frame and extend previous frame to it 
        TSharedPtr<FOdysseyAnimationCellImageRaster> cell = FOdysseyAnimationCellImageRaster::Create(this, animation->Width(), animation->Height(), animation->Format());
        cell->SetLength(1);
        
        int cellCount = mCellsContainer->GetCells().Num();
        int lastCellLength = mCellsContainer->GetCells().Last()->GetLength();

        FOdysseyAnimationCellsMutator mutator(this, mCellsContainer);
        mutator.SetLength( cellCount - 1, lastCellLength + iFrameIndex - range.GetUpperBoundValue() - 1);
        mutator.Add({cell});
        mutator.Commit();
        return;
    }
    
    //iFrameIndex is not Out Of Range

    int cellIndex = mCellsContainer->GetCellIndexAtFrame(iFrameIndex);
    if (cellIndex == INDEX_NONE)
        return;

    int cellFrameIndex = mCellsContainer->GetCellFrameAtFrame(iFrameIndex);
    if (cellFrameIndex == INDEX_NONE)
        return;

    if (cellFrameIndex == 0) //is Cell Head
        return;

    //Is Not Head
    if (!bAutoBreakCells) //If breaking a cell is not allowed, return
        return;

    TSharedPtr<FOdysseyAnimationCell> currentCell = mCellsContainer->GetCells()[cellIndex];
    int currentCellLength = cellFrameIndex;
    int newCellLength = currentCell->GetLength() - currentCellLength;

    TSharedPtr<FOdysseyAnimationCell> cell = currentCell->CreateCellFromFrame(cellFrameIndex);
    cell->SetLength(newCellLength);

    FOdysseyAnimationCellsMutator mutator(this, mCellsContainer);
    mutator.SetLength(cellIndex, currentCellLength);
    mutator.Add({ cell }, cellIndex + 1);
    mutator.Commit();

    /* TSharedPtr<FOdysseyAnimationCell> currentCell = mCellsContainer->GetCells()[cellIndex];
    int currentCellLength = cellFrameIndex;
    int newCellLength = currentCell->GetLength() - currentCellLength;

    //Copy Current Cell block at given frameindex
    TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(animation->Width(), animation->Height(), animation->Format());
    TSharedPtr<IOdysseyImageRenderer> renderer = currentCell->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, cellFrameIndex);
    renderer->Init();
    renderer->Copy(block, block->Rect(), {});
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block->Format());
    ctx.Finish();

    //Create a new raster cell from the given block
    TSharedPtr<FOdysseyAnimationCellImageRaster> cell = FOdysseyAnimationCellImageRaster::Create(this, block);
    cell->SetLength(newCellLength);

    FOdysseyAnimationCellsMutator mutator(this, mCellsContainer);
    mutator.SetLength(cellIndex, currentCellLength);
    mutator.Add({ cell }, cellIndex + 1);
    mutator.Commit(); */
}

#undef LOCTEXT_NAMESPACE
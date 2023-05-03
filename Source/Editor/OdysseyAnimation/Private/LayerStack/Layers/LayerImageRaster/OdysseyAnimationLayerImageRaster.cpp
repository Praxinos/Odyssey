// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRasterImageRenderingAbility.h"

#include "OdysseyPixelFormat.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"
#include "OdysseyStyleSet.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"

#define LOCTEXT_NAMESPACE "UOdysseyAnimationLayerImageRaster"

UOdysseyAnimationLayerImageRaster::FOnIsAlphaLockedChanged&
UOdysseyAnimationLayerImageRaster::OnIsAlphaLockedChanged()
{
    static FOnIsAlphaLockedChanged onIsAlphaLockedChanged;
    return onIsAlphaLockedChanged;
}

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

UOdysseyAnimationLayerImageRaster::FOnOffsetChanged&
UOdysseyAnimationLayerImageRaster::OnOffsetChanged()
{
    static FOnOffsetChanged onOffsetChanged;
    return onOffsetChanged;
}

UOdysseyAnimationLayerImageRaster::FOnCellsChanged&
UOdysseyAnimationLayerImageRaster::OnCellsChanged()
{
    static FOnCellsChanged onCellsChanged;
    return onCellsChanged;
}

UOdysseyAnimationLayerImageRaster::~UOdysseyAnimationLayerImageRaster()
{
}

UOdysseyAnimationLayerImageRaster::UOdysseyAnimationLayerImageRaster()
{
	LayerTypeName = LOCTEXT("LayerTypeName", "Raster Image Layer");
    Icon = *FOdysseyStyle::GetBrush( "OdysseyLayerStack.ImageLayer16");
}

void
UOdysseyAnimationLayerImageRaster::OnCreated_Implementation()
{
    AddImageCell();
}

FInt32Range
UOdysseyAnimationLayerImageRaster::GetFrameRange() const
{
    uint32 length = 0;
    for (TSharedPtr<FOdysseyAnimationCell> cell : mCells)
    {
        length += cell->GetLength();
    }
    return FInt32Range::Inclusive(Offset, Offset + length - 1);
}

int
UOdysseyAnimationLayerImageRaster::GetCellsCount() const
{
    return mCells.Num();
}

bool
UOdysseyAnimationLayerImageRaster::GetCellIndexAtFrame(int iFrameIndex, int& oCellIndex, int& oCellFrameIndex) const
{
    if( iFrameIndex < Offset)
        return false;

    int frameIndex = Offset;
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

    uint32 startFrame = Offset;
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
}

void
UOdysseyAnimationLayerImageRaster::AddImageCell()
{
    UOdysseyAnimation* animation = GetAnimation();
    if ( !animation )
        return;

    //TODO: Use iIndex to Insert
    mCells.Add(FOdysseyAnimationCellImageRaster::Create(this, animation->Width(), animation->Height(), animation->Format()));
    
    OnCellsChanged().Broadcast(this);

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if ( imageRenderAbility )
    {
        imageRenderAbility->OnCompositionChanged().Broadcast(imageRenderAbility->GetId());
        imageRenderAbility->OnCompositionCommited().Broadcast(imageRenderAbility->GetId());
    }
}

void
UOdysseyAnimationLayerImageRaster::RemoveCell(int iIndex)
{
    if (iIndex < 0 || iIndex >= mCells.Num())
        return;

    mCells.RemoveAt(iIndex);

    OnCellsChanged().Broadcast(this);

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if ( imageRenderAbility )
    {
        imageRenderAbility->OnCompositionChanged().Broadcast(imageRenderAbility->GetId());
        imageRenderAbility->OnCompositionCommited().Broadcast(imageRenderAbility->GetId());
    }
}

void
UOdysseyAnimationLayerImageRaster::SetCellLength(int iIndex, int iLength)
{
    if (iIndex < 0 || iIndex >= mCells.Num())
        return;

    mCells[iIndex]->SetLength(iLength);
    
    TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if ( imageRenderAbility )
    {
        imageRenderAbility->OnCompositionChanged().Broadcast(imageRenderAbility->GetId());
        imageRenderAbility->OnCompositionCommited().Broadcast(imageRenderAbility->GetId());
    }
}

TArray<TSharedPtr<FOdysseyAnimationCell>>&
UOdysseyAnimationLayerImageRaster::GetCells()
{
    return mCells;
}

TSharedPtr<FOdysseyAnimationCell>
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
}

void
UOdysseyAnimationLayerImageRaster::Merge(const TArray<UOdysseyLayer*>& iLayers)
{
    UOdysseyAnimation* animation = GetAnimation();
    if ( !animation )
        return;

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> thisImageRenderAbility = GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if ( !thisImageRenderAbility )
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

        if ( !layer->HasAbility<IOdysseyAnimationImageRenderingAbility>() )
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

            TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = layer->GetAbility<IOdysseyAnimationImageRenderingAbility>();
            if ( !imageRenderAbility )
                return;

            currentIds.Append(imageRenderAbility->GetComposition(frameIndex));
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
        TSharedPtr<::ULIS::FBlock> ULISBlock = rasterBlock->GetBlock();
        int frame = cellRange.GetLowerBoundValue();
        ::ULIS::FRectI rect = ::ULIS::FRectI::FromXYWH(0, 0, rasterBlock->GetWidth(), rasterBlock->GetHeight());


        TArray<::ULIS::FEvent> lastEvent;
        for (int layerIndex = 0; layerIndex < iLayers.Num(); layerIndex++)
        {
            UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(iLayers[layerIndex]);
            if ( !layer )
                continue;

            TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = layer->GetAbility<IOdysseyAnimationImageRenderingAbility>();
            if ( !imageRenderAbility )
                return;

            lastEvent = imageRenderAbility->RenderOverBlock(ULISBlock, frame, rect, lastEvent);
        }

        cells.Add(cell);
    }

    ctx.Finish();

    mCells = cells;
    Offset = offset;

    OnCellsChanged().Broadcast(this);
    
    thisImageRenderAbility->OnCompositionChanged().Broadcast(thisImageRenderAbility->GetId());
    thisImageRenderAbility->OnCompositionCommited().Broadcast(thisImageRenderAbility->GetId());
}

void
UOdysseyAnimationLayerImageRaster::IsAlphaLockedChanged()
{
    OnIsAlphaLockedChanged().Broadcast(this);
}

void
UOdysseyAnimationLayerImageRaster::OpacityChanged()
{
    OnOpacityChanged().Broadcast(this);

    UOdysseyAnimation* animation = GetAnimation();
    if ( !animation )
        return;

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if ( !imageRenderAbility )
        return;

    //TODO: react to interactive events by not commiting immediately
    ::ULIS::FRectI rect = ::ULIS::FRectI::FromXYWH(0, 0, animation->Width(), animation->Height());
    imageRenderAbility->OnChanged().Broadcast(imageRenderAbility->GetId(), { rect });
    imageRenderAbility->OnCommited().Broadcast(imageRenderAbility->GetId(), { rect });
}

void
UOdysseyAnimationLayerImageRaster::OffsetChanged()
{
    OnOpacityChanged().Broadcast(this);

    UOdysseyAnimation* animation = GetAnimation();
    if ( !animation )
        return;

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if ( !imageRenderAbility )
        return;

    //TODO: react to interactive events by not commiting immediately
    imageRenderAbility->OnCompositionChanged().Broadcast(imageRenderAbility->GetId());
    imageRenderAbility->OnCompositionCommited().Broadcast(imageRenderAbility->GetId());
}

void
UOdysseyAnimationLayerImageRaster::BlendModeChanged()
{
    OnBlendModeChanged().Broadcast(this);

    UOdysseyAnimation* animation = GetAnimation();
    if ( !animation )
        return;

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if ( !imageRenderAbility )
        return;
    {
        ::ULIS::FRectI rect = ::ULIS::FRectI::FromXYWH(0, 0, animation->Width(), animation->Height());
        imageRenderAbility->OnChanged().Broadcast(imageRenderAbility->GetId(), { rect });
        imageRenderAbility->OnCommited().Broadcast(imageRenderAbility->GetId(), { rect });
    }
}

void
UOdysseyAnimationLayerImageRaster::PropertyChanged(const FName& iPropertyName)
{
    Super::PropertyChanged(iPropertyName);

    if (iPropertyName == "BlendMode")
        BlendModeChanged();
    if (iPropertyName == "Opacity")
        OpacityChanged();
    if (iPropertyName == "IsAlphaLocked")
        IsAlphaLockedChanged();
    if (iPropertyName == "Offset")
        OffsetChanged();
}

void
UOdysseyAnimationLayerImageRaster::PostInitProperties()
{
    Super::PostInitProperties();

    SetAbility(MakeShared<FOdysseyAnimationLayerImageRasterImageRenderingAbility>(this));
}

void
UOdysseyAnimationLayerImageRaster::PostLoad()
{
    Super::PostLoad();

    for (TSharedPtr<FOdysseyAnimationCell> cell : mCells)
        cell->PostLoad();
}

void
UOdysseyAnimationLayerImageRaster::PostDuplicate(bool bDuplicateForPIE)
{
    for (TSharedPtr<FOdysseyAnimationCell> cell : mCells)
        cell->PostDuplicate();

    Super::PostDuplicate(bDuplicateForPIE);
}

void
UOdysseyAnimationLayerImageRaster::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);

    //TODO: check undo

    if ( Ar.IsLoading() )
    {
        mCells.Empty();
    }

    int32 numCells = mCells.Num();
    Ar << numCells;
    for ( int i = 0; i < numCells; i++ )
    {
        if ( Ar.IsLoading() )
        {
            FName cellType;
            Ar << cellType;
            TSharedPtr<FOdysseyAnimationCell> cell;
            if ( cellType == FOdysseyAnimationCellImageRaster::StaticType() )
            {
                cell = MakeShared<FOdysseyAnimationCellImageRaster>(this);
            }
            else
            {
                checkf(false, TEXT(""))
            }
            cell->Serialize(Ar);
            mCells.Add(cell);
        }
        else
        {
            FName cellType = mCells[i]->GetType();
            Ar << cellType;
            mCells[i]->Serialize(Ar);
        }
    }
}

void
UOdysseyAnimationLayerImageRaster::OnCellLengthChanged(TSharedRef<FOdysseyAnimationCell> iCell)
{
    if (!mCells.Contains(iCell))
        return;

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if ( imageRenderAbility )
    {
        imageRenderAbility->OnCompositionChanged().Broadcast(imageRenderAbility->GetId());
        imageRenderAbility->OnCompositionCommited().Broadcast(imageRenderAbility->GetId());
    }
}


#undef LOCTEXT_NAMESPACE
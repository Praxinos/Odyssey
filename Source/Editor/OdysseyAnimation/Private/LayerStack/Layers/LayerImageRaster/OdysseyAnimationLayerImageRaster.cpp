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
        imageRenderAbility->OnCompositionChanged().Broadcast(imageRenderAbility->GetId());
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
        imageRenderAbility->OnCompositionChanged().Broadcast(imageRenderAbility->GetId());
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
        imageRenderAbility->OnCompositionChanged().Broadcast(imageRenderAbility->GetId());
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
    //TODO: Merge Cells and Raster Blocks

    /*
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("Layer Image Raster", "Merge Layers"));
#endif

    RasterBlock->Modify();
    
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ULISRasterBlock = RasterBlock->GetBlock();

    //Make tmpblock to merge the layers into
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(ULISRasterBlock->Format());

    TArray<::ULIS::FEvent> lastEvent = {};
    for (UOdysseyLayer* layer : iLayers)
    {
        UOdysseyAnimationLayer* animationLayer = Cast<UOdysseyAnimationLayer>(layer);
        if (!animationLayer)
            continue;

        lastEvent = animationLayer->RenderImage(ULISRasterBlock, ULISRasterBlock->Rect(), ::ULIS::FVec2I(0), lastEvent);
    }
    ctx.Finish();

    RasterBlock->Invalidate({ ULISRasterBlock->Rect() }, false);
    */
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
    {
        //TODO: react to interactive events by not commiting immediately
        ::ULIS::FRectI rect = ::ULIS::FRectI::FromXYWH(0, 0, animation->Width(), animation->Height());
        imageRenderAbility->OnChanged().Broadcast(imageRenderAbility->GetId(), { rect });
        imageRenderAbility->OnCommited().Broadcast(imageRenderAbility->GetId(), { rect });
    }
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
        imageRenderAbility->OnCompositionChanged().Broadcast(imageRenderAbility->GetId());
    }
}


#undef LOCTEXT_NAMESPACE
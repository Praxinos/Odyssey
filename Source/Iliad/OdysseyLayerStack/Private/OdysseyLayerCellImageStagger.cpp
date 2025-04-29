// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyLayerCellImageStagger.h"
#include "OdysseyLayerCellImageStaggerImport.h"
#include "OdysseyLayer.h"

void
UOdysseyLayerCellImageStagger::OldSerialize(FArchive& Ar)
{
    Super::OldSerialize(Ar);
    if( Ar.IsLoading() )
    {
        if (!FOdysseyLayerCellImageStaggerImport::Read( this, Ar ))
        {
            checkf(false, TEXT("Error while loading Stagger Cell"));
        }
    }
}

UOdysseyLayerCell*
UOdysseyLayerCellImageStagger::GetReferenceCellAtFrame(int Frame, bool Recursive) const
{
    int staggerFrame = GetReferenceFrameAtFrame(Frame);
    UOdysseyLayerCell* referenceCell = Cast<UOdysseyLayerCell>(GetLayer()->GetCellAtFrame(staggerFrame));
    if (!referenceCell)
        return nullptr;

    int frame = staggerFrame - referenceCell->GetFrameRange().GetLowerBoundValue();

    while(referenceCell->IsA<UOdysseyLayerCellImageStagger>())
    {
        UOdysseyLayerCellImageStagger* referenceStaggerCell = Cast<UOdysseyLayerCellImageStagger>(referenceCell);
        staggerFrame = referenceStaggerCell->GetReferenceFrameAtFrame(frame);
        referenceCell = Cast<UOdysseyLayerCell>(GetLayer()->GetCellAtFrame(staggerFrame));
        if (!referenceCell)
            return nullptr;

        frame = staggerFrame - referenceCell->GetFrameRange().GetLowerBoundValue();
    }
    return referenceCell;
}

int
UOdysseyLayerCellImageStagger::GetReferenceFrameAtFrame(int iFrameIndex) const
{
    FInt32Range frameRange = GetFrameRange();
    if (frameRange.IsEmpty())
        return INDEX_NONE;

    int cellStartFrame = frameRange.GetLowerBoundValue();
    int frame = INDEX_NONE;
    switch(Behaviour)
    {
        case EOdysseyLayerCellImageStaggerBehaviour::Loop:
        {
            int layerStartFrame = GetLayer()->GetFrameRange().GetLowerBoundValue();
            int startFrame = Reach <= 0 ? layerStartFrame : FMath::Max(layerStartFrame, int(cellStartFrame - Reach));
            if (cellStartFrame - startFrame <= 0)
                return INDEX_NONE;

            int offset = iFrameIndex % (cellStartFrame - startFrame);
            frame = startFrame + offset;
        }
        break;

        case EOdysseyLayerCellImageStaggerBehaviour::PingPong:
        {
            int layerStartFrame = GetLayer()->GetFrameRange().GetLowerBoundValue();
            int startFrame = Reach <= 0 ? layerStartFrame : FMath::Max(layerStartFrame, int(cellStartFrame - Reach));

            if (cellStartFrame - startFrame <= 0)
                return INDEX_NONE;

            //If there is only one frame before the stagger cell,
            //we return that one frame because PingPong needs at least 2 frames to work properly
            if (cellStartFrame - startFrame == 1)
                return startFrame;

            int offset = (iFrameIndex % (cellStartFrame - startFrame - 1));
            int forwardFrame = startFrame + offset + 1;
            int backwardFrame = cellStartFrame - offset - 2;

            int direction = (iFrameIndex / (cellStartFrame - startFrame - 1)) % 2;
            frame = direction == 0 ? backwardFrame : forwardFrame;
        }
        break;

        default:
            break;;
    }

    if (!GetLayer()->HasCellAtFrame(frame))
        return INDEX_NONE;

    return frame;
}

EOdysseyLayerCellImageStaggerBehaviour
UOdysseyLayerCellImageStagger::GetBehaviour() const
{
    return Behaviour;
}

int
UOdysseyLayerCellImageStagger::GetReach() const
{
    return Reach;
}

void
UOdysseyLayerCellImageStagger::SetBehaviour(EOdysseyLayerCellImageStaggerBehaviour Value)
{
    Behaviour = Value;
    RenderingCompositionChanged();
}

void
UOdysseyLayerCellImageStagger::SetReach(int Value, bool IsInteractive)
{
    Reach = Value;
    RenderingCompositionChanged(IsInteractive);
}

TArray<FGuid>
UOdysseyLayerCellImageStagger::GetRenderingComposition(EOdysseyRenderingType iRenderType, int iFrameIndex) const
{
    TArray<FGuid> idComposition = { GetRenderingId() };

    int staggerFrame = GetReferenceFrameAtFrame(iFrameIndex);
    if (staggerFrame == INDEX_NONE)
        return idComposition;

    UOdysseyLayerCell* cell = Cast<UOdysseyLayerCell>(GetLayer()->GetCellAtFrame(staggerFrame));
    if (!cell)
        return idComposition;

    int cellFrame = staggerFrame - cell->GetFrameRange().GetLowerBoundValue();
    idComposition.Append(cell->GetRenderingComposition(EOdysseyRenderingType::Render, cellFrame));

    return idComposition;
}

FIntRect
UOdysseyLayerCellImageStagger::GetDefaultRenderRect() const
{
    return GetLayer()->GetDefaultRenderRect();
}

#if WITH_EDITOR
UOdysseyLayerCell*
UOdysseyLayerCellImageStagger::Break(int Frame, bool bClear)
{
    if (Frame <= 0 || Frame >= Exposure)
        return nullptr;

    UOdysseyLayerCell* cell = GetReferenceCellAtFrame(Frame);
    if (!cell)
        return nullptr;

    UOdysseyLayerCell* newCell = nullptr;
    if (bClear)
    {
        newCell = GetLayer()->AddCell(cell->GetClass(), IndexInLayer + 1);
    }
    else
    {
        newCell = GetLayer()->CopyCell(cell, IndexInLayer + 1);
    }
    newCell->SetExposure(GetExposure() - Frame);
    SetExposure(Frame);

    return newCell;
}
#endif

bool
UOdysseyLayerCellImageStagger::BuildRenderPipeline(
    FFrameNumber iFrame,
    EOdysseyRenderingType iType,
    FOdysseyTextureRenderFunction& oRenderFunction
) const
{
    int staggerFrame = GetReferenceFrameAtFrame(iFrame.Value);
    if ( staggerFrame == INDEX_NONE )
        return false;

    UOdysseyLayerCell* cell = Cast<UOdysseyLayerCell>(GetLayer()->GetCellAtFrame(staggerFrame));
    if ( !cell )
        return false;

    int cellFrame = staggerFrame - cell->GetFrameRange().GetLowerBoundValue();
    return cell->BuildRenderPipeline(cellFrame, iType, oRenderFunction);
}

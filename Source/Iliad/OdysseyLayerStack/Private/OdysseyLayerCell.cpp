// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyLayerCell.h"
#include "OdysseyLayerCellImport.h"
#include "OdysseyLayer.h"
#include "Misc/TransactionObjectEvent.h"

UOdysseyLayer*
UOdysseyLayerCell::GetLayer() const
{
    return Cast<UOdysseyLayer>(GetOuter());
}

void
UOdysseyLayerCell::OldSerialize(FArchive& Ar)
{
    if( !Ar.IsLoading() )
        return;

    if (!FOdysseyLayerCellImport::Read( this, Ar ))
    {
        //Old Style No Chunk Loading
        Ar << Exposure;
    }
}

UOdysseyLayerStack*
UOdysseyLayerCell::GetLayerStack() const
{
    return GetLayer() ? GetLayer()->GetLayerStack() : nullptr;
}

FOdysseyMediaProvider
UOdysseyLayerCell::GetMediaProvider(uint32 iFrameIndex) const
{
    return FOdysseyMediaProvider();
}

FInt32Range
UOdysseyLayerCell::GetFrameRange() const
{
    if (!GetLayer())
        return FInt32Range::Empty();

    const TArray<FInt32Range>& frameRanges = GetLayer()->GetCellsFrameRanges();
    if (IndexInLayer < 0 || IndexInLayer >= frameRanges.Num())
        return FInt32Range::Empty();

    return GetLayer()->GetCellsFrameRanges()[IndexInLayer];
}

FSimpleMulticastDelegate&
UOdysseyLayerCell::OnThumbnailChanged()
{
    return mOnThumbnailChanged;
}

FSimpleMulticastDelegate&
UOdysseyLayerCell::OnThumbnailDirtied()
{
    return mOnThumbnailDirtied;
}

int
UOdysseyLayerCell::GetIndexInLayer() const
{
    return IndexInLayer;
}

int
UOdysseyLayerCell::GetExposure() const
{
    return Exposure;
}

int
UOdysseyLayerCell::GetMark() const
{
    return Mark;
}

void
UOdysseyLayerCell::SetExposure(int Value)
{
    Exposure = Value;

    if (GetLayer())
        GetLayer()->InvalidateCellsFrameRanges();

    RenderingCompositionChanged();
}

void
UOdysseyLayerCell::SetExposureInteractive(int Value)
{
    Exposure = Value;

    if (GetLayer())
        GetLayer()->InvalidateCellsFrameRanges();

    RenderingCompositionChanged(true);
}

void
UOdysseyLayerCell::SetMark(int Value)
{
    Mark = Value;
}

void
UOdysseyLayerCell::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    Super::PostTransacted(iTransactionEvent);

    if ( iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo )
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();
    if (changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyLayerCell, Exposure)))
        RenderingCompositionChanged();
}

UOdysseyLayerCell*
UOdysseyLayerCell::Break(int Frame, bool bClear)
{
    if (Frame <= 0 || Frame >= Exposure)
        return nullptr;

    UOdysseyLayerCell* newCell = nullptr;
    if (bClear)
    {
        newCell = GetLayer()->AddCell(GetClass(), IndexInLayer + 1);
    }
    else
    {
        newCell = GetLayer()->CopyCell(this, IndexInLayer + 1);
    }

    newCell->SetExposure(Exposure - Frame);
    SetExposure(Frame);

    return newCell;
}

void
UOdysseyLayerCell::DirtyThumbnail()
{
    if (ThumbnailIsDirty)
        return;

    ThumbnailIsDirty = true;
    mOnThumbnailDirtied.Broadcast();
}

void
UOdysseyLayerCell::UndirtyThumbnail()
{
    ThumbnailIsDirty = false;
}

bool
UOdysseyLayerCell::IsThumbnailDirty() const
{
    return ThumbnailIsDirty;
}

void
UOdysseyLayerCell::RenderToTexture(FCanvas* iCanvas, FFrameNumber iFrame, const FIntRect& iSrcRect, const FIntRect& iDstRect) const
{
}

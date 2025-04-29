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

#if WITH_EDITOR
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
#endif

UOdysseyLayerStack*
UOdysseyLayerCell::GetLayerStack() const
{
    return GetLayer() ? GetLayer()->GetLayerStack() : nullptr;
}

#if WITH_EDITOR
FOdysseyMediaProvider
UOdysseyLayerCell::GetMediaProvider(uint32 iFrameIndex) const
{
    return FOdysseyMediaProvider();
}
#endif

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

#if WITH_EDITOR
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
#endif

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

#if WITH_EDITOR
int
UOdysseyLayerCell::GetMark() const
{
    return Mark;
}
#endif

void
UOdysseyLayerCell::SetExposure(int Value)
{
    Exposure = Value;

    if (GetLayer())
        GetLayer()->InvalidateCellsFrameRanges();

    RenderingCompositionChanged();
}

#if WITH_EDITOR
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
#endif

TSharedPtr<FOdysseyTextureRenderer>
UOdysseyLayerCell::BuildTextureRenderer(FFrameNumber iFrame, TMap<const IOdysseyTextureRenderingAbility*, FGuid>* iIds) const
{
    return nullptr;
}

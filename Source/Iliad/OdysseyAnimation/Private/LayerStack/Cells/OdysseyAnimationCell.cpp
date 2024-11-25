// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "LayerStack/Cells/OdysseyAnimationCellImport.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "Misc/TransactionObjectEvent.h"
#include "OdysseyAnimation.h"
#include "UObject/OdysseyObjectEditorUtils.h"

UOdysseyAnimationLayer*
UOdysseyAnimationCell::GetLayer() const
{
    return Cast<UOdysseyAnimationLayer>(GetOuter());
}

UOdysseyAnimation*
UOdysseyAnimationCell::GetAnimation() const
{
    return GetLayer() ? GetLayer()->GetAnimation() : nullptr;
}

void
UOdysseyAnimationCell::OldSerialize(FArchive& Ar)
{
    if( !Ar.IsLoading() )
        return;

    if (!FOdysseyAnimationCellImport::Read( this, Ar ))
    {
        //Old Style No Chunk Loading
        Ar << Exposure;
    }
}

UOdysseyAnimationLayerStack*
UOdysseyAnimationCell::GetLayerStack() const
{
    return GetLayer() ? Cast<UOdysseyAnimationLayerStack>(GetLayer()->GetLayerStack()) : nullptr;
}

FOdysseyMediaProvider
UOdysseyAnimationCell::GetMediaProvider(uint32 iFrameIndex) const
{
    return FOdysseyMediaProvider();
}

FInt32Range
UOdysseyAnimationCell::GetFrameRange() const
{
    if (!GetLayer())
        return FInt32Range::Empty();

    return GetLayer()->GetCellsFrameRanges()[IndexInLayer];
}

bool
UOdysseyAnimationCell::IsOutOfPegs() const
{
    return OutOfPegs.Pan != FVector2D(0, 0) || OutOfPegs.Rotation != 0.f || OutOfPegs.Zoom != 100.f;
}

::ULIS::FMat3F
UOdysseyAnimationCell::OutOfPegsTransform() const
{
    UOdysseyAnimation* animation = GetAnimation();
    if (!animation)
        return ::ULIS::FMat3F();

    return ::ULIS::FMat3F::MakeTranslationMatrix(animation->GetWidth() / 2.f, animation->GetHeight() / 2.f)
            * ::ULIS::FMat3F::MakeTranslationMatrix(OutOfPegs.Pan.X, OutOfPegs.Pan.Y)
            * ::ULIS::FMat3F::MakeRotationMatrix(FMath::DegreesToRadians(OutOfPegs.Rotation))
            * ::ULIS::FMat3F::MakeScaleMatrix(OutOfPegs.Zoom / 100.f, OutOfPegs.Zoom / 100.f)
            * ::ULIS::FMat3F::MakeTranslationMatrix(animation->GetWidth() / -2.f, animation->GetHeight() / -2.f);
}

UOdysseyAnimationCell::FOnOutOfPegsChanged&
UOdysseyAnimationCell::OnOutOfPegsChanged()
{
    return mOnOutOfPegsChanged;
}

FSimpleMulticastDelegate&
UOdysseyAnimationCell::OnThumbnailChanged()
{
    return mOnThumbnailChanged;
}

FSimpleMulticastDelegate&
UOdysseyAnimationCell::OnThumbnailDirtied()
{
    return mOnThumbnailDirtied;
}

void
UOdysseyAnimationCell::OutOfPegsChanged(bool iIsInteractive)
{
    mOnOutOfPegsChanged.Broadcast(iIsInteractive);
    ImageRenderingChanged(iIsInteractive);
}

void
UOdysseyAnimationCell::ExposureChanged(bool iIsInteractive)
{
    if (GetLayer())
        GetLayer()->InvalidateCellsFrameRanges();
    ImageRenderingCompositionChanged(iIsInteractive);
}

void
UOdysseyAnimationCell::PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive)
{
    if (iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, OutOfPegs))
    {
        OutOfPegsChanged(iIsInteractive);
    }

    if (iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure))
    {
        ExposureChanged(iIsInteractive);
    }
}

void
UOdysseyAnimationCell::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    PropertyChanged(PropertyChangedEvent.GetPropertyName(), PropertyChangedEvent.GetMemberPropertyName(), PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive);
}

void
UOdysseyAnimationCell::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    Super::PostTransacted(iTransactionEvent);

    if ( iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo )
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();
    for ( const FName& propertyName : changedPropertyNames )
    {
        PropertyChanged(propertyName, propertyName, false);
    }
}

UOdysseyAnimationCell*
UOdysseyAnimationCell::Break(int Frame, bool bClear)
{
    if (Frame <= 0 || Frame >= Exposure)
        return nullptr;

    UOdysseyAnimationCell* newCell = nullptr;
    if (bClear)
    {
        newCell = GetLayer()->AddCell(GetClass(), IndexInLayer + 1);
    }
    else
    {
        newCell = GetLayer()->CopyCell(this, IndexInLayer + 1);
    }

    FOdysseyObjectEditorUtils::SetPropertyValue(newCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), Exposure - Frame);
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), Frame);

    return newCell;
}

void
UOdysseyAnimationCell::DirtyThumbnail()
{
    if (ThumbnailIsDirty)
        return;

    ThumbnailIsDirty = true;
    mOnThumbnailDirtied.Broadcast();
}

void
UOdysseyAnimationCell::UndirtyThumbnail()
{
    ThumbnailIsDirty = false;
}

bool
UOdysseyAnimationCell::IsThumbnailDirty() const
{
    return ThumbnailIsDirty;
}

void
UOdysseyAnimationCell::ExposureBlueprintSetter(int Value)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), Value);
}

void
UOdysseyAnimationCell::MarkBlueprintSetter(int Value)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Mark), Value);
}

void
UOdysseyAnimationCell::OutOfPegsBlueprintSetter(FOdysseyAnimationCellOutOfPegs Value)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, OutOfPegs), Value);
}

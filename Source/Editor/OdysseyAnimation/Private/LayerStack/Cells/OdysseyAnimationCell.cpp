// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "OdysseyAnimation.h"

UOdysseyAnimationLayer*
FOdysseyAnimationCell::GetLayer() const
{
    return Cast<UOdysseyAnimationLayer>(GetOuter());
}

UOdysseyAnimation*
FOdysseyAnimationCell::GetAnimation() const
{
    return GetLayer() ? GetLayer()->GetAnimation() : nullptr;
}

void
FOdysseyAnimationCell::OldSerialize(FArchive& Ar)
{
    if( Ar.IsSaving() )
    {
        FOdysseyAnimationCellExport::Write( this, Ar );
    }

    if( Ar.IsLoading() )
    {
        if (!FOdysseyAnimationCellImport::Read( this, Ar ))
        {
            //Old Style No Chunk Loading
            Ar << Length;
        }
    }
}

UOdysseyAnimationLayerStack*
FOdysseyAnimationCell::GetLayerStack() const
{
    return GetLayer() ? Cast<UOdysseyAnimationLayerStack>(GetLayer()->GetLayerStack()) : nullptr;
}

FOdysseyMediaProvider
FOdysseyAnimationCell::GetMediaProvider(uint32 iFrameIndex) const
{
    return FOdysseyMediaProvider();
}

FInt32Range
FOdysseyAnimationCell::GetFrameRange() const
{
	if (!GetLayer())
		return FInt32Range::Empty();

	return GetLayer()->GetCellsFrameRanges()[IndexInLayer];
}

bool
FOdysseyAnimationCell::IsOutOfPegs() const
{
    return OutOfPegs.Pan != FVector2D(0, 0) || OutOfPegs.Rotation != 0.f || OutOfPegs.Zoom != 1.f;
}

::ULIS::FMat3F
FOdysseyAnimationCell::OutOfPegsTransform() const
{
    UOdysseyAnimation* animation = GetAnimation();
    if (!animation)
        return ::ULIS::FMat3F();

    return ::ULIS::FMat3F::MakeTranslationMatrix(animation->Width() / 2.f, animation->Height() / 2.f)
            * ::ULIS::FMat3F::MakeTranslationMatrix(OutOfPegs.Pan.X, OutOfPegs.Pan.Y)
            * ::ULIS::FMat3F::MakeRotationMatrix(FMath::DegreesToRadians(OutOfPegs.Rotation))
            * ::ULIS::FMat3F::MakeScaleMatrix(OutOfPegs.Zoom, OutOfPegs.Zoom)
            * ::ULIS::FMat3F::MakeTranslationMatrix(animation->Width() / -2.f, animation->Height() / -2.f);
}

FOdysseyAnimationCell::FOnOutOfPegsChanged&
FOdysseyAnimationCell::OnOutOfPegsChanged()
{
    return mOnOutOfPegsChanged;
}

void
FOdysseyAnimationCell::OutOfPegsChanged(bool iIsInteractive)
{
	mOnOutOfPegsChanged.Broadcast(iIsInteractive);
    ImageRenderingChanged(iIsInteractive);
}

void
FOdysseyAnimationCell::LengthChanged(bool iIsInteractive)
{
	if (GetLayer())
		GetLayer()->InvalidateCellFrameRanges();
    ImageRenderingCompositionChanged(iIsInteractive);
}

void
FOdysseyAnimationCell::PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive)
{
	if (iMemberPropertyName == GET_MEMBER_NAME_CHECKED(OutOfPegs))
	{
		OutOfPegsChanged(iIsInteractive);
	}

	if (iMemberPropertyName == GET_MEMBER_NAME_CHECKED(Length))
	{
		LengthChanged(iIsInteractive);
	}
}

void
FOdysseyAnimationCell::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
    
    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName(), PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive);
}

void
FOdysseyAnimationCell::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
	Super::PostTransacted(iTransactionEvent);

    if ( iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo )
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();
    for ( const FName& propertyName : changedPropertyNames )
    {
        PropertyChanged(propertyName, false);
    }
}

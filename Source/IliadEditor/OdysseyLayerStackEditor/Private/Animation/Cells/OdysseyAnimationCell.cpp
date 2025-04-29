// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationCell.h"
#include "OdysseyAnimationLayer.h"
#include "Misc/TransactionObjectEvent.h"
#include "OdysseyAnimation.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "Misc/OdysseyUndoDelegates.h"

UOdysseyAnimation*
UOdysseyAnimationCell::GetAnimation() const
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(GetLayer());
    return layer ? layer->GetAnimation() : nullptr;
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

void
UOdysseyAnimationCell::OutOfPegsChanged(bool iIsInteractive)
{
}

void
UOdysseyAnimationCell::PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive)
{
    if (iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, OutOfPegs))
        OutOfPegsChanged(iIsInteractive);
}

void
UOdysseyAnimationCell::PostPropertyChanged(const FName& iPropertyName, bool iIsInteractive)
{
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, OutOfPegs))
    {
        mOnOutOfPegsChanged.Broadcast(iIsInteractive);
        RenderingChanged(iIsInteractive);
    }
}

void
UOdysseyAnimationCell::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    PropertyChanged(PropertyChangedEvent.GetPropertyName(), PropertyChangedEvent.GetMemberPropertyName(), PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive);
    PostPropertyChanged(PropertyChangedEvent.GetMemberPropertyName(), PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive);
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
        FOdysseyUndoDelegates::Get().OnAfterUndoRedo().AddLambda(
            [this, propertyName](bool iIsRedo)
            {
                PostPropertyChanged(propertyName, false);
            }
        );
    }
}

void
UOdysseyAnimationCell::OutOfPegsBlueprintSetter(FOdysseyAnimationCellOutOfPegs Value)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, OutOfPegs), Value);
}

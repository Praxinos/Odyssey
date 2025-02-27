// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationPlayer.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/Material.h"
#include "Subsystems/UnrealEditorSubsystem.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "Misc/TransactionObjectEvent.h"
#include "Misc/OdysseyUndoDelegates.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OdysseyAnimationComponent)

//////////////////////////////////////////////////////////////////////////
// UOdysseyAnimationComponent

UOdysseyAnimation*
UOdysseyAnimationComponent::GetActiveAnimation() const
{
    if (Mode == EOdysseyAnimationComponentMode::Animation)
    {
        return Animation;
    }

    if (Mode == EOdysseyAnimationComponentMode::Player)
    {
        if (Player)
            return Player->Animation;
    }

    return nullptr;
}

UOdysseyAnimationPlayer*
UOdysseyAnimationComponent::GetActivePlayer() const
{
    if (Mode == EOdysseyAnimationComponentMode::Animation)
    {
        return DefaultPlayer;
    }

    if (Mode == EOdysseyAnimationComponentMode::Player)
    {
        return Player;
    }

    return nullptr;
}

FSimpleMulticastDelegate&
UOdysseyAnimationComponent::OnAnimationChanged()
{
    return mOnAnimationChanged;
}

FSimpleMulticastDelegate&
UOdysseyAnimationComponent::OnPlayerChanged()
{
    return mOnPlayerChanged;
}

FSimpleMulticastDelegate&
UOdysseyAnimationComponent::OnModeChanged()
{
    return mOnModeChanged;
}

void
UOdysseyAnimationComponent::Play()
{
    if (Mode == EOdysseyAnimationComponentMode::Animation)
    {
        DefaultPlayer->Play();
    }

    if (Mode == EOdysseyAnimationComponentMode::Player && Player)
    {
        Player->Play();
    }
}

void
UOdysseyAnimationComponent::Stop()
{
    if (Mode == EOdysseyAnimationComponentMode::Animation)
    {
        DefaultPlayer->Stop();
    }

    if (Mode == EOdysseyAnimationComponentMode::Player && Player)
    {
        Player->Stop();
    }
}

UOdysseyAnimationComponent::UOdysseyAnimationComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
    Mobility = EComponentMobility::Movable;
    SetGenerateOverlapEvents(false);
    bUseDefaultCollision = false;
    SetStaticMesh(LoadObject<UStaticMesh>(this, TEXT("/Engine/BasicShapes/Plane.Plane")));

#if WITH_EDITOR
    bAlwaysAllowTranslucentSelect = true;
#endif

    bCastStaticShadow = false;
    bCastDynamicShadow = true;
    bSelectable = true;

    Material = LoadObject<UMaterial>(this, TEXT("/Odyssey/Animation2D/DefaultAnimationMaterial.DefaultAnimationMaterial"));
    DefaultPlayer = CreateDefaultSubobject<UOdysseyAnimationPlayer>(TEXT("DefaultPlayer"));

    GenerateMaterialInstance();
}

void
UOdysseyAnimationComponent::GenerateMaterialInstance()
{
    MaterialInstance = CreateDefaultSubobject<UMaterialInstanceConstant>(TEXT("MaterialInstance"));
    MaterialInstance->SetParentEditorOnly(Material);
    SetMaterial(0, MaterialInstance);
}

void
UOdysseyAnimationComponent::PostInitProperties()
{
    Super::PostInitProperties();

    if (HasAnyFlags(RF_ClassDefaultObject))
        return;

    DefaultPlayer->OnTextureChanged().AddUObject(this, &UOdysseyAnimationComponent::OnDefaultPlayerTextureChanged);
}

void
UOdysseyAnimationComponent::PostLoad()
{
    Super::PostLoad();

    if (Player)
    {
        PreviousPlayer = Player;
        Player->OnTextureChanged().AddUObject(this, &UOdysseyAnimationComponent::OnPlayerTextureChanged);
    }

    //Reset the callbacks in case DefaultPlayer pointer changed
    DefaultPlayer->OnTextureChanged().RemoveAll(this);
    DefaultPlayer->OnTextureChanged().AddUObject(this, &UOdysseyAnimationComponent::OnDefaultPlayerTextureChanged);

    RefreshMaterialTexture();
}

void
UOdysseyAnimationComponent::PropertyChanged(const FName& iPropertyName)
{
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationComponent, Mode) )
        ModeChanged();
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationComponent, Animation) )
        AnimationChanged();
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationComponent, Player) )
        PlayerChanged();
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationComponent, Material) )
        MaterialChanged();
}

void
UOdysseyAnimationComponent::PostPropertyChanged(const FName& iPropertyName)
{
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationComponent, Mode) )
        mOnModeChanged.Broadcast();
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationComponent, Animation) )
        mOnAnimationChanged.Broadcast();
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationComponent, Player) )
        mOnPlayerChanged.Broadcast();
}

void
UOdysseyAnimationComponent::ModeChanged()
{
    RefreshMaterialTexture();
}

void
UOdysseyAnimationComponent::AnimationChanged()
{
    if (Mode == EOdysseyAnimationComponentMode::Animation)
    {
        FOdysseyObjectEditorUtils::SetPropertyValue(DefaultPlayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationPlayer, Animation), Animation);
        if (Animation)
        {
            float scaleW = (float)Animation->GetWidth() / (float)Animation->GetHeight();
            SetRelativeScale3D(FVector(scaleW, 1, 1));
        }
        MarkRenderStateDirty();
    }
}

void
UOdysseyAnimationComponent::PlayerChanged()
{
    if (Mode == EOdysseyAnimationComponentMode::Player)
    {
        if (PreviousPlayer)
            PreviousPlayer->OnTextureChanged().RemoveAll(this);

        PreviousPlayer = Player;

        if (Player)
        {
            Player->OnTextureChanged().AddUObject(this, &UOdysseyAnimationComponent::OnPlayerTextureChanged);

            UOdysseyAnimation* animation = Player->Animation;
            if (animation)
            {
                float scaleW = (float)animation->GetWidth() / (float)animation->GetHeight();
                SetRelativeScale3D(FVector(scaleW, 1, 1));
            }
        }

        RefreshMaterialTexture();
    }
}

void
UOdysseyAnimationComponent::MaterialChanged()
{
    MaterialInstance->SetParentEditorOnly(Material);
    RefreshMaterialTexture();
}

void
UOdysseyAnimationComponent::OnDefaultPlayerTextureChanged()
{
    RefreshMaterialTexture();
}

void
UOdysseyAnimationComponent::OnPlayerTextureChanged()
{
    RefreshMaterialTexture();
}

void
UOdysseyAnimationComponent::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());
    PostPropertyChanged(PropertyChangedEvent.GetPropertyName());
}

void
UOdysseyAnimationComponent::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    Super::PostTransacted(iTransactionEvent);

    if ( iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo )
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();
    for ( const FName& propertyName : changedPropertyNames )
    {
        PropertyChanged(propertyName);
        FOdysseyUndoDelegates::Get().OnAfterUndoRedo().AddLambda(
            [this, propertyName](bool iIsRedo)
            {
                PostPropertyChanged(propertyName);
            }
        );
    }
}

void
UOdysseyAnimationComponent::PostDuplicate(bool bDuplicateForPIE)
{
    Super::PostDuplicate(bDuplicateForPIE);

    DefaultPlayer->OnTextureChanged().AddUObject(this, &UOdysseyAnimationComponent::OnDefaultPlayerTextureChanged);

    RefreshMaterialTexture();
}

void
UOdysseyAnimationComponent::RefreshMaterialTexture()
{
    if (!MaterialInstance)
        return;

    UOdysseyAnimationPlayer* player = GetActivePlayer();
    if (!player)
        return;

    UTexture* texture = player->GetTexture();
    if (texture)
    {
        MaterialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo("AnimationTexture"), texture);
    }
    else
    {
        //Needed because SetTextureParameterValueEditorOnly does nothing if texture is nullptr
        MaterialInstance->ClearParameterValuesEditorOnly();
    }

    MaterialInstance->PostEditChange();

    FMaterialUpdateContext UpdateContext(FMaterialUpdateContext::EOptions::Default, GMaxRHIShaderPlatform);
    UpdateContext.AddMaterialInstance(MaterialInstance);
    MaterialInstance->MarkPackageDirty();
}

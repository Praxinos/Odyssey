// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationPlayer.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/Material.h"
#include "Subsystems/UnrealEditorSubsystem.h"
#include "Misc/TransactionObjectEvent.h"
#include "Engine/TextureRenderTarget2D.h"

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

EOdysseyAnimationComponentMode
UOdysseyAnimationComponent::GetMode() const
{
    return Mode;
}

void
UOdysseyAnimationComponent::SetAnimation(UOdysseyAnimation* iAnimation)
{
    if (iAnimation == Animation )
        return;

    Animation = iAnimation;
    AnimationChanged();
}

void
UOdysseyAnimationComponent::SetPlayer(UOdysseyAnimationPlayer* iPlayer)
{
    if (iPlayer == Player )
        return;

    Player = iPlayer;
}

void
UOdysseyAnimationComponent::SetMode(EOdysseyAnimationComponentMode iMode)
{
    if (iMode == Mode )
        return;

    Mode = iMode;
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

    MaterialInstance = CreateDefaultSubobject<UMaterialInstanceConstant>(TEXT("MaterialInstance"));
    MaterialInstance->SetParentEditorOnly(Material);
    SetMaterial(0, MaterialInstance);
    RefreshMaterialTexture();
}

void
UOdysseyAnimationComponent::PostLoad()
{
    Super::PostLoad();

    SetMaterial(0, MaterialInstance);
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
UOdysseyAnimationComponent::ModeChanged()
{
    RefreshMaterialTexture();
}

void
UOdysseyAnimationComponent::AnimationChanged()
{
    if (Mode == EOdysseyAnimationComponentMode::Animation)
    {
        DefaultPlayer->SetAnimation(Animation);
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
        if (Player)
        {
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
UOdysseyAnimationComponent::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());
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
    }
}

void
UOdysseyAnimationComponent::PostDuplicate(bool bDuplicateForPIE)
{
    Super::PostDuplicate(bDuplicateForPIE);

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

    UTextureRenderTarget2D* renderTarget = player->GetRenderTarget();
    if (renderTarget)
    {
        MaterialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo("AnimationTexture"), renderTarget);
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

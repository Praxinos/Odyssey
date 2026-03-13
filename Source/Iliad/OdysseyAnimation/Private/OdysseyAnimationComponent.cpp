// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationPlayer.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/Material.h"
#include "Misc/TransactionObjectEvent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "MaterialShared.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OdysseyAnimationComponent)

//////////////////////////////////////////////////////////////////////////
// UOdysseyAnimationComponent

UOdysseyAnimation*
UOdysseyAnimationComponent::GetAnimation() const
{
    if (Mode == EOdysseyAnimationComponentMode::Animation)
    {
        return Animation;
    }

    if (Mode == EOdysseyAnimationComponentMode::Player)
    {
        if (Player)
            return Player->GetAnimation();
    }

    return nullptr;
}

UOdysseyAnimationPlayer*
UOdysseyAnimationComponent::GetPlayer() const
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

UOdysseyAnimationComponent::UOdysseyAnimationComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
    Mobility = EComponentMobility::Movable;
    SetGenerateOverlapEvents(false);
    bUseDefaultCollision = false;

#if WITH_EDITOR
    bAlwaysAllowTranslucentSelect = true;
#endif

    bCastStaticShadow = false;
    bCastDynamicShadow = true;
    bSelectable = true;
}

void
UOdysseyAnimationComponent::Initialize()
{
    SetStaticMesh( LoadObject<UStaticMesh>( this, TEXT( "/Odyssey/Meshes/S_1_Unit_Plane.S_1_Unit_Plane" ) ) );
    SetAnimationMaterial(LoadObject<UMaterial>(this, TEXT("/Odyssey/Animation2D/DefaultAnimationMaterial.DefaultAnimationMaterial")));
}

void
UOdysseyAnimationComponent::InitializeFromAnimation(UOdysseyAnimation* iAnimation)
{
    Initialize();
    SetMode(EOdysseyAnimationComponentMode::Animation);
    SetAnimation(iAnimation);
    SetPlayer(nullptr);
}


void
UOdysseyAnimationComponent::InitializeFromPlayer(UOdysseyAnimationPlayer* iPlayer)
{
    Initialize();
    SetMode(EOdysseyAnimationComponentMode::Animation);
    SetAnimation(nullptr);
    SetPlayer(iPlayer);
}

void
UOdysseyAnimationComponent::PostInitProperties()
{
    Super::PostInitProperties();

    if (HasAnyFlags(RF_ClassDefaultObject))
        return;

    if (!DefaultPlayer)
        DefaultPlayer = NewObject<UOdysseyAnimationPlayer>(this, TEXT("DefaultPlayer"), RF_Public | RF_Transactional);
}

void
UOdysseyAnimationComponent::PostLoad()
{
    Super::PostLoad();

    if( !GetStaticMesh() )
    {
        InitializeFromAnimation( Animation );
    }

    if( GetStaticMesh()->GetPathName() == TEXT( "/Engine/BasicShapes/Plane.Plane" ) ) // Old mesh (deprecated) used for animation actor component
    {
        SetStaticMesh( LoadObject<UStaticMesh>( this, TEXT( "/Odyssey/Meshes/S_1_Unit_Plane.S_1_Unit_Plane" ) ) );
        FVector old_scale = GetRelativeScale3D();
        FVector new_scale = old_scale * FVector( 100.f, 100.f, 1.f ); // Because Plane.Plane mesh is 100x100 and S_1_Unit_Plane.S_1_Unit_Plane is 1x1
        SetRelativeScale3D( new_scale );
    }

    if (!MaterialInstance)
    {
        CreateMaterialInstance();
        RefreshMaterialTexture();
    }

    if (Player)
    {
        PreviousPlayer = Player;
        Player->OnAnimationChanged().RemoveAll(this);
        Player->OnAnimationChanged().AddUObject(this, &UOdysseyAnimationComponent::OnPlayerAnimationChanged);
    }
}

void
UOdysseyAnimationComponent::PostEditImport()
{
    Super::PostEditImport();

    // Make sure to update the material after duplicating this component
    // When duplicating an actor PostDuplicate() is not called on its components
    CreateMaterialInstance();
    RefreshMaterialTexture();
}

void
UOdysseyAnimationComponent::CreateMaterialInstance()
{
    EmptyOverrideMaterials();
    if (!Material)
    {
        MaterialInstance = nullptr;
        return;
    }

    MaterialInstance = UMaterialInstanceDynamic::Create(Material, GetTransientPackage());
    MaterialInstance->SetFlags(MaterialInstance->GetFlags() | RF_Public);
    UStaticMeshComponent::SetMaterial(0, MaterialInstance);
}

void
UOdysseyAnimationComponent::ModeChanged()
{
    RefreshMaterialTexture();
}

void
UOdysseyAnimationComponent::RescaleToMatchAnimation(UOdysseyAnimation* iAnimation)
{
    if (!iAnimation)
        return;

    float scaleW = (float)iAnimation->GetWidth() / (float)iAnimation->GetHeight();
    SetRelativeScale3D(FVector(scaleW * 100.f, 1 * 100.f, 1)); // *100: to have something more visible than 1x1
    MarkRenderStateDirty();
}

void
UOdysseyAnimationComponent::AnimationChanged()
{
    if (Mode == EOdysseyAnimationComponentMode::Animation)
    {
        DefaultPlayer->SetAnimation(Animation);
        RescaleToMatchAnimation(Animation);
    }
}

void
UOdysseyAnimationComponent::OnPlayerAnimationChanged()
{
    if (Mode == EOdysseyAnimationComponentMode::Player)
    {
        if (Player)
            RescaleToMatchAnimation(Player->GetAnimation());
    }
}

void
UOdysseyAnimationComponent::PlayerChanged()
{
    if (PreviousPlayer)
        PreviousPlayer->OnAnimationChanged().RemoveAll(this);

    PreviousPlayer = Player;

    if (Player)
    {
        Player->OnAnimationChanged().RemoveAll(this);
        Player->OnAnimationChanged().AddUObject(this, &UOdysseyAnimationComponent::OnPlayerAnimationChanged);
        RescaleToMatchAnimation(Player->GetAnimation());
    }
    RefreshMaterialTexture();
}

UMaterialInterface*
UOdysseyAnimationComponent::GetAnimationMaterial() const
{
    return Material;
}

void
UOdysseyAnimationComponent::SetAnimationMaterial(UMaterialInterface* iMaterial)
{
    Material = iMaterial;
    MaterialChanged();
}

void
UOdysseyAnimationComponent::MaterialChanged()
{
    MaterialInstance->Parent = Material;
    MaterialInstance->PostEditChange();
}

void
UOdysseyAnimationComponent::LODGroupChanged()
{
    DefaultPlayer->SetLODGroup(LODGroup);
}

#if WITH_EDITOR
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
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationComponent, LODGroup) )
        LODGroupChanged();
}

void
UOdysseyAnimationComponent::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    bool isInteractive = PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive;
    if (!isInteractive)
    {
        PropertyChanged(PropertyChangedEvent.GetPropertyName());
    }

    Super::PostEditChangeProperty(PropertyChangedEvent);
}

void
UOdysseyAnimationComponent::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
    //PostEditChangeChainProperty is called on the Archetype Component Object before PostEditChangeProperty
    //This fixes incoherences like  UOdysseyAnimationComponent::Animation != UOdysseyAnimationPlayer::Animation
    bool isInteractive = PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive;
    if (!isInteractive)
    {
        PropertyChanged(PropertyChangedEvent.GetPropertyName());
    }

    Super::PostEditChangeChainProperty(PropertyChangedEvent);
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
#endif

void
UOdysseyAnimationComponent::RefreshMaterialTexture()
{
    if (!MaterialInstance)
    {
        CreateMaterialInstance();
        if (!MaterialInstance)
            return;
    }

    UOdysseyAnimationPlayer* player = GetPlayer();
    if (!player)
        return;

    UTextureRenderTarget2D* renderTarget = player->GetRenderTarget();
    if (renderTarget)
    {
        UTexture* prevTexture = MaterialInstance->K2_GetTextureParameterValue("AnimationTexture");

        MaterialInstance->SetTextureParameterValue("AnimationTexture", renderTarget);
    }
    else
    {
        //Needed because SetTextureParameterValue does nothing if texture is nullptr
        MaterialInstance->ClearParameterValues();
    }
    MaterialInstance->EnsureIsComplete();
}

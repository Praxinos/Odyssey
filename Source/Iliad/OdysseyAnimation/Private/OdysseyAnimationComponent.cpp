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
    PlayerChanged();
}

void
UOdysseyAnimationComponent::SetMode(EOdysseyAnimationComponentMode iMode)
{
    if (iMode == Mode )
        return;

    Mode = iMode;
    ModeChanged();
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

    DefaultPlayer = CreateDefaultSubobject<UOdysseyAnimationPlayer>(TEXT("DefaultPlayer"));
    SetStaticMesh( LoadObject<UStaticMesh>( this, TEXT( "/Odyssey/Meshes/S_1_Unit_Plane.S_1_Unit_Plane" ) ) );
    Material = LoadObject<UMaterial>(this, TEXT("/Odyssey/Animation2D/DefaultAnimationMaterial.DefaultAnimationMaterial"));

    DefaultPlayer->OnRenderTargetChanged().AddUObject(this, &UOdysseyAnimationComponent::OnDefaultPlayerRenderTargetChanged);
}

void
UOdysseyAnimationComponent::InitializeFromAnimation(UOdysseyAnimation* iAnimation)
{
    SetMode(EOdysseyAnimationComponentMode::Animation);
    SetAnimation(iAnimation);
    SetPlayer(nullptr);
}


void
UOdysseyAnimationComponent::InitializeFromPlayer(UOdysseyAnimationPlayer* iPlayer)
{
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

    //bNeedUpdateMaterialInstance
}

void
UOdysseyAnimationComponent::PostReinitProperties()
{
    Super::PostReinitProperties();

    if (HasAnyFlags(RF_ClassDefaultObject))
        return;

    //UpdateMaterialInstance();
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

    //UpdateMaterialInstance();

    if (Player)
    {
        PreviousPlayer = Player;
        Player->OnAnimationChanged().RemoveAll(this);
        Player->OnAnimationChanged().AddUObject(this, &UOdysseyAnimationComponent::OnPlayerAnimationChanged);
    }
}

void
UOdysseyAnimationComponent::PostDuplicate(bool bDuplicateForPIE)
{
    Super::PostDuplicate(bDuplicateForPIE);

    // Make sure to update the material after duplicating this component
    // When duplicating an actor PostDuplicate() is not called on its components
    //UpdateMaterialInstance();
}

void
UOdysseyAnimationComponent::PostEditImport()
{
    Super::PostEditImport();

    // Make sure to update the material after duplicating this component
    // When duplicating an actor PostDuplicate() is not called on its components
    //UpdateMaterialInstance();
}

TStructOnScope<FActorComponentInstanceData>
UOdysseyAnimationComponent::GetComponentInstanceData() const
{
    return MakeStructOnScope<FActorComponentInstanceData, FOdysseyAnimationComponentInstanceData>(this);
}

void
UOdysseyAnimationComponent::ApplyComponentInstanceData(FOdysseyAnimationComponentInstanceData* ComponentInstanceData)
{
    if (!ComponentInstanceData)
        return;

    Material = ComponentInstanceData->Material;
    Mode = ComponentInstanceData->Mode;
    Player = ComponentInstanceData->Player;
    Animation = ComponentInstanceData->Animation;
    LODGroup = ComponentInstanceData->LODGroup;

    //Here we don't call AnimationChanged() and similar "*Changed()" functions
    //to avoid calling RescaleToMatchAnimation()
    //The correct Transform is already applied by UStaticMeshComponent::ApplyComponentInstanceData()
    //Calling RescaleToMatchAnimation() here would prevent change the component scale in the details Panel

    DefaultPlayer->SetLODGroup(LODGroup);
    DefaultPlayer->SetAnimation(Animation);

    if (PreviousPlayer)
        PreviousPlayer->OnAnimationChanged().RemoveAll(this);

    PreviousPlayer = Player;

    if (Player)
    {
        Player->OnAnimationChanged().RemoveAll(this);
        Player->OnAnimationChanged().AddUObject(this, &UOdysseyAnimationComponent::OnPlayerAnimationChanged);
    }

    UpdateMaterialInstance();

    //TODO: Copy important parameters to DefaultPlayer
}

void
UOdysseyAnimationComponent::ModeChanged()
{
    UpdateMaterialInstance();
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
UOdysseyAnimationComponent::OnDefaultPlayerRenderTargetChanged()
{
    //UpdateMaterialInstance();
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
    UpdateMaterialInstance();
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
    UpdateMaterialInstance();
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
UOdysseyAnimationComponent::UpdateMaterialInstance()
{
    if (!Material)
    {
        UStaticMeshComponent::SetMaterial(0, nullptr);
        return;
    }

    UMaterialInstanceDynamic* materialInstance = Cast<UMaterialInstanceDynamic>(UStaticMeshComponent::GetMaterial(0));
    if (!materialInstance || materialInstance->GetOuter() != this)
    {
        //A material instance is needed to be either {transient} or {public + standalone} (saved in its own asset)
        //If not, we get an error when saving the object containing it (here when we save the actor containing the component)
        //Here the material instance is not saved in its own asset so we set it to be transient
        //And we also define its Outer to be this, because it is owned by the Component even if it's Transient
        materialInstance = UMaterialInstanceDynamic::Create(Material, this);
        materialInstance->SetFlags(RF_Transient);
        //materialInstance->SetFlags(RF_Public);
        UStaticMeshComponent::SetMaterial(0, materialInstance);
    }

    if (materialInstance->Parent != Material)
    {
        materialInstance->Parent = Material;
        materialInstance->PostEditChange();
    }

    UOdysseyAnimationPlayer* player = GetPlayer();
    if (!player)
    {
        materialInstance->ClearParameterValues();
        return;
    }

    UTextureRenderTarget2D* renderTarget = player->GetRenderTarget();
    if (!renderTarget)
    {
        //Needed because SetTextureParameterValue does nothing if texture is nullptr
        materialInstance->ClearParameterValues();
        return;
    }

    materialInstance->SetTextureParameterValue("AnimationTexture", renderTarget);
}

void
UOdysseyAnimationComponent::OnComponentCreated()
{
    Super::OnComponentCreated();
    UpdateMaterialInstance();
}

//======================================================================================

//NEEDED BY FOdysseyStaticMeshComponentInstanceDataPATCH
#include "StaticMeshComponentLODInfo.h"

FOdysseyStaticMeshComponentInstanceDataPATCH::FOdysseyStaticMeshComponentInstanceDataPATCH(const UStaticMeshComponent* SourceComponent)
    : FPrimitiveComponentInstanceData(SourceComponent)
    , StaticMesh(SourceComponent->GetStaticMesh())
{
    for (const FStaticMeshComponentLODInfo& LODDataEntry : SourceComponent->LODData)
    {
        CachedStaticLighting.Add(LODDataEntry.OriginalMapBuildDataId);
        CachedStaticLighting.Add(LODDataEntry.MapBuildDataId);
    }

    // Backup the texture streaming data.
    StreamingTextureData = SourceComponent->StreamingTextureData;
#if WITH_EDITORONLY_DATA
    MaterialStreamingRelativeBoxes = SourceComponent->MaterialStreamingRelativeBoxes;
#endif

    // Cache instance vertex colors
    for (int32 LODIndex = 0; LODIndex < SourceComponent->LODData.Num(); ++LODIndex)
    {
        const FStaticMeshComponentLODInfo& LODInfo = SourceComponent->LODData[LODIndex];

        // Note: we don't need to check LODInfo.PaintedVertices here since it's not always required.
        if (LODInfo.OverrideVertexColors && LODInfo.OverrideVertexColors->GetNumVertices() > 0)
        {
            AddVertexColorData(LODInfo, LODIndex);
        }
    }
}

bool FOdysseyStaticMeshComponentInstanceDataPATCH::ContainsData() const
{
    return Super::ContainsData()
        || StreamingTextureData.Num() > 0
        || CachedStaticLighting.Num() > 0
#if WITH_EDITORONLY_DATA
        || MaterialStreamingRelativeBoxes.Num() > 0
#endif
        || StaticMesh != nullptr;
}

void FOdysseyStaticMeshComponentInstanceDataPATCH::ApplyToComponent(UActorComponent* Component, const ECacheApplyPhase CacheApplyPhase)
{
    Super::ApplyToComponent(Component, CacheApplyPhase);
    if (CacheApplyPhase == ECacheApplyPhase::PostUserConstructionScript)
    {
        CastChecked<UStaticMeshComponent>(Component)->ApplyComponentInstanceData(reinterpret_cast<FStaticMeshComponentInstanceData*>(this));
    }
}

void FOdysseyStaticMeshComponentInstanceDataPATCH::AddReferencedObjects(FReferenceCollector& Collector)
{
    Super::AddReferencedObjects(Collector);
    Collector.AddReferencedObject(StaticMesh);
}

/** Add vertex color data for a specified LOD before RerunConstructionScripts is called */
void FOdysseyStaticMeshComponentInstanceDataPATCH::AddVertexColorData(const struct FStaticMeshComponentLODInfo& LODInfo, uint32 LODIndex)
{
    if (VertexColorLODs.Num() <= (int32)LODIndex)
    {
        VertexColorLODs.SetNum(LODIndex + 1);
    }
    FStaticMeshVertexColorLODData& VertexColorData = VertexColorLODs[LODIndex];
    VertexColorData.LODIndex = LODIndex;
    VertexColorData.PaintedVertices = LODInfo.PaintedVertices;
    LODInfo.OverrideVertexColors->GetVertexColors(VertexColorData.VertexBufferColors);
}

/** Re-apply vertex color data after RerunConstructionScripts is called */
bool FOdysseyStaticMeshComponentInstanceDataPATCH::ApplyVertexColorData(UStaticMeshComponent* StaticMeshComponent) const
{
    bool bAppliedAnyData = false;

    if (StaticMeshComponent != NULL)
    {
        StaticMeshComponent->SetLODDataCount(VertexColorLODs.Num(), StaticMeshComponent->LODData.Num());

        // Its possible that we have recreated LODs in SetLODDataCount that existed prior
        // to reconstruction, but not *rebuilt* them because static lighting usage was clobbered
        // by the construction script. In this case we should recover the GUIDs we had before
        // so we dont end up creating new (non-deterministic) data
        for(int32 LODIndex = 0; LODIndex < StaticMeshComponent->LODData.Num(); ++LODIndex)
        {
            FStaticMeshComponentLODInfo& LODInfo = StaticMeshComponent->LODData[LODIndex];
            if(CachedStaticLighting.IsValidIndex((LODIndex*2)+1))
            {
                LODInfo.OriginalMapBuildDataId = CachedStaticLighting[(LODIndex*2)];
                LODInfo.MapBuildDataId = CachedStaticLighting[(LODIndex*2)+1];
            }
        }

        for (int32 LODDataIndex = 0; LODDataIndex < VertexColorLODs.Num(); ++LODDataIndex)
        {
            const FStaticMeshVertexColorLODData& VertexColorLODData = VertexColorLODs[LODDataIndex];
            uint32 LODIndex = VertexColorLODData.LODIndex;

            if (StaticMeshComponent->LODData.IsValidIndex(LODIndex))
            {
                FStaticMeshComponentLODInfo& LODInfo = StaticMeshComponent->LODData[LODIndex];
                // this component could have been constructed from a template
                // that had its own vert color overrides; so before we apply
                // the instance's color data, we need to clear the old
                // vert colors (so we can properly call InitFromColorArray())
                StaticMeshComponent->RemoveInstanceVertexColorsFromLOD(LODIndex);
                // may not be null at the start (could have been initialized
                // from a  component template with vert coloring), but should
                // be null at this point, after RemoveInstanceVertexColorsFromLOD()
                if (LODInfo.OverrideVertexColors == NULL && VertexColorLODData.VertexBufferColors.Num() > 0)
                {
                    LODInfo.PaintedVertices = VertexColorLODData.PaintedVertices;

                    LODInfo.OverrideVertexColors = new FColorVertexBuffer;
                    LODInfo.OverrideVertexColors->InitFromColorArray(VertexColorLODData.VertexBufferColors);

                    check(LODInfo.OverrideVertexColors->GetStride() > 0);
                    BeginInitResource(LODInfo.OverrideVertexColors);
                    bAppliedAnyData = true;
                }
            }
        }
    }

    return bAppliedAnyData;
}

//=======================================================================================

FOdysseyAnimationComponentInstanceData::FOdysseyAnimationComponentInstanceData(const UOdysseyAnimationComponent* SourceComponent)
    : FOdysseyStaticMeshComponentInstanceDataPATCH(SourceComponent)
{
    Material = SourceComponent->Material;
    Mode = SourceComponent->Mode;
    Animation = SourceComponent->Animation;
    Player = SourceComponent->Player;
    LODGroup = SourceComponent->LODGroup;
    DefaultPlayer = SourceComponent->DefaultPlayer;
}

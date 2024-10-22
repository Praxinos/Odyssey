// Copyright Epic Games, Inc. All Rights Reserved.

#include "OdysseyAnimationActor.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationPlayer.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Subsystems/UnrealEditorSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OdysseyAnimationActor)

//////////////////////////////////////////////////////////////////////////
// AOdysseyAnimationActor

UOdysseyAnimationPlayer*
AOdysseyAnimationActor::GetActivePlayer() const
{
	if (Mode == EOdysseyAnimationActorMode::Animation)
	{
		return DefaultPlayer;
	}

	if (Mode == EOdysseyAnimationActorMode::Player)
	{
		return Player;
	}

	return nullptr;
}

void
AOdysseyAnimationActor::FaceCamera()
{
	UUnrealEditorSubsystem* UnrealEditorSubsystem = GEditor->GetEditorSubsystem<UUnrealEditorSubsystem>();
	FVector cameraLocation;
	FRotator cameraRotation;
	if (!UnrealEditorSubsystem->GetLevelViewportCameraInfo(cameraLocation, cameraRotation))
		return;

    //-

    FRotator plane_rotator = cameraRotation;

    //---

	SetActorRotation( plane_rotator );
}

void
AOdysseyAnimationActor::MoveInFrontOfCamera()
{
	UUnrealEditorSubsystem* UnrealEditorSubsystem = GEditor->GetEditorSubsystem<UUnrealEditorSubsystem>();
	FVector cameraLocation;
	FRotator cameraRotation;
	if (!UnrealEditorSubsystem->GetLevelViewportCameraInfo(cameraLocation, cameraRotation))
		return;

    //-

    float FocusDistance = 200;
    FVector plane_location = cameraLocation + cameraRotation.Vector() * FocusDistance;
    FRotator plane_rotator = cameraRotation;

    //---

    SetActorLocation( plane_location );
	SetActorRotation( plane_rotator );
}

void
AOdysseyAnimationActor::Play()
{
	if (Mode == EOdysseyAnimationActorMode::Animation)
	{
		DefaultPlayer->Play();
	}

	if (Mode == EOdysseyAnimationActorMode::Player && Player)
	{
		Player->Play();
	}
}

void
AOdysseyAnimationActor::Stop()
{
	if (Mode == EOdysseyAnimationActorMode::Animation)
	{
		DefaultPlayer->Stop();
	}

	if (Mode == EOdysseyAnimationActorMode::Player && Player)
	{
		Player->Stop();
	}
}

AOdysseyAnimationActor::AOdysseyAnimationActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	StaticMeshComponent->Mobility = EComponentMobility::Movable;
	StaticMeshComponent->SetGenerateOverlapEvents(false);
	StaticMeshComponent->bUseDefaultCollision = false;
	StaticMeshComponent->SetStaticMesh(LoadObject<UStaticMesh>(this, TEXT("/Engine/BasicShapes/Plane.Plane")));
	StaticMeshComponent->SetRelativeRotation(FRotator(0, 90, 90));
	

#if WITH_EDITOR
	StaticMeshComponent->bAlwaysAllowTranslucentSelect = true;
#endif

	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->bCastStaticShadow = false;
	StaticMeshComponent->bCastDynamicShadow = false;
	StaticMeshComponent->bSelectable = true;

	UMaterial* material = LoadObject<UMaterial>(this, TEXT("/Iliad/Animation2D/DefaultAnimationMaterial.DefaultAnimationMaterial"));
	UMaterialInstanceConstant* materialInstance = CreateDefaultSubobject<UMaterialInstanceConstant>(TEXT("DefaultAnimationMaterialInstance"));
	materialInstance->SetParentEditorOnly(material);
	StaticMeshComponent->SetMaterial(0, materialInstance);

	DefaultPlayer = CreateDefaultSubobject<UOdysseyAnimationPlayer>(TEXT("DefaultPlayer"));
}

void
AOdysseyAnimationActor::PostInitProperties()
{
	Super::PostInitProperties();

	if (HasAnyFlags(RF_ClassDefaultObject))
        return;
	
	//DefaultPlayer = NewObject<UOdysseyAnimationPlayer>(this, "DefaultPlayer", RF_Public | RF_Transactional);
	DefaultPlayer->OnTextureChanged().AddUObject(this, &AOdysseyAnimationActor::OnDefaultPlayerTextureChanged);
}

void
AOdysseyAnimationActor::PostLoad()
{
	Super::PostLoad();

	if (Player)
	{
		PreviousPlayer = Player;
		Player->OnTextureChanged().AddUObject(this, &AOdysseyAnimationActor::OnPlayerTextureChanged);
	}

	RefreshMaterialTexture();
}


#if WITH_EDITOR
bool AOdysseyAnimationActor::GetReferencedContentObjects(TArray<UObject*>& Objects) const
{
	Super::GetReferencedContentObjects(Objects);
	switch(Mode)
	{
		case EOdysseyAnimationActorMode::Animation : Objects.Add(Animation); break;
		case EOdysseyAnimationActorMode::Player : Objects.Add(Player); break;
	}
	return true;
}
#endif



void
AOdysseyAnimationActor::PropertyChanged(const FName& iPropertyName)
{
	if ( iPropertyName == GET_MEMBER_NAME_CHECKED(AOdysseyAnimationActor, Mode) )
        ModeChanged();
	if ( iPropertyName == GET_MEMBER_NAME_CHECKED(AOdysseyAnimationActor, Animation) )
        AnimationChanged();
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(AOdysseyAnimationActor, Player) )
        PlayerChanged();
}

void
AOdysseyAnimationActor::ModeChanged()
{
	RefreshMaterialTexture();
}

void
AOdysseyAnimationActor::AnimationChanged()
{
	if (Mode != EOdysseyAnimationActorMode::Animation)
		return;

	FOdysseyObjectEditorUtils::SetPropertyValue(DefaultPlayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationPlayer, Animation), Animation);

	if (Animation)
	{
		float scaleW = (float)Animation->GetWidth() / (float)Animation->GetHeight();
		StaticMeshComponent->SetRelativeScale3D(FVector(scaleW, 1, 1));
	}
}

void
AOdysseyAnimationActor::PlayerChanged()
{
	if (Mode != EOdysseyAnimationActorMode::Player)
		return;
	
	if (PreviousPlayer)
		PreviousPlayer->OnTextureChanged().RemoveAll(this);
	 
	PreviousPlayer = Player;

	if (Player)
	{
		Player->OnTextureChanged().AddUObject(this, &AOdysseyAnimationActor::OnPlayerTextureChanged);

		UOdysseyAnimation* animation = Player->Animation;
		if (animation)
		{
			float scaleW = (float)animation->GetWidth() / (float)animation->GetHeight();
			StaticMeshComponent->SetRelativeScale3D(FVector(scaleW, 1, 1));
		}
	}

	RefreshMaterialTexture();
}

void
AOdysseyAnimationActor::OnDefaultPlayerTextureChanged()
{		
	RefreshMaterialTexture();
}

void
AOdysseyAnimationActor::OnPlayerTextureChanged()
{		
	RefreshMaterialTexture();
}

void
AOdysseyAnimationActor::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());
}

void
AOdysseyAnimationActor::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
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
AOdysseyAnimationActor::RefreshMaterialTexture()
{
	UMaterialInstanceConstant* materialInstance = Cast<UMaterialInstanceConstant>(StaticMeshComponent->GetMaterial(0));
	if (!materialInstance)
		return;

	if (Mode == EOdysseyAnimationActorMode::Player)
	{
		materialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo("AnimationTexture"), Player->GetTexture());
	}
	else
	{
		materialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo("AnimationTexture"), DefaultPlayer->GetTexture());
	}
}
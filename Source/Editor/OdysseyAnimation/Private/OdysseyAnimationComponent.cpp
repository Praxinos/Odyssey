// Copyright Epic Games, Inc. All Rights Reserved.

#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationPlayer.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Subsystems/UnrealEditorSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OdysseyAnimationComponent)

//////////////////////////////////////////////////////////////////////////
// UOdysseyAnimationComponent

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
	bCastDynamicShadow = false;
	bSelectable = true;

	UMaterial* material = LoadObject<UMaterial>(this, TEXT("/Iliad/Animation2D/DefaultAnimationMaterial.DefaultAnimationMaterial"));
	UMaterialInstanceConstant* materialInstance = CreateDefaultSubobject<UMaterialInstanceConstant>(TEXT("DefaultAnimationMaterialInstance"));
	materialInstance->SetParentEditorOnly(material);
	SetMaterial(0, materialInstance);

	DefaultPlayer = CreateDefaultSubobject<UOdysseyAnimationPlayer>(TEXT("DefaultPlayer"));
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
}

void
UOdysseyAnimationComponent::ModeChanged()
{
	RefreshMaterialTexture();
}

void
UOdysseyAnimationComponent::AnimationChanged()
{
	if (Mode != EOdysseyAnimationComponentMode::Animation)
		return;

	FOdysseyObjectEditorUtils::SetPropertyValue(DefaultPlayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationPlayer, Animation), Animation);

	if (Animation)
	{
		float scaleW = (float)Animation->GetWidth() / (float)Animation->GetHeight();
		SetRelativeScale3D(FVector(scaleW, 1, 1));
		MarkRenderStateDirty();
	}
}

void
UOdysseyAnimationComponent::PlayerChanged()
{
	if (Mode != EOdysseyAnimationComponentMode::Player)
		return;
	
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

	DefaultPlayer->OnTextureChanged().AddUObject(this, &UOdysseyAnimationComponent::OnDefaultPlayerTextureChanged);
	
	RefreshMaterialTexture();
}

void
UOdysseyAnimationComponent::RefreshMaterialTexture()
{
	UMaterialInstanceConstant* materialInstance = Cast<UMaterialInstanceConstant>(GetMaterial(0));
	if (!materialInstance)
		return;

	UOdysseyAnimationPlayer* player = GetActivePlayer();
	UTexture* texture = player->GetTexture();
	
	materialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo("AnimationTexture"), texture);
	materialInstance->PostEditChange();

	FMaterialUpdateContext UpdateContext(FMaterialUpdateContext::EOptions::Default, GMaxRHIShaderPlatform);
	UpdateContext.AddMaterialInstance(materialInstance);
	materialInstance->MarkPackageDirty();
}

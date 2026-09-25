// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianePainting3DActor.h"
#include "ArianePainting3DComponent.h"
// Unreal headers
#include "Components/SceneComponent.h"

AArianePainting3DActor::~AArianePainting3DActor()
{
}

AArianePainting3DActor::AArianePainting3DActor(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    Painting3DComponent = ObjectInitializer.CreateDefaultSubobject<UArianePainting3DComponent>(this,TEXT("Painting3DComponent"));

    PrimaryActorTick.bCanEverTick = true;

    RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this,TEXT("RootComponent"));

    Painting3DComponent->SetupAttachment( RootComponent );

    // Add this so that the Component shows in the Outliner
    AddInstanceComponent( Painting3DComponent );
}

UArianePainting3DComponent*
AArianePainting3DActor::GetPainting3DComponent()
{
    return Painting3DComponent;
}

void
AArianePainting3DActor::PostActorCreated()
{
    Painting3DComponent->Init();
}

void
AArianePainting3DActor::EnsureNestedComponentsArePublic()
{
    TArray<UActorComponent*> AllComponents;
    GetComponents(AllComponents, /*bIncludeFromChildActors=*/true);

    for (UActorComponent* Comp : AllComponents)
    {
        if (Comp && !Comp->HasAnyFlags(RF_Public))
        {
            Comp->SetFlags(RF_Public);

            // ensure they all have this actor as owner
            Comp->Rename( nullptr, this, REN_DontCreateRedirectors );
        }
    }
}

void
AArianePainting3DActor::PostInitProperties()
{
    Super::PostInitProperties();
    //EnsureNestedComponentsArePublic();
}

void AArianePainting3DActor::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
    Super::PostDuplicate(DuplicateMode);
    //EnsureNestedComponentsArePublic();
}

void
AArianePainting3DActor::PostLoad()
{
    Super::PostLoad();

    //EnsureNestedComponentsArePublic();

    Painting3DComponent->Init();
}

void
AArianePainting3DActor::BeginPlay()
{
    Super::BeginPlay();
}

void
AArianePainting3DActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

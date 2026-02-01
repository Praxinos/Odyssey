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

AArianePainting3DActor::AArianePainting3DActor()
{
    UArianePainting3DComponent* painting3DComponent = CreateDefaultSubobject<UArianePainting3DComponent>(TEXT("Painting3DComponent"));

    PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));

    painting3DComponent->SetupAttachment( RootComponent );
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

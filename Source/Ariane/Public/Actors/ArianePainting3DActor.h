// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

// Unreal headers
#include "CoreMinimal.h"

#include "ArianePainting3DActor.generated.h"

class UArianePainting3DComponent;

UCLASS()
class ARIANE_API AArianePainting3DActor : public AActor
{
    GENERATED_BODY()

    public:
        ~AArianePainting3DActor();
        AArianePainting3DActor();

    protected:
        virtual void BeginPlay() override;

    public:
        virtual void Tick(float DeltaTime) override;
};

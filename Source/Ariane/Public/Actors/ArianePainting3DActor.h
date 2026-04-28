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


    virtual void BeginPlay() override;
    virtual void PostActorCreated() override;
    virtual void PostLoad() override;
    virtual void Tick(float DeltaTime) override;

    UArianePainting3DComponent* GetPainting3DComponent();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Painting 3D", meta = (AllowPrivateAccess = "true"))
    UArianePainting3DComponent* Painting3DComponent;
};

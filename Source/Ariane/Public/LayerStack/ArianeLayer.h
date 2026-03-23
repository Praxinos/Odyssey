// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

// Unreal headers
#include "CoreMinimal.h"

#include "ArianeLayer.generated.h"


class UArianeLayerFolder;

UCLASS()
class ARIANE_API UArianeLayer : public UObject
{
    GENERATED_BODY()

    public:
        ~UArianeLayer();
        UArianeLayer();

    public:
/*
        virtual void TickComponent( float DeltaTime
                                  , ELevelTick TickType
                                  , FActorComponentTickFunction* ThisTickFunction ) override;
        virtual void PostLoad() override;

        #if WITH_EDITOR
        virtual void PostEditChangeProperty( FPropertyChangedEvent& event ) override;

        virtual void PostEditUndo() override;
        #endif
*/
};

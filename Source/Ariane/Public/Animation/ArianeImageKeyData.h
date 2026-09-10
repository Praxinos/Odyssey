// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once
// Unreal headers
#include "CoreMinimal.h"
#if WITH_EDITOR
#include "MovieSceneClipboard.h"
#endif
// Ariane Headers
#include "ArianeKeyedObject.h"

#include "ArianeImageKeyData.generated.h"

class UArianeImage;

USTRUCT(BlueprintType)
struct FArianeImageKeyData
{
    GENERATED_BODY()

public:
    void RecordGeometry( UArianeImage* RecordedImage );
    FArianeKeyedObject* GetKeyedObject( const FGuid& ObjectGuid );
    void PostLoad();
    void PostEditUndo();

    UPROPERTY(EditAnywhere, Instanced, Category = "Ariane")
    UArianeImage* Image = nullptr;

protected:
    UPROPERTY( EditAnywhere )
    TArray<FInstancedStruct> InstancedKeyedObjects;
};

#if WITH_EDITOR
namespace MovieSceneClipboard
{
    template<>
    inline FName GetKeyTypeName<FArianeImageKeyData>()
    {
        return "ArianeImageKeyData";
    }
}
#endif

// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "OdysseyPainterEditorGuide.h"
#include "OdysseyPainterEditorGuideCollection.generated.h"

/** Asset-owned collection of instanced visual guides. Stored directly on animations and as asset user data on textures. */
UCLASS(BlueprintType)
class ODYSSEYGUIDES_API UOdysseyPainterEditorGuideCollection : public UAssetUserData
{
    GENERATED_BODY()

public:
    /** Guides in display order. Each guide is instanced beneath this collection. */
    UPROPERTY(EditAnywhere, Instanced, Category="Guides")
    TArray<TObjectPtr<UOdysseyPainterEditorGuide>> Guides;
};

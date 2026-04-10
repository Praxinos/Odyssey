// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal Headers
#include "CoreMinimal.h"
// Ariane Headers

#include "ArianeEditorLayerView.generated.h"

class UArianeLayer;
class FArianeEditor;

UCLASS()
class ARIANEEDITOR_API UArianeEditorLayerView : public UObject
{
GENERATED_BODY()

public:
    void SetEditor( FArianeEditor* InEditor );
    virtual void ImportLayerProperties( UArianeLayer* Layer );

protected:
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostEditChangeLayerProperty( UArianeLayer* Layer, FPropertyChangedEvent& PropertyChangedEvent );

public:
    UPROPERTY(EditAnywhere, Category = "LayerSettings")
    FTransform Transform;

protected:
    FArianeEditor* Editor;
};

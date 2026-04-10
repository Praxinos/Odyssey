// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal Headers
#include "CoreMinimal.h"
// Ariane Headers
#include "ArianeLayerDrawing.h"
#include "ArianeEditorLayerView.h"

#include "ArianeEditorLayerDrawingView.generated.h"

class UArianeLayer;

UCLASS()
class ARIANEEDITOR_API UArianeEditorLayerDrawingView : public UArianeEditorLayerView
{
GENERATED_BODY()

public:
    virtual void ImportLayerProperties( UArianeLayer* Layer ) override;

protected:
    virtual void PostEditChangeLayerProperty( UArianeLayer* Layer, FPropertyChangedEvent& PropertyChangedEvent ) override;

public:
    UPROPERTY(EditAnywhere, Category = "LayerSettings")
    EArianeLayerDrawingOrigin DrawingOrigin;

    UPROPERTY(EditAnywhere, Category = "LayerSettings")
    EArianeLayerDrawingOrientation DrawingOrientation;

};

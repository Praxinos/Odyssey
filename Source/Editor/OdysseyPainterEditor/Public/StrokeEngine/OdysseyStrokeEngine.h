// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "OdysseyBrushOptions.h"
#include "OdysseyBlendParameters.h"
#include "StrokeEngine/OdysseyStrokeEngineWorker.h"
#include "StrokeEngine/OdysseyStrokeOptions.h"
#include "StrokeEngine/OdysseyFreehandShape.h"
#include "StrokeEngine/Smoothing/IOdysseySmoothing.h"

#include "OdysseyStrokeEngine.generated.h"

class UOdysseyBrushAssetBase;
class FOdysseyPaintEngine;
class FOdysseyStrokeEngineBrushOptions;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyStrokeEngine : public UObject
{
    GENERATED_UCLASS_BODY()

public:
    // Destructor
    virtual ~UOdysseyStrokeEngine();
    
public:
    //TOOL
    void Initialize(FOdysseyPaintEngine* iPaintEngine);
    void Activate();
    void Inactivate();

public:
    // Paint Engine Stroke API

    //Begins a stroke at iPoint
    //Some value are computed from the last call to MoveTo(), like direction for example
    bool Begin( const FOdysseyPoint& iPoint );

    //Draws a Stroke from the last position to iPoint
    bool To( const FOdysseyPoint& iPoint );

    //Ends the stroke
    bool End();

    //Aborts the stroke
    bool Abort();

public:
    // Tick
    void Tick(float iDeltaTime);

public:
    // Setters

    // Sets the PaintEngine used to draw
    void SetPaintEngine(FOdysseyPaintEngine* iPaintEngine);

    // Recreates the brush instance
    void RefreshBrushInstance();


public:
    // Getters

    //Returns the selected Brush
    UOdysseyBrush* GetBrush();
    
    //Returns the instance of the selected Brush used to draw
    UOdysseyBrushAssetBase* GetBrushInstance();

    // Returns the BlendParameters
    FOdysseyBlendParameters GetBlendParameters() const;

    // Returns the BrushOptions
    UOdysseyBrushOptions* GetBrushOptions();

public:
    //UObject overrides

    //Called when properties inside structs are about to be modified
    virtual void PreEditChange(FEditPropertyChain& PropertyAboutToChange);

    //Called when a property is about to be modified externally
    virtual void PreEditChange(FProperty* PropertyAboutToChange);
    
    //Called when a property inside a struct property changes
    virtual void PostEditChangeChainProperty( struct FPropertyChangedChainEvent& PropertyChangedEvent);

    //Called when a simple property changes
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);

private:
    // Internal - BrushInstance

    //Destroy the BrushInstance
    void DestroyBrushInstance();

    //Creates the Brush Instance and applies brush overrides if asked
    void CreateBrushInstance(bool iApplyOverrides);

    // Fired when the Brush is compiled
    void OnBrushCompiled(UBlueprint* iBlueprint);

private:
    // Internal - Property Changed

    // Fired when the Brush is compiled
    void OnPreBrushChanged();

    // Fired when the Brush is compiled
    void OnPostBrushChanged();

private:
    // Internal - Callbacks

    void OnShapePathBegin(const FOdysseyPoint& iPoint);

    void OnShapePathTo(const TArray<FOdysseyPoint>& iPoints);

    void OnShapePathEnd(const FOdysseyPoint& iPoint);

    void OnShapeReset();

    void OnPaintEngineBlockChanged();

private:
    //PROPERTIES

    UPROPERTY(EditInstanceOnly)
    UOdysseyBrush* Brush;

    UPROPERTY(VisibleInstanceOnly, Instanced)
    UOdysseyFreehandShape* Shape;

    UPROPERTY(EditInstanceOnly)
    FOdysseyBlendParameters BlendParameters;

    UPROPERTY()
    UOdysseyBrushAssetBase* BrushInstance;

    UPROPERTY()
    UOdysseyBrushOptions* BrushOptions;

protected:
    // protected Data Members

    //Resources
    FOdysseyPaintEngine*                mPaintEngine;
    FOdysseyStrokeEngineWorker          mWorker;

    //---

    //Internal
    bool                                mIsPainting;
};

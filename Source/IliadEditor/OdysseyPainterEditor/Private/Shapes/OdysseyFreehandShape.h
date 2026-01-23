// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "OdysseyShape.h"
#include "OdysseySmoothingOptions.h"
#include "IOdysseySmoothing.h"

#include "OdysseyFreehandShape.generated.h"

class FOdysseyHUDPolygon;

UCLASS(meta=(DisplayName="Freehand Shape"))
class UOdysseyFreehandShape : public UOdysseyShape
{
    GENERATED_UCLASS_BODY()

public:
    // Destructor
    virtual ~UOdysseyFreehandShape();

public:
    //Mouse events
    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture);
    virtual bool OnKeyDown(const FKey& iKey);

    virtual void Abort() override;

public:
    // Tick
    void Tick(float iDeltaTime);

    // Applies the shapes specific overrides
    void ApplyOverrides(const TMap< TObjectPtr<UClass>, TObjectPtr<UObject>>& iOverrides);

public:
    // Getters
    //Returns the SmoothingOptions
    FOdysseySmoothingOptions& GetSmoothingOptions();
    void DisplayHUD(bool iDisplayHUD);

private:
    // Internal

    //Draws a Stroke from the last position to iPoint
    bool StrokeTo(const FOdysseyPoint& iPoint);

    // Begins the smoothing process
    void BeginSmoothing();

    // Smoothes the given point
    // Returns false if no smoothing point has been generated
    bool SmoothTo(const FOdysseyPoint& iPoint);

    // Applies the CatchUp if needed
    void CatchUp();

    // Applies the smoothing if enabled and not in realtime
    void ReapplySmoothing();

private:
    void CreateHUD();
    void RefreshHUD();
    void RemoveHUD();

private:
    UPROPERTY( EditInstanceOnly, Category="Shape", meta=(InlineEditConditionToggle))
    bool    SmoothingEnabled = false;

    UPROPERTY( EditInstanceOnly, Category="Shape", meta=(DisplayName="Smoothing", editcondition = "SmoothingEnabled") )
    FOdysseySmoothingOptions SmoothingOptions;

protected:
    //Internal
    TArray< FOdysseyPoint >             mRawStroke; //Raw Stroke (basically mouse positions)
    TArray< FOdysseyPoint >             mSmoothedStroke; //The raw stroke once smoothed using the smoother
    TSharedPtr<IOdysseySmoothing>       mSmoother;

    TSharedPtr<FOdysseyHUDPolygon> mPathHUD;

    bool mIsDrawing = false;
    bool mDisplayHUD = false;
};

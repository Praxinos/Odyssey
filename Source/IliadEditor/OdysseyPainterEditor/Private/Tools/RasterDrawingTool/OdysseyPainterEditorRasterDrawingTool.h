// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "UObject/UObjectGlobals.h"
#include "InputCoreTypes.h"
#include "Tools/RasterBaseTool/OdysseyPainterEditorRasterBaseTool.h"
#include "OdysseyBrushBlueprint.h"
#include "OdysseyBrushOptions.h"
#include "OdysseyBlendParameters.h"
#include "OdysseyShape.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyBrushContext.h"
#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingToolWorker.h"
#include "FreehandShape/Interpolation/OdysseyInterpolationTypes.h"

#include "OdysseyPainterEditorRasterDrawingTool.generated.h"

class UOdysseyBrushAssetBase;
class FOdysseyPaintEngine;
class FOdysseyStrokeEngineBrushOptions;
class IOdysseyInterpolation;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorRasterDrawingTool : public UOdysseyPainterEditorRasterBaseTool
{
    GENERATED_BODY()

public:
    typedef TMap< TObjectPtr<UClass>, TObjectPtr<UObject>> tOverride;

    DECLARE_MULTICAST_DELEGATE_OneParam(FOnApplyOverrides, const tOverride&);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnDestroyBrushInstance, UOdysseyBrushAssetBase*);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnCreatedBrushInstance, UOdysseyBrushAssetBase*);
    DECLARE_DELEGATE_RetVal_OneParam(TArray<FOdysseyPoint>, FAdaptShapePoints, const TArray<FOdysseyPoint>&);
public:
    // Destructor
    virtual ~UOdysseyPainterEditorRasterDrawingTool();

    //Constructor
    UOdysseyPainterEditorRasterDrawingTool();

public:
    //TOOL
    template<class T> T* CreateShape(FName iName);

public:
    //OdysseyPainterEditorTool overrides
    virtual void Activate() override;
    virtual bool IsActivable() const override;
    virtual void Load() override;
    virtual void Unload() override;

    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture) override;
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture) override;
    virtual bool OnKeyDown(const FKey& iKey) override;
    virtual bool OnKeyUp(const FKey& iKey) override;

    virtual void Tick(float iDeltaTime) override;

    virtual void Flush() override;
    virtual void Commit() override;

    virtual void BindShortcuts(class FBaseToolkit* iToolkit) override;
    virtual void ExtendMenu( TSharedRef<FExtender> iExtender) override;
    virtual void ExtendToolbar( FToolBarBuilder& iBuilder ) override;

    virtual EMouseCursor::Type GetMouseCursor() const override;

    virtual FText GetTooltip() const override;

public:
    // Setters
    // Sets the BrushContexts to apply to brushInstance
    void SetBrushContexts(TArray<FOdysseyBrushContext*>* iContexts);

    // Recreates the brush instance
    void RefreshBrushInstance();

    void SetBaseSize(float iValue);

public:
    // Getters

    //Returns the selected Brush
    UOdysseyBrush* GetBrush() const;

    //Returns the instance of the selected Brush used to draw
    UOdysseyBrushAssetBase* GetBrushInstance() const;

    // Returns the BlendParameters
    FOdysseyBlendParameters GetBlendParameters() const;

    // Returns the BrushOptions
    UOdysseyBrushOptions* GetBrushOptions() const;

    // Returns the OnDestroyBrushInstance delegate
    FOnDestroyBrushInstance& OnDestroyBrushInstance();

    // Returns the OnCreatedBrushInstance delegate
    FOnCreatedBrushInstance& OnCreatedBrushInstance();

    // Returns the OnApplyOverrides delegate
    FOnApplyOverrides& OnApplyOverridesDelegate();
    FAdaptShapePoints& AdaptShapePointsDelegate();

    FSimpleMulticastDelegate& OnBrushChanged();

public:
    //Properties changes
    void BrushChanged();

    virtual void PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive) override;
    virtual void PostPropertyChanged(const FName& iPropertyName, bool iIsInteractive) override;

public:
    // Paint Engine Stroke API

    //Begins a stroke at iPoint
    //Some value are computed from the last call to MoveTo(), like direction for example
    bool Begin(const FOdysseyPoint& iPoint);

    //Draws a Stroke from the last position to iPoint
    bool To(const FOdysseyPoint& iPoint);

    //Ends the stroke
    bool End();

    //Aborts the stroke
    bool Abort();

private:
    // Internal - BrushInstance

    //Destroy the BrushInstance
    void DestroyBrushInstance();

    //Adds some amound to the brush size
    void AddSize(int iAmount);
    void SetAlphaMode(::ULIS::eAlphaMode iAlphaMode);
    void SetBlendMode(::ULIS::eBlendMode iBlendMode);

    //Creates the Brush Instance and applies brush overrides if asked
    void CreateBrushInstance(bool iApplyOverrides);

    void ConfigureBrushInstance(UOdysseyBrushAssetBase* iBrushInstance);

    // Fired when the Brush is compiled
    //void OnBlueprintCompiled(UBlueprint* iBlueprint);

    void OnBlueprintReinstanced(const FCoreUObjectDelegates::FReplacementObjectMap& iObjectMap);

    //Apply brush Overrides
    void ApplyOverrides(UOdysseyBrushAssetBase* iBrushInstance);

private:
    void BeginStroke(const FOdysseyPoint& iPoint);
    void StrokeTo(const TArray<FOdysseyPoint>& iPoints);
    void StrokeEnd();
    void StrokeAbort();
    void StrokeReset();

    // Internal - Callbacks
    void OnShapeInteractive(const TArray<FOdysseyPoint>& iPoints);
    void OnShapeCommit(const TArray<FOdysseyPoint>& iPoints, bool iReset);
    void OnShapeAbort();

    void OnPaintEngineBlockChanged();
    void OnRasterSelectionChanged();

    float AdaptShapeStep(float iStep);

    TArray<FOdysseyPoint> InterpolateTo(const FOdysseyPoint& iPoint);
    void ResetInterpolation();

    void AddBlendingModeToolbarMenuEntry(FMenuBuilder& iMenuBuilder, EOdysseyBlendingMode iBlendingMode);

public:
    friend class SOdysseyPainterEditorRasterDrawingToolBrushSelector;

private:
    UFUNCTION(BlueprintSetter)
    void SubPixelBlueprintSetter(bool Value);

public:
    UPROPERTY(meta=(ForceShowEngineContent, ForceShowPluginContent))
    UOdysseyBrush* Brush;

    UPROPERTY()
    UOdysseyBrushAssetBase* BrushInstance;

    UPROPERTY()
    UOdysseyBrushOptions* BrushOptions;

    UPROPERTY(EditAnywhere, Category="Shape")
    FOdysseyShapes Shapes;

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Shape", BlueprintSetter=SubPixelBlueprintSetter)
    bool SubPixel = true;

    UPROPERTY(EditAnywhere, Category="Interpolation", meta = (ClampMin = "1", UIMin = "1", LinearDeltaSensitivity = "15", Delta = "1", Multiple = "1"))
    float   Step = 1.0;

    UPROPERTY( EditAnywhere, Category="Interpolation")
    bool    AdaptativeStep = false;

    UPROPERTY( EditInstanceOnly, Category="Interpolation")
    EOdysseyInterpolationType InterpolationType = EOdysseyInterpolationType::kCatmullRom;

    UPROPERTY(EditInstanceOnly, Category="Blending", meta=(ShowOnlyInnerProperties))
    FOdysseyBlendParameters BlendParameters;

protected:
    // protected Data Members

    //Resources
    FOdysseyPaintEngine                 mPaintEngine;
    TArray<FOdysseyBrushContext*>*       mBrushContexts;
    FOdysseyPainterEditorRasterDrawingToolWorker     mWorker;
    TSharedPtr<IOdysseyInterpolation>   mInterpolator;
    float mBaseSize; //Size on which the tool is based to compute its size from a percentage

    //---

    //Internal
    FOnApplyOverrides                   mOnApplyOverridesDelegate;
    FOnDestroyBrushInstance             mOnDestroyBrushInstance;
    FOnCreatedBrushInstance             mOnCreatedBrushInstance;
    FAdaptShapePoints                   mAdaptShapePointsDelegate;
    FSimpleMulticastDelegate            mOnBrushChanged;

    TSharedPtr<FOdysseyHUDElement> mShapeHUD;

    FOdysseyPoint mLastPoint;
    bool mIsFirstPoint = true;

    FOdysseyPoint mSubPixelPoint;
};

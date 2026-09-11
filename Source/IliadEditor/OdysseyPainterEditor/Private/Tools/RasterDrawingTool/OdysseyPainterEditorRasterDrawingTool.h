// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "UObject/UObjectGlobals.h"
#include "InputCoreTypes.h"
#include "OdysseyPainterEditorRasterBaseTool.h"
#include "OdysseyBrushBlueprint.h"
#include "OdysseyBrushOptions.h"
#include "OdysseyBlendParameters.h"
#include "OdysseyShape.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyBrushContext.h"
#include "OdysseyPainterEditorRasterDrawingToolWorker.h"
#include "OdysseyInterpolationTypes.h"

#include "OdysseyPainterEditorRasterDrawingTool.generated.h"

class UOdysseyBrushAssetBase;
class FOdysseyPaintEngine;
class FOdysseyStrokeEngineBrushOptions;
class IOdysseyInterpolation;
class FScopedTransaction;
UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorRasterDrawingTool : public UOdysseyPainterEditorRasterBaseTool
{
    GENERATED_BODY()

public:
    typedef TMap< TObjectPtr<UClass>, TObjectPtr<UObject>> tOverride;

    DECLARE_MULTICAST_DELEGATE_OneParam(FOnApplyOverrides, const tOverride&);
    DECLARE_DELEGATE_RetVal_OneParam(TArray<FOdysseyPoint>, FAdaptShapePoints, const TArray<FOdysseyPoint>&);

public:
    static void RegisterDetailCustomization();
    static void UnregisterDetailCustomization();

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
    virtual void Inactivate() override;
    virtual bool IsActivable() const override;

    virtual void Reset() override;

    virtual void BeginInteractiveMode();
    virtual void EndInteractiveMode();

    virtual bool HasRadius() const override;
    virtual void SetRadius(float Radius = false) override;
    virtual float GetRadius() const override;

    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture) override;
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture) override;
    virtual bool OnKeyDown(const FKey& iKey) override;
    virtual bool OnKeyUp(const FKey& iKey) override;

    virtual void Tick(float iDeltaTime) override;

    virtual void Flush() override;
    virtual void Commit() override;

    virtual void BindShortcuts(TSharedPtr<FUICommandList> iCommandList) override;
    virtual void ExtendMenu( TSharedRef<FExtender> iExtender) override;
    virtual void ExtendToolbar( UToolMenu* iToolMenu ) override;

    virtual FText GetTooltip() const override;

protected:
    virtual TOptional<FMouseCursor> GetMouseCursorOverride() const override;

public:
    // Setters
    // Sets the BrushContexts to apply to brushInstance
    void SetBrushContexts(TArray<FOdysseyBrushContext*>* iContexts);

    // Recreates the brush instance
    void RecreateBrushInstance(bool iApplyOverrides);

    // Refresh the brush instance based on the existing one in UProperties
    void RefreshBrushInstance(bool iApplyOverrides);

    void SetBaseSize(float iValue);

    void SetBrush(UOdysseyBrush* iBrush);

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

    // Returns the OnApplyOverrides delegate
    FAdaptShapePoints& AdaptShapePointsDelegate();

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
    TSharedPtr<FOdysseyRasterBlock> GetRasterBlockFromEditor(bool iCreate) const;

    void BeginStroke();
    void StrokeTo(const TArray<FOdysseyPoint>& iPoints);
    void StrokeEnd();

    // Internal - Callbacks
    void OnShapeBegin();
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

public:
    /** The brush. */
    UPROPERTY(
        meta=(
            Tooltip="Brush asset to draw with.",
            ForceShowEngineContent,
            ForceShowPluginContent
        ))
    TObjectPtr<UOdysseyBrush> Brush;

    /** The brush. */
    UPROPERTY(
        EditInstanceOnly,
        Instanced,
        Category="Brush",
        meta = (
          EditInline
          , IgnoreToolConfiguration
          , AllowEditInlineCustomization
          /*, ShowInnerProperties*/
        ))
    TObjectPtr<UOdysseyBrushAssetBase> BrushInstance;

    /** The brush options. */
    UPROPERTY()
    TObjectPtr<UOdysseyBrushOptions> BrushOptions;

    /** The list of available shapes. */
    UPROPERTY(
        EditAnywhere,
        Category="Shape",
        meta = (
            Tooltip="Shape to use to draw",
            IgnoreToolConfiguration
        ))
    FOdysseyShapes Shapes;

    /** Use subpixel mode. */
    UPROPERTY(
        EditInstanceOnly,
        Category="Shape",
        meta = (
            Tooltip="If checked, allows stamps to be positionned between 2 pixels for a smoother result."
        )
    )
    bool SubPixel = true;

    /** The step. */
    UPROPERTY(
        EditAnywhere,
        Category="Interpolation",
        meta = (
            Tooltip = "Defines the space between 2 stamps in pixels",
            ClampMin = "1",
            UIMin = "1",
            LinearDeltaSensitivity = "15",
            Delta = "1"
        ))
    float   Step = 1.0;

    /** Use adaptative step. */
    UPROPERTY(
        EditAnywhere,
        Category="Interpolation",
        meta = (
            Tooltip = "If checked, the step will increase proportionally to the tool's size"
        )
    )
    bool    AdaptativeStep = false;

    /** The interpolation type. */
    UPROPERTY(
        EditInstanceOnly,
        Category="Interpolation",
        meta = (
            Tooltip = "Type of interpolation used to interpolate between mouse positions"
        )
    )
    EOdysseyInterpolationType InterpolationType = EOdysseyInterpolationType::kCatmullRom;

    /** The blending parameters. */
    UPROPERTY(
        EditInstanceOnly,
        Category="Blending",
        meta=(
            Tooltip = "Parameters defining how the stroke blends with the current drawing",
            ShowOnlyInnerProperties
        ))
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
    FAdaptShapePoints                   mAdaptShapePointsDelegate;

    TSharedPtr<FOdysseyHUDElement> mShapeHUD;

    FOdysseyPoint mLastPoint;
    bool mIsFirstPoint = true;

    FOdysseyPoint mSubPixelPoint;
    TSharedPtr<FScopedTransaction> mTransaction;
};

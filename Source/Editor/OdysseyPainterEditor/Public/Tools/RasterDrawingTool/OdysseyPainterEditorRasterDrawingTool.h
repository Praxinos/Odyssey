// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyBrushBlueprint.h"
#include "OdysseyBrushOptions.h"
#include "OdysseyBlendParameters.h"
#include "OdysseyShape.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyBrushContext.h"
#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingToolWorker.h"

#include "OdysseyPainterEditorRasterDrawingTool.generated.h"

class UOdysseyBrushAssetBase;
class FOdysseyPaintEngine;
class FOdysseyStrokeEngineBrushOptions;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorRasterDrawingTool : public UOdysseyPainterEditorTool
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
    virtual void ExtendMenu( FToolMenuOwner iOwner, FName iMenuName ) override;

public:
    // Setters
    // Sets the BrushContexts to apply to brushInstance
    void SetBrushContexts(TArray<FOdysseyBrushContext*> iContexts);

    // Recreates the brush instance
    void RefreshBrushInstance();

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

    // Returns the Selected Shape
    EOdysseyShape GetSelectedShape() const;

    // Retuns the instance of the selected Shape
    UOdysseyShape* GetSelectedShapeInstance() const;

    // Returns the OnDestroyBrushInstance delegate
    FOnDestroyBrushInstance& OnDestroyBrushInstance();

    // Returns the OnCreatedBrushInstance delegate
    FOnCreatedBrushInstance& OnCreatedBrushInstance();

    // Returns the OnApplyOverrides delegate
    FOnApplyOverrides& OnApplyOverridesDelegate();
    FAdaptShapePoints& AdaptShapePointsDelegate();

    FSimpleMulticastDelegate& OnBrushChanged();
    FSimpleMulticastDelegate& OnShapeChanged();

public:
    //Properties changes
    void BrushChanged();
    void SelectedShapeChanged();

    virtual void PropertyChanged(const FName& iPropertyName) override;

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

    //Wether the tool can draw or not
    virtual bool CanDraw();

private:
    // Internal - BrushInstance

    //Destroy the BrushInstance
    void DestroyBrushInstance();

    //Creates the Brush Instance and applies brush overrides if asked
    void CreateBrushInstance(bool iApplyOverrides);

    // Fired when the Brush is compiled
    //void OnBlueprintCompiled(UBlueprint* iBlueprint);

    //Apply brush Overrides
    void ApplyOverrides(UOdysseyBrushAssetBase* iBrushInstance);

private:
    // Internal - Property Changed

    // Fired when the Brush is compiled
    /* void OnPreBrushChanged();

    // Fired when the Brush is compiled
    void OnPostBrushChanged();

    // Fired when a Shape is selected
    void OnPostShapeChanged(); */

private:
    // Internal - Callbacks

    void OnShapePathBegin(const FOdysseyPoint& iPoint);

    void OnShapePathTo(const TArray<FOdysseyPoint>& iPoints);

    void OnShapePathEnd(const FOdysseyPoint& iPoint);

    void OnShapePathAbort();
    
    void OnShapePathReset();

    void OnPaintEngineBlockChanged();

    float AdaptShapeStep(float iStep);

protected:
    friend class SOdysseyPainterEditorRasterDrawingToolBrushSelector;

    //Visible properties

    UPROPERTY()
    UOdysseyBrush* Brush;

    UPROPERTY()
    UOdysseyBrushAssetBase* BrushInstance;

    UPROPERTY()
    UOdysseyBrushOptions* BrushOptions;

    UPROPERTY()
    EOdysseyShape SelectedShape;

    UPROPERTY(VisibleInstanceOnly, Category="Shape", Instanced, meta=(ShowInnerProperties))
    class UOdysseyShape* SelectedShapeInstance;


    // Hidden properties
    UPROPERTY()
    TMap<EOdysseyShape, class UOdysseyShape*> AvailableShapes;

    UPROPERTY(EditInstanceOnly, Category="Blending", meta=(ShowOnlyInnerProperties))
    FOdysseyBlendParameters BlendParameters;

protected:
    // protected Data Members

    //Resources
    FOdysseyPaintEngine                 mPaintEngine;
    TArray<FOdysseyBrushContext*>       mBrushContexts;
    FOdysseyPainterEditorRasterDrawingToolWorker     mWorker;

    //---

    //Internal
    FOnApplyOverrides                   mOnApplyOverridesDelegate;
    FOnDestroyBrushInstance             mOnDestroyBrushInstance;
    FOnCreatedBrushInstance             mOnCreatedBrushInstance;
    FAdaptShapePoints                   mAdaptShapePointsDelegate;
    FSimpleMulticastDelegate            mOnBrushChanged;
    FSimpleMulticastDelegate            mOnShapeChanged;
};

// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "OdysseyTool.h"
#include "OdysseyBrushBlueprint.h"
#include "OdysseyBrushOptions.h"
#include "OdysseyBlendParameters.h"
#include "OdysseyShape.h"
#include "Tools/DrawingTool/OdysseyDrawingToolWorker.h"

#include "OdysseyDrawingTool.generated.h"

class UOdysseyBrushAssetBase;
class FOdysseyPaintEngine;
class FOdysseyStrokeEngineBrushOptions;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyDrawingTool : public UOdysseyTool
{
    GENERATED_BODY()

public:
    typedef TMap<FName, UObject*> tOverride;

    DECLARE_MULTICAST_DELEGATE_OneParam(FOnApplyOverrides, const tOverride& iOverrides);

public:
    // Destructor
    virtual ~UOdysseyDrawingTool();

    //Constructor
    UOdysseyDrawingTool();
    
public:
    //TOOL
    void Initialize(FOdysseyPaintEngine* iPaintEngine);
    template<class T> T* CreateShape(FName iName);

public:
    //OdysseyTool overrides
    virtual void Activate() override;
    virtual void Inactivate() override;

    virtual void OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture) override;
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture) override;
    virtual void OnKeyDown(const FKey& iKey) override;
    virtual void OnKeyUp(const FKey& iKey) override;

    virtual void Tick(float iDeltaTime) override;

    virtual void Flush() override;
    virtual void Commit() override;

    virtual void BindShortcuts(class FBaseToolkit* iToolkit) override;
    virtual void ExtendMenu( FToolMenuOwner iOwner, FName iMenuName ) override;
    virtual TSharedPtr<SWidget> GetWidget() override;

private:
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
    // Setters

    // Sets the PaintEngine used to draw
    void SetPaintEngine(FOdysseyPaintEngine* iPaintEngine);

    // Sets the BrushContexts to apply to brushInstance
    void SetBrushContexts(TArray<FOdysseyBrushContext*> iContexts);

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

    // Returns the OnApplyOverrides delegate
    FOnApplyOverrides& OnApplyOverridesDelegate();

    // Returns the Selected Shape
    EOdysseyShape GetSelectedShape() const;

    // Retuns the instance of the selected Shape
    UOdysseyShape* GetSelectedShapeInstance();

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

    //Apply brush Overrides
    void ApplyOverrides(UOdysseyBrushAssetBase* iBrushInstance);

private:
    // Internal - Property Changed

    // Fired when the Brush is compiled
    void OnPreBrushChanged();

    // Fired when the Brush is compiled
    void OnPostBrushChanged();

    // Fired when a Shape is selected
    void OnPostShapeChanged();

private:
    // Internal - Callbacks

    void OnShapePathBegin(const FOdysseyPoint& iPoint);

    void OnShapePathTo(const TArray<FOdysseyPoint>& iPoints);

    void OnShapePathEnd(const FOdysseyPoint& iPoint);

    void OnShapePathAbort();
    
    void OnShapePathReset();

    void OnPaintEngineBlockChanged();

private:
    friend class SOdysseyDrawingToolBrushSelector;

    //Visible properties

    UPROPERTY(EditInstanceOnly)
    UOdysseyBrush* Brush;

    UPROPERTY(EditInstanceOnly)
    UOdysseyBrushAssetBase* BrushInstance;

    UPROPERTY(EditInstanceOnly)
    UOdysseyBrushOptions* BrushOptions;

    UPROPERTY(EditInstanceOnly)
    FOdysseyBlendParameters BlendParameters;

    UPROPERTY(EditInstanceOnly)
    EOdysseyShape SelectedShape;

    UPROPERTY(EditInstanceOnly)
    class UOdysseyShape* SelectedShapeInstance;


    // Hidden properties
    UPROPERTY()
    TMap<EOdysseyShape, class UOdysseyShape*> AvailableShapes;

protected:
    // protected Data Members

    //Resources
    FOdysseyPaintEngine*                mPaintEngine;
    TArray<FOdysseyBrushContext*>       mBrushContexts;
    FOdysseyDrawingToolWorker           mWorker;

    //---

    //Internal
    bool                                mIsPainting;
    FOnApplyOverrides                   mOnApplyOverridesDelegate;
    TSharedPtr<SWidget>                 mOptionsWidget;
};

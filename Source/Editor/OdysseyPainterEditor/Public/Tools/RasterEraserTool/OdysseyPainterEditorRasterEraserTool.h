// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyShape.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyPainterEditorRasterEraserTool.generated.h"

class FOdysseyPaintEngine;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorRasterEraserTool :
    public UOdysseyPainterEditorTool
{
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorRasterEraserTool();

    //Constructor
    UOdysseyPainterEditorRasterEraserTool();
    
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

    virtual EMouseCursor::Type GetMouseCursor() const override;

    virtual void Commit() override;

    virtual TSharedRef<SWidget> CreateTopTabWidget() override;

    virtual FText GetTooltip() const override;

public:
    // Getters
    // Returns the Selected Shape
    EOdysseyShape GetSelectedShape() const;

    // Retuns the instance of the selected Shape
    UOdysseyShape* GetSelectedShapeInstance() const;

    FSimpleMulticastDelegate& OnShapeChanged();
    FSimpleMulticastDelegate& OnSizeChanged();
    FSimpleMulticastDelegate& OnOpacityChanged();

public:
    //Properties changes
    void SelectedShapeChanged();
    void SizeChanged();
    void OpacityChanged();

    virtual void PropertyChanged(const FName& iPropertyName) override;
    
private:
    // Internal - Callbacks
    void OnShapePathBegin(const FOdysseyPoint& iPoint);

    void OnShapePathTo(const TArray<FOdysseyPoint>& iPoints);

    void OnShapePathEnd(const FOdysseyPoint& iPoint);

    void OnShapePathAbort();
    
    void OnShapePathReset();
    
    void OnRasterSelectionChanged();

    float AdaptShapeStep(float iStep);

private:
    TSharedPtr<::ULIS::FBlock> CreateStampBlockMask();
    void PrepareStampBlock();
    void Stamp(const FOdysseyPoint& iPoint);

protected:
    UPROPERTY( EditAnywhere, Category="Parameters", meta = ( ClampMin = "1", UIMin = "1", LinearDeltaSensitivity = "15", Delta = "1", Multiple="1", DisplayPriority="1" ) )
    float   Size = 20.f;

    UPROPERTY( EditAnywhere, Category="Parameters", meta = ( ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100", Delta = "1", Multiple="1", Units="Percent") )
    float   Flow = 100.f;

    UPROPERTY( EditAnywhere, Category="Parameters", meta = ( ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100", Delta = "1", Multiple = "1", Units = "Percent"))
    float   Opacity = 100.f;

    //Visible properties
    UPROPERTY(EditAnywhere, Category="Shape")
    EOdysseyShape SelectedShape;

    UPROPERTY(VisibleInstanceOnly, Category="Shape", Instanced, meta=(ShowInnerProperties))
    class UOdysseyShape* SelectedShapeInstance;
    // Hidden properties
    UPROPERTY()
    TMap<EOdysseyShape, class UOdysseyShape*> AvailableShapes;

protected:
    // protected Data Members

    //Resources
    FOdysseyPaintEngine                 mPaintEngine;
    TSharedPtr<::ULIS::FBlock>          mStampBlock;
    TSharedPtr<::ULIS::FBlock>          mStampBlockMask;
    FOdysseyBlendParameters             mBlendParameters;
    //---

    //Internal
    FSimpleMulticastDelegate            mOnShapeChanged;
    FSimpleMulticastDelegate            mOnSizeChanged;
    FSimpleMulticastDelegate            mOnOpacityChanged;
};

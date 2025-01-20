// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "Tools/RasterBaseTool/OdysseyPainterEditorRasterBaseTool.h"
#include "OdysseyShape.h"
#include "OdysseyPaintEngine.h"
#include "FreehandShape/Interpolation/OdysseyInterpolationTypes.h"
#include "OdysseyPainterEditorRasterEraserTool.generated.h"

class FOdysseyPaintEngine;
class IOdysseyInterpolation;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorRasterEraserTool :
    public UOdysseyPainterEditorRasterBaseTool
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

    virtual FText GetTooltip() const override;

public:
    // Getters
    FSimpleMulticastDelegate& OnSizeChanged();
    FSimpleMulticastDelegate& OnOpacityChanged();

public:
    //Properties changes
    void SizeChanged();
    void OpacityChanged();
    void ActiveShapeChanged();

    virtual void PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive) override;
    virtual void PostPropertyChanged(const FName& iPropertyName, bool iIsInteractive) override;

private:
    // Internal - Callbacks
    void OnShapeInteractive(const TArray<FOdysseyPoint>& iPoints);
    void OnShapeCommit(const TArray<FOdysseyPoint>& iPoints, bool iReset);
    void OnShapeAbort();

    void OnRasterSelectionChanged();

    float AdaptShapeStep(float iStep);

    TArray<FOdysseyPoint> InterpolateTo(const FOdysseyPoint& iPoint);
    void ResetInterpolation();

private:
    TSharedPtr<::ULIS::FBlock> CreateStampBlockMask();
    void PrepareStampBlock();
    void Stamp(const FOdysseyPoint& iPoint);

protected:
    //Visible properties
    UPROPERTY(EditAnywhere, Category="Shape")
    FOdysseyShapes Shapes;

    UPROPERTY( EditAnywhere, Category="Parameters", meta = ( ClampMin = "1", UIMin = "1", LinearDeltaSensitivity = "15", Delta = "1", Multiple="1", DisplayPriority="1" ) )
    float   Size = 20.f;

    UPROPERTY( EditAnywhere, Category="Parameters", meta = ( ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100", Delta = "1", Multiple="1", Units="Percent") )
    float   Flow = 100.f;

    UPROPERTY( EditAnywhere, Category="Parameters", meta = ( ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100", Delta = "1", Multiple = "1", Units = "Percent"))
    float   Opacity = 100.f;

    UPROPERTY( EditInstanceOnly, Category="Interpolation")
    EOdysseyInterpolationType InterpolationType = EOdysseyInterpolationType::kCatmullRom;

    UPROPERTY( EditAnywhere, Category="Interpolation")
    bool    AdaptativeStep = false;

    UPROPERTY( EditAnywhere, Category="Interpolation", meta = (ClampMin = "1", UIMin = "1", LinearDeltaSensitivity = "15", Delta = "1", Multiple = "1", DisplayPriority = "0"))
    float   Step = 1.0;

protected:
    // protected Data Members

    //Resources
    FOdysseyPaintEngine                 mPaintEngine;
    TSharedPtr<::ULIS::FBlock>          mStampBlock;
    TSharedPtr<::ULIS::FBlock>          mStampBlockMask;
    FOdysseyBlendParameters             mBlendParameters;
    TSharedPtr<IOdysseyInterpolation>   mInterpolator;
    FOdysseyPoint mLastPoint;
    //---

    //Internal
    FSimpleMulticastDelegate            mOnSizeChanged;
    FSimpleMulticastDelegate            mOnOpacityChanged;

    TSharedPtr<FOdysseyHUDElement> mShapeHUD;
};

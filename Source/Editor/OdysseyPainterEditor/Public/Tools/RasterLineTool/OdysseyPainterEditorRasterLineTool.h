// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyPainterEditorRasterLineTool.generated.h"

class FOdysseyHUDLine;
class FOdysseyHUDHandle;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorRasterLineTool : public UOdysseyPainterEditorTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorRasterLineTool();

    //Constructor
    UOdysseyPainterEditorRasterLineTool();

    virtual bool IsActivable() const override;

    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture) override;
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture) override;
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;

    virtual bool OnKeyUp(const FKey& iKey) override;

    virtual void Load() override;
    virtual void Unload() override;

    ::ULIS::FRectI GetLineBoundingRect();

private:
    void BlendLineToPaintBlock();

    void CommitLine();
    void AbortLine();

    //Resets the tool and its HUD
    void ClearLine();

private: 
    FOdysseyPaintEngine                             mPaintEngine;

    FOdysseyHUDLine* mLine;
    TArray<FOdysseyHUDHandle*> mHandles;

    FOdysseyRasterBlockMutator mRasterMutator;
};

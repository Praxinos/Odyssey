// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyVectorPathBuilder.h"

#include "OdysseyPainterEditorVectorPrimitiveDrawingTool.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorPrimitiveDrawingTool : public UOdysseyPainterEditorTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorVectorPrimitiveDrawingTool();

    //Constructor
    UOdysseyPainterEditorVectorPrimitiveDrawingTool();
 
    //OdysseyPainterEditorTool overrides
    virtual void Activate() override;

    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture) override;
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void Commit() override;

public:
    // Setters
    virtual bool CanDraw();

protected:

};

// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyVector.h"

#include "OdysseyPainterEditorVectorGridTool.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorGridTool : public UOdysseyPainterEditorTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorVectorGridTool();

    //Constructor
    UOdysseyPainterEditorVectorGridTool();
 
    //OdysseyPainterEditorTool overrides
    virtual void Activate() override;

    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture) override;
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void Commit() override;

private:
    FOdysseyVectorHUDGrid mGridHUD;
    void Redraw(const TArray<::ULIS::FRectI>& iRects);

private:
    double mOldLocalMouseX;
    double mOldLocalMouseY;
    std::vector<FGridNode *> mGridNodeArray;
    bool mMultipleSelectionMode;

public:
    // Setters
    virtual bool CanDraw();

    UPROPERTY(EditAnywhere, Category="Odyssey Grid Tool")
    uint32 DivisionsX;

    UPROPERTY(EditAnywhere, Category="Odyssey Grid Tool")
    uint32 DivisionsY;

    UPROPERTY(EditAnywhere, Category="Odyssey Grid Tool")
    double PickingRadius;

protected:

};

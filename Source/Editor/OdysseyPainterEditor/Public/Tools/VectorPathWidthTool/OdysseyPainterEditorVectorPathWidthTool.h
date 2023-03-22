// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyVector.h"
#include "OdysseyPainterEditorVectorPathWidthTool.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorPathWidthTool : public UOdysseyPainterEditorTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorVectorPathWidthTool();

    //Constructor
    UOdysseyPainterEditorVectorPathWidthTool();
 
    //OdysseyPainterEditorTool overrides
    virtual void Activate() override;

    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void OnMouseHover( const FOdysseyPoint& iPointInTexture ) override;
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture) override;
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void Commit() override;

    void PropertyChanged( const FName& iPropertyName );
    void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent );

private:
    std::vector<FOdysseyVectorSegment*> mSegmentArray;
    FOdysseyVectorHUDPicking mPickingHUD;

public:
    // Setters
    virtual bool CanDraw();

    UPROPERTY(EditAnywhere, Category="Odyssey PathWidth Tool")
    double Radius;

    UPROPERTY(EditAnywhere, Category="Odyssey PathWidth Tool")
    double Strength;

protected:

};

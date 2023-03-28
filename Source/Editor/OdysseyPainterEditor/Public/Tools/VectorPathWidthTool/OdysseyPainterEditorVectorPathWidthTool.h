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
 
    void Activate( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    bool OnMouseDown( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene, const FOdysseyPoint& iPointInTexture,const FKey& iKey );
    void OnMouseHover( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene, const FOdysseyPoint& iPointInTexture );
    void OnMouseDrag( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene, const FOdysseyPoint& iPointInTexture);
    bool OnMouseUp( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene, const FOdysseyPoint& iPointInTexture, const FKey& iKey);

    //OdysseyPainterEditorTool overrides
    virtual void Commit() override;

    void PropertyChanged( const FName& iPropertyName );

private:
    std::vector<FOdysseyVectorSegment*> mSegmentArray;
    FOdysseyVectorHUDPicking mPickingHUD;

public:
    UPROPERTY(EditAnywhere, Category="Odyssey PathWidth Tool")
    double Radius;

    UPROPERTY(EditAnywhere, Category="Odyssey PathWidth Tool")
    double Strength;

};

// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/DefaultTool/OdysseyPainterEditorDefaultTool.h"
#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndoSelect.h"

#include "OdysseyPainterEditorVectorObjectPickTool.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorObjectPickTool : public UOdysseyPainterEditorDefaultTool
{
public:
    GENERATED_BODY()

public:
    static bool DoubleClicked();
    // Destructor
    virtual ~UOdysseyPainterEditorVectorObjectPickTool();

    //Constructor
    UOdysseyPainterEditorVectorObjectPickTool();
 
    void ActivateVector( FOdysseyVectorEngine* iEngine
                       , FOdysseyVectorScene* iScene
                       , int32 iSizeX
                       , int32 iSizeY );
    bool OnMouseDownVector( FOdysseyVectorEngine* iEngine
                          , FOdysseyVectorScene* iScene
                          , const FOdysseyPoint& iPointInTexture
                          , const FKey& iKey );
    void OnMouseDragVector( FOdysseyVectorEngine* iEngine
                          , FOdysseyVectorScene* iScene
                          , const FOdysseyPoint& iPointInTexture );
    bool OnMouseUpVector( FOdysseyVectorEngine* iEngine
                        , FOdysseyVectorScene* iScene
                        , const FOdysseyPoint& iPointInTexture
                        , const FKey& iKey );
    virtual bool OnKeyDownVector( FOdysseyVectorEngine* iEngine
                                , FOdysseyVectorScene* iScene
                                , const FKey& iKey ) override;
    virtual bool OnKeyUpVector( FOdysseyVectorEngine* iEngine
                              , FOdysseyVectorScene* iScene
                              , const FKey& iKey ) override;

    //OdysseyPainterEditorTool overrides
    virtual void Commit() override;

    void Copy( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    void Paste( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );

private:
    FOdysseyVectorHUDSelection *mSelectionHUD;
    std::vector<::ULIS::FVec2D> mPointArray;
};

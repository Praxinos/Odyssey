// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"
#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndoPathStitch.h"

#include "OdysseyPainterEditorVectorPathStitchTool.generated.h"

class FOdysseyPainterEditorVectorPathStitchToolHUD;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorPathStitchTool : public UOdysseyPainterEditorVectorBaseTool
{
    public:
        GENERATED_BODY()

    public:
        // Destructor
        virtual ~UOdysseyPainterEditorVectorPathStitchTool();

        //Constructor
        UOdysseyPainterEditorVectorPathStitchTool();

        virtual bool IsActivable() const override;

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorScene* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorScene* iScene ) override;
        //virtual uint64 OnKeyDownVector( FOdysseyVectorScene* iScene
        //                            , const FKey& iKey ) override;
        //virtual uint64 OnKeyUpVector( FOdysseyVectorScene* iScene, const FKey& iKey ) override;
        virtual uint64 OnMouseDownVector( FOdysseyVectorScene* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey ) override;
        virtual uint64 OnMouseHoverVector( FOdysseyVectorScene* iScene
                                         , const FOdysseyPoint& iPointInTexture ) override;
        virtual uint64 OnMouseDragVector( FOdysseyVectorScene* iScene
                                        , const FOdysseyPoint& iPointInTexture ) override;
        virtual uint64 OnMouseUpVector( FOdysseyVectorScene* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey ) override;
        virtual uint64 PropertyChangedVector( FOdysseyVectorScene* iScene
                                            , const FName& iPropertyName ) override;

    private:
            FOdysseyPainterEditorVectorPathStitchToolHUD* mPathStitchHUD;

    public:
        UPROPERTY( EditAnywhere, Category = PathStitchTool, meta = (ClampMin = "0.0", UIMin = "0.0") )
        double PickingRadius;
/*
        UPROPERTY( EditAnywhere, Category = PathStitchTool, meta = (ClampMin = "0.0", UIMin = "0.0") )
        bool RestrictToSelection;
*/
};

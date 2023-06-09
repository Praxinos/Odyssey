// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorObjectPickTool/OdysseyPainterEditorVectorObjectPickTool.h"
#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndoObjectTransform.h"

#include "OdysseyPainterEditorVectorObjectMoveTool.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorObjectMoveTool : public UOdysseyPainterEditorVectorObjectPickTool
{
    public:
        GENERATED_BODY()

    public:
        // Destructor
        virtual ~UOdysseyPainterEditorVectorObjectMoveTool();

        //Constructor
        UOdysseyPainterEditorVectorObjectMoveTool();

        virtual void UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene ) override;
        virtual void LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene ) override;
        virtual bool OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                      , FOdysseyVectorScene* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey ) override;
        virtual void OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                      , FOdysseyVectorScene* iScene
                                      , const FOdysseyPoint& iPointInTexture ) override;
        virtual bool OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                    , FOdysseyVectorScene* iScene
                                    , const FOdysseyPoint& iPointInTexture
                                    , const FKey& iKey ) override;

        //OdysseyPainterEditorTool overrides
        virtual void Commit() override;

    private:
        std::vector<FObjectTransform> mObjectTransformArray;
        bool mDragging;
};

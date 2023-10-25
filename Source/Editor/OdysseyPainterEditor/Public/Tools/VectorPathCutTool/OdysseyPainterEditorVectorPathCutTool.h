// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"
#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndoPathAlter.h"

#include "OdysseyPainterEditorVectorPathCutTool.generated.h"

class FOdysseyPainterEditorVectorPathCutToolHUD;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorPathCutTool : public UOdysseyPainterEditorVectorBaseTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorVectorPathCutTool();

    //Constructor
    UOdysseyPainterEditorVectorPathCutTool();

    virtual bool IsActivable() const override;
    virtual void Load() override;
    virtual void Unload() override;

    virtual bool OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
    virtual void OnMouseDrag( const FOdysseyPoint& iPointInTexture ) override;
    virtual bool OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;

    void UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    void LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
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

    //OdysseyPainterEditorTool overrides
    virtual void Commit() override;

    protected:
        void CutPath( FOdysseyVectorPath* iPath
                    , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                    , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                    , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray );
        void CutPaintGroup( FOdysseyVectorGroupPaint* iPaintGroup
                          , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                          , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                          , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray  );
private:
    FOdysseyPainterEditorVectorPathCutToolHUD* mPathCutHUD;
    //FOdysseyVectorHUDPathCubic mCubicPathHUD;
};

// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"
#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndoErase.h"

#include "OdysseyPainterEditorVectorEraserTool.generated.h"

class FOdysseyPainterEditorVectorEraserToolHUD;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorEraserTool : public UOdysseyPainterEditorVectorBaseTool
{
    public:
        GENERATED_BODY()

    public:
        // Destructor
        virtual ~UOdysseyPainterEditorVectorEraserTool();

        //Constructor
        UOdysseyPainterEditorVectorEraserTool();

        virtual bool IsActivable() const override;

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        //virtual bool OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
        //                            , const FKey& iKey ) override;
        //virtual bool OnKeyUpVector( FOdysseyVectorGroupPaint* iScene, const FKey& iKey ) override;
        virtual uint64 OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey ) override;
        virtual uint64 OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                         , const FOdysseyPoint& iPointInTexture ) override;
        virtual uint64 OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture ) override;
        virtual uint64 OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey ) override;
        //virtual void PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
        //                                 , const FName& iPropertyName ) override;

        void ErasePaths( FOdysseyVectorGroupPaint* iScene
                       , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                       , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                       , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                       , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                       , std::vector<FOdysseyVectorObject*>& oRemovedObjectArray );
        void EraseSections( FOdysseyVectorGroupPaint* iScene
                          , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                          , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                          , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                          , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                          , std::vector<FOdysseyVectorObject*>& oRemovedObjectArray );

    public:
        UPROPERTY( EditAnywhere, Category = EraserTool, meta = (ClampMin = "0.0", UIMin = "0.0") )
        double Radius;

    private:
        FOdysseyPainterEditorVectorEraserToolHUD* mEraserHUD;

};

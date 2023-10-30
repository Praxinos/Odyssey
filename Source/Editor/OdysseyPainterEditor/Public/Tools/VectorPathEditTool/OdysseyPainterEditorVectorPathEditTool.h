// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"
#include "OdysseyVector.h"

#include "OdysseyPainterEditorVectorPathEditTool.generated.h"

class FOdysseyPainterEditorVectorPathEditToolHUD;

UENUM()
enum class ePathPickingMode : uint8
{
    Vertex = 0,
    VertexHandle = 1,
    SegmentHandle = 2
};

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorPathEditTool : public UOdysseyPainterEditorVectorBaseTool
{
    public:
        GENERATED_BODY()

    public:
        // Destructor
        virtual ~UOdysseyPainterEditorVectorPathEditTool();

        //Constructor
        UOdysseyPainterEditorVectorPathEditTool();

        virtual bool IsActivable() const override;

        uint64 GetPickingFlags();
        ePathPickingMode GetPickingMode();

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorScene* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorScene* iScene ) override;
        virtual uint64 OnKeyDownVector( FOdysseyVectorScene* iScene
                                      , const FKey& iKey ) override;
        virtual uint64 OnKeyUpVector( FOdysseyVectorScene* iScene, const FKey& iKey ) override;
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
        //virtual void PropertyChangedVector( FOdysseyVectorScene* iScene
        //                                  , const FName& iPropertyName ) override;

    private:
        FOdysseyVectorPathCubic* FetchPath( FOdysseyVectorScene* iScene
                                          , double iWorldX
                                          , double iWorldY );
        void OnMouseDownDeletePoint( FOdysseyVectorScene* iScene
                                   , const FOdysseyPoint& iPointInTexture
                                   , const FKey& iKey );
        void OnMouseDownPickPoint( FOdysseyVectorScene* iScene
                                 , const FOdysseyPoint& iPointInTexture
                                 , const FKey& iKey );
        void GroupPaintDeletePoint( FOdysseyVectorGroupPaint* iGroupPaint
                                  , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                                  , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray
                                  , std::vector<FOdysseyVectorPath*>& iRemovedPathArray
                                  , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray
                                  , double iSelectionRadius
                                  , const FOdysseyPoint& iPointInTexture );

        ::ULIS::FRectD DragPoint( FOdysseyVectorPoint *iPoint
                                , double iWorldX
                                , double iWorldY
                                , double iDeltaX
                                , double iDeltaY
                                , bool iWidenAllAlong );

        void GetPathsFromSelection( FOdysseyVectorScene* iScene
                                  , std::vector<FOdysseyVectorPath*>& oPathArray );

    private:
        FOdysseyPainterEditorVectorPathEditToolHUD *mPathEditHUD;
        std::vector<FOdysseyVectorPoint*> mPickedPointArray;
        std::vector<FOdysseyVectorPath*> mSelectedPathArray;
        uint64 mPickingFlags;
        ePathPickingMode mPickingMode;
        ::ULIS::FVec2D mOldPointInTexture;

    public:
        UPROPERTY( EditAnywhere, Category = PathEditTool, meta = (ClampMin = "0.0", UIMin = "0.0") )
        double PickingRadius;

        UPROPERTY( EditAnywhere, Category = PathEditTool )
        bool WidenAllAlong;

        UPROPERTY( EditAnywhere, Category = Behavior )
        bool RestrictToSelection;
};

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
    SegmentHandle = 2,
    Alter = 3 // Add, Remove or cut
};

// struct that stores the ratio of handleLength / segmentLength at mouseDown
// This allows us to adjust the handle length when moving vertices.
typedef struct _FSegmentAdjustment
{
    double handleRatio[2];
    FOdysseyVectorSegment* segment;

    _FSegmentAdjustment( FOdysseyVectorSegment* iSegment )
    {
        double segmentLength = iSegment->GetLength();

        segment = iSegment;
        handleRatio[0] = handleRatio[1] = 0.0f;

        if( segmentLength )
        {
            if( iSegment->HasBaseClass( FOdysseyVectorSegmentCubic::StaticClass() ) )
            {
                FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(iSegment);
                ::ULIS::FVec2D handleVector0 = cubicSegment->GetHandleVector( (uint32)0, false );
                ::ULIS::FVec2D handleVector1 = cubicSegment->GetHandleVector( (uint32)1, false );

                handleRatio[0] = handleVector0.Distance() / segmentLength;
                handleRatio[1] = handleVector1.Distance() / segmentLength;
            }
        }
    }

    void Adjust()
    {
        if( segment->HasBaseClass( FOdysseyVectorSegmentCubic::StaticClass() ) )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);
            FOdysseyVectorVertex* vertex0 = cubicSegment->GetVertex(0);
            FOdysseyVectorVertex* vertex1 = cubicSegment->GetVertex(1);
            ::ULIS::FVec2D handleVector0 = cubicSegment->GetHandleVector( vertex0, true );
            ::ULIS::FVec2D handleVector1 = cubicSegment->GetHandleVector( vertex1, true );
            double currentSegmentLength = cubicSegment->GetLength();

            cubicSegment->GetHandle(0)->Set( vertex0->GetCoords() + ( handleVector0 * currentSegmentLength * handleRatio[0] ) );
            cubicSegment->GetHandle(1)->Set( vertex1->GetCoords() + ( handleVector1 * currentSegmentLength * handleRatio[1] ) );
        }
    }
}
FSegmentAdjustment;

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
        virtual TSharedRef<SWidget> CreateTopTabWidget();
        
        virtual FText GetTooltip() const override;

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                          , const FKey& iKey ) override;
        virtual uint64 OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                            , const FKey& iKey ) override;
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
        //                                  , const FName& iPropertyName ) override;

    private:
        void OnMouseUpCutPaths( FOdysseyVectorGroupPaint* iScene
                              , const FOdysseyPoint& iPointInTexture );
        void OnMouseUpAddPoint( FOdysseyVectorGroupPaint* iScene
                              , const FOdysseyPoint& iPointInTexture
                              , const std::vector<FOdysseyVectorSegment*>& iPickedSegmentArray );
        void OnMouseUpDeletePoint( FOdysseyVectorGroupPaint* iScene
                                 , const std::vector<FOdysseyVectorPoint*>& iPickedPointArray );
        void OnMouseDownPickPoint( FOdysseyVectorGroupPaint* iScene
                                 , const FOdysseyPoint& iPointInTexture
                                 , const FKey& iKey );
        void PickObjects( FOdysseyVectorGroupPaint* iScene
                        , double iX
                        , double iY );
/*
        void GroupPaintDeletePoint( FOdysseyVectorGroupPaint* iGroupPaint
                                  , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                                  , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray
                                  , std::vector<FOdysseyVectorPath*>& iRemovedPathArray
                                  , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray
                                  , double iSelectionRadius
                                  , const FOdysseyPoint& iPointInTexture );
*/
        ::ULIS::FRectD DragSegmentHandle( FOdysseyVectorHandleSegment *iHandle
                                        , double iWorldX
                                        , double iWorldY
                                        , double iDeltaX
                                        , double iDeltaY
                                        , bool iRealign );

        ::ULIS::FRectD DragVertex( FOdysseyVectorVertex *iVertex
                                 , double iWorldX
                                 , double iWorldY
                                 , double iDeltaX
                                 , double iDeltaY
                                 , bool iWidenAllAlong );

        void GetPathsFromSelection( FOdysseyVectorGroupPaint* iScene
                                  , std::vector<FOdysseyVectorPath*>& oPathArray );

        static void BuildSegmentAdjustments( const std::vector<FOdysseyVectorSegment*>& iSegmentArray
                                           , std::vector<FSegmentAdjustment>& oSegmentAdjustmentArray );

    private:
        FOdysseyPainterEditorVectorPathEditToolHUD *mPathEditHUD;
        std::vector<FOdysseyVectorVertex*> mPickedVertexArray;
        std::vector<FOdysseyVectorHandleSegment*> mPickedHandleArray;
        std::vector<FSegmentAdjustment> mSegmentAdjustmentArray;
        std::vector<FOdysseyVectorPath*> mSelectedPathArray;
        uint64 mPickingFlags;
        ePathPickingMode mPickingMode;
        ::ULIS::FVec2D mOldPointInTexture;

    public:
        UPROPERTY( EditAnywhere
                 , Category=PathEditTool
                 , meta = ( ToolTip = "Picking Radius"
                          , ClampMin = "0.0"
                          , UIMin = "0.0" ) )
        double PickingRadius;

        UPROPERTY( EditAnywhere
                 , Category=PathEditTool
                 , meta = ( ToolTip = "Widen All Along" ) )
        bool WidenAllAlong;
};

// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Misc/Optional.h"

#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorSegmentCubic.h"

#include "OdysseyPainterEditorVectorPathEditTool.generated.h"


class FOdysseyPainterEditorVectorPathEditToolHUD;
class FOdysseyVectorVertex;
class FOdysseyVectorGroupPaint;
class FOdysseyVectorHandleSegment;

UENUM()
enum class eVectorPathEditEditionMode : uint8
{
    Vertex = 0,
    VertexHandle = 1,
    SegmentHandle = 2,
    Alter = 3, // Add, Remove or cut
    ToolRadius = 4 UMETA(Hidden)
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
        eVectorPathEditEditionMode GetEditionMode();

        virtual FText GetTooltip() const override;

        virtual void ExtendToolbar( UToolMenu* iToolMenu ) override;
        virtual void BindShortcuts( TSharedPtr<FUICommandList> iCommandList ) override;

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual bool OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                        , const FKeyEvent& InKeyEvent ) override;
        virtual bool OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                          , const FKeyEvent& InKeyEvent ) override;
        virtual bool OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey ) override;
        virtual void OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                         , const FOdysseyPoint& iPointInTexture ) override;
        virtual void OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture ) override;
        virtual bool OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey ) override;
        virtual void OnVectorLayerUpdate( const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags
                                        , uint32 iUpdateFlags ) override;

    private:
        uint64 OnMouseUpCutPaths( FOdysseyVectorGroupPaint* iScene
                                , const FOdysseyPoint& iPointInTexture );
        uint64 OnMouseUpAddPoint( FOdysseyVectorGroupPaint* iScene
                                , const FOdysseyPoint& iPointInTexture
                                , const std::vector<FOdysseyVectorSegment*>& iPickedSegmentArray );
        uint64 OnMouseUpDeletePoint( FOdysseyVectorGroupPaint* iScene
                                   , const std::vector<FOdysseyVectorPoint*>& iPickedPointArray );
        uint64 OnMouseDownPickPoint( FOdysseyVectorGroupPaint* iScene
                                   , const FOdysseyPoint& iPointInTexture
                                   , const FKey& iKey );
        uint64 PickObjects( FOdysseyVectorGroupPaint* iScene
                          , double iX
                          , double iY );

        void DragSegmentHandle( FOdysseyVectorHandleSegment *iHandle
                              , double iWorldX
                              , double iWorldY
                              , double iDeltaX
                              , double iDeltaY );

        void DragVertex( FOdysseyVectorVertex *iVertex
                       , double iWorldX
                       , double iWorldY
                       , double iDeltaX
                       , double iDeltaY );

        void DragVertexHandle( FOdysseyVectorVertex *iVertex
                             , double iOriginalRadius
                             , const ::ULIS::FVec2D& iMouse
                             , bool iWidenAllAlong );

        void GetPathsFromSelection( FOdysseyVectorGroupPaint* iScene
                                  , std::vector<FOdysseyVectorPath*>& oPathArray );
        virtual void ExtendContextMenuObject( FOdysseyVectorGroupPaint* iScene
                                            , FMenuBuilder& menu
                                            , uint64 iObjectMenuFlags ) override;

        static void BuildSegmentAdjustments( const std::vector<FOdysseyVectorSegment*>& iSegmentArray
                                           , std::vector<FSegmentAdjustment>& oSegmentAdjustmentArray );

        void SetEditionMode( eVectorPathEditEditionMode inValue );
        TSharedRef<SWidget> CreateModifierSegmentControl();
        const FSlateBrush* GetBackgroundColor( eVectorPathEditEditionMode iMode ) const;
        void ActionSubdivideSegments();

    public:
        virtual bool IsSameAs(const UOdysseyPainterEditorTool* Other) const override;

    private:
        FOdysseyPainterEditorVectorPathEditToolHUD *mPathEditHUD;
        std::vector<FOdysseyVectorVertex*> mPickedVertexArray;
        std::vector<::ULIS::FVec2D> mPickedVertexPositionArray;
        std::vector<double> mPickedVertexRadiusArray;
        std::vector<FOdysseyVectorHandleSegment*> mPickedHandleArray;
        std::vector<::ULIS::FVec2D> mPickedHandlePositionArray;
        std::vector<FSegmentAdjustment> mSegmentAdjustmentArray;
        std::vector<FOdysseyVectorPath*> mSelectedPathArray;
        uint64 mPickingFlags;
        eVectorPathEditEditionMode mEditionMode;
        ::ULIS::FVec2D mPointInTextureAtDown;

    public:
        UPROPERTY( EditAnywhere
                 , Category=PathEditTool
                 , meta = ( ToolTip  = "Picking Radius"
                          , ClampMin = "0"
                          , UIMin    = "0" ) )
        uint32 PickingRadius;

        UPROPERTY( EditAnywhere
                 , Category=PathEditTool
                 , meta = ( ToolTip = "Widen All Along" ) )
        bool WidenAllAlong;
};

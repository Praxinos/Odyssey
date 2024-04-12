// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"

#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndo.h"

#include "OdysseyPainterEditorVectorPathDrawingTool.generated.h"

class FOdysseyPainterEditorVectorPathDrawingToolHUD;
class FOdysseyVectorUndoPathExtend;

UENUM()
enum class eTracingType : uint8
{
    Organic  = 0,
    Mechanic = 1
};

UENUM()
enum class eTracingFidelity: uint8
{
    Lowest  = 10,
    Low     =  8,
    Average =  6,
    High    =  4,
    Highest =  2
};

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorPathDrawingTool : public UOdysseyPainterEditorVectorBaseTool
{
    public:
        GENERATED_BODY()

    public:
        // Destructor
        virtual ~UOdysseyPainterEditorVectorPathDrawingTool();

        //Constructor
        UOdysseyPainterEditorVectorPathDrawingTool();

        virtual bool IsActivable() const override;

        FOdysseyVectorPathTracer& GetPathTracer();

        virtual TSharedRef<SWidget> CreateTopTabWidget() override;

        virtual FText GetTooltip() const override;

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
                                      , const FKey& iKey ) override;
        virtual uint64 OnKeyUpVector( FOdysseyVectorGroupPaint* iScene, const FKey& iKey ) override;
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
        virtual uint64 PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                            , const FName& iPropertyName ) override;

        FOdysseyVectorObject* GetParentObject( FOdysseyVectorGroupPaint* iScene );

    private:
        FOdysseyVectorVertex* PickVertex( FOdysseyVectorGroupPaint* iScene
                                        , double iWorldX
                                        , double iWorldY
                                        , double iPickingRadius );

        void RecordUndoPathAdd( FOdysseyVectorGroupPaint* iScene
                                     , FOdysseyVectorPath* iPath
                                     , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                                     , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray );
        void RecordUndoPathAlter( FOdysseyVectorGroupPaint* iScene
                                       , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                                       , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray );

    public:
        UPROPERTY( EditAnywhere
                 , Category = PathDrawingTool
                 , meta = ( ToolTip = "Color Mode" ) )
        eForegroundColorMode ColorMode;

        //UPROPERTY( EditAnywhere, Category = PathDrawingTool, meta = (ClampMin = "0.0",UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0" ))
        double Opacity;

        UPROPERTY( EditAnywhere
                 , Category = PathDrawingTool
                 , meta = ( ToolTip = "Brush" ) )
        FOdysseyVectorBrush Brush;

        //UPROPERTY( EditAnywhere, Category="Odyssey PathDrawing Tool" )
        eTracingType TracingType;

        UPROPERTY( EditAnywhere
                 , Category = PathDrawingTool
                 , meta = ( ToolTip = "Tracing Fidelity" ) )
        eTracingFidelity TracingFidelity;

        UPROPERTY( EditAnywhere
                 , Category = PathDrawingTool
                 , meta = ( ToolTip = "Radius"
                          , ClampMin = "0.0"
                          , UIMin = "0.0" ) )
        double Radius;
        // computed based upon whether or not the pencil size is relative to the object's transformation matrix
        double mRealSize;

        UPROPERTY( EditAnywhere
                 , Category = PathDrawingTool
                 , meta = ( ToolTip = "Pressure Sensitive" ) )
        bool PressureSensitive;

        //UPROPERTY( EditAnywhere, Category = PathDrawingTool )
        //bool Absolute;

        UPROPERTY( EditAnywhere
                 , Category = PathDrawingTool
                 , meta = ( ToolTip = "Update PaintGroups" ) )
        bool UpdatePaintGroups;

        UPROPERTY( EditAnywhere
                 , Category = PathDrawingTool
                 , meta = ( ToolTip = "Stitch" ) )
        bool Stitch;
        bool StitchAtKeyDown;

        UPROPERTY( EditAnywhere
                 , Category = PathDrawingTool
                 , meta = ( ToolTip = "Snap" ) )
        bool Snap;

        UPROPERTY( EditAnywhere
                 , Category = PathDrawingTool
                 , meta = ( ToolTip = "Average Stitched Radius" ) )
        bool AverageStitchedRadius;

        UPROPERTY( EditAnywhere
                 , Category = PathDrawingTool
                 , meta = ( ToolTip = "Snap/Stitching Radius"
                          , ClampMin = "0.0"
                          , UIMin = "0.0" ) )
        double StitchingRadius;

        //UPROPERTY( EditAnywhere, Category="Odyssey PathDrawing Tool" )
        bool Debug;

    private:
        std::vector<FOdysseyVectorVertex*> mAddedVertexArray;
        std::vector<FOdysseyVectorSegment*> mAddedSegmentArray;
        FOdysseyPainterEditorVectorPathDrawingToolHUD* mPathDrawingHUD;
        FOdysseyVectorPathTracer mPathTracer;
        FOdysseyVectorVertex* mStitchedVertex;
        uint32 mPathNumber;
};

// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"

#include "OdysseyVectorPathTracer.h"

#include "OdysseyPainterEditorVectorPathDrawingTool.generated.h"

class FOdysseyPainterEditorVectorPathDrawingToolHUD;
class FOdysseyVectorUndoPathExtend;

enum class ePathDrawingMode : uint8
{
    Create = 0,
    Alter = 1
};

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
    Higher  =  2,
    Highest =  1
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

        virtual FText GetTooltip() const override;
        virtual void BindShortcuts(TSharedPtr<FUICommandList> iCommandList) override;
        virtual bool HasRadius() const override;
        virtual float GetRadius() const override;
        virtual void SetRadius(float iRadius)override;

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        bool OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                , const FKeyEvent& InKeyEvent ) override;
        bool OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                  , const FKeyEvent& InKeyEvent ) override;
        virtual bool OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey ) override;
        virtual void OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                         , const FOdysseyPoint& iPointInTextur ) override;
        virtual void OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture ) override;
        virtual bool OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                    , const FOdysseyPoint& iPointInTexture
                                    , const FKey& iKey ) override;
        virtual void PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                          , const FName& iPropertyName ) override;

        virtual void ExtendToolbar( UToolMenu* iToolMenu ) override;

    private:
        void AddSize(int iAmount);
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
        //UPROPERTY( EditAnywhere, Category=PathDrawingTool, meta = (ClampMin = "0.0",UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0" ))
        double Opacity;

        UPROPERTY( EditAnywhere
                 , Category=PathDrawingTool
                 , meta = ( ToolTip = "Brush" ) )
        FOdysseyVectorBrush Brush;

        //UPROPERTY( EditAnywhere, Category="Odyssey PathDrawing Tool" )
        eTracingType TracingType;

        UPROPERTY( EditAnywhere
                 , Category=PathDrawingTool
                 , meta = ( ToolTip = "Tracing Fidelity" ) )
        eTracingFidelity TracingFidelity;

        UPROPERTY( EditAnywhere
                 , Category=PathDrawingTool
                 , meta = ( ToolTip = "Radius"
                          , ClampMin = "0.0"
                          , UIMin = "0.0"
                          , LinearDeltaSensitivity = "15"
                          , Delta = "1" ) )
        double Radius;
        // computed based upon whether or not the pencil size is relative to the object's transformation matrix
        double mRealSize;

        UPROPERTY( EditAnywhere
                 , Category=PathDrawingTool
                 , meta = ( ToolTip = "Pressure Sensitive" ) )
        bool PressureSensitive;

        //UPROPERTY( EditAnywhere, Category=PathDrawingTool )
        //bool Absolute;

        UPROPERTY( EditAnywhere
                 , Category=PathDrawingTool
                 , meta = ( ToolTip = "Update PaintGroups" ) )
        bool UpdatePaintGroups;

        UPROPERTY( EditAnywhere
                 , Category=PathDrawingTool
                 , meta = ( ToolTip = "Stitch" ) )
        bool Stitch;
        bool StitchAtKeyDown;

        UPROPERTY( EditAnywhere
                 , Category=PathDrawingTool
                 , meta = ( ToolTip = "Snap" ) )
        bool Snap;

        UPROPERTY( EditAnywhere
                 , Category=PathDrawingTool
                 , meta = ( ToolTip = "Average Stitched Radius" ) )
        bool AverageStitchedRadius;

        UPROPERTY( EditAnywhere
                 , Category=PathDrawingTool
                 , meta = ( ToolTip = "Snap/Stitching Radius"
                          , ClampMin = "0"
                          , UIMin    = "0"
                          , LinearDeltaSensitivity = "15"
                          , Delta = "1" ) )
        uint32 StitchingRadius;

        //UPROPERTY( EditAnywhere, Category="Odyssey PathDrawing Tool" )
        bool Debug;

    private:
        uint64 mTimeAtDown;
        uint64 mTimeAtUp;
        std::vector<FOdysseyVectorVertex*> mAddedVertexArray;
        std::vector<FOdysseyVectorSegment*> mAddedSegmentArray;
        FOdysseyPainterEditorVectorPathDrawingToolHUD* mPathDrawingHUD;
        FOdysseyVectorPathTracer mPathTracer;
        FOdysseyVectorVertex* mStitchedVertex;
        uint32 mPathNumber;
        ePathDrawingMode mPathDrawingMode;
};

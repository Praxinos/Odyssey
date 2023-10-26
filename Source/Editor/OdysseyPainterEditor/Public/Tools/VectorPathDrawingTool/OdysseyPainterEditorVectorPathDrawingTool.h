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
enum class ePathDrawingToolColorSource : uint8
{
    ColorWheel = eBucketColorMode::SolidColor,
    Palette  = eBucketColorMode::Palette
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
        virtual uint64 PropertyChangedVector( FOdysseyVectorScene* iScene
                                            , const FName& iPropertyName ) override;

    private:
        bool HasMedia() const;
        FOdysseyVectorVertex* PickVertex( FOdysseyVectorScene* iScene
                                        , double iWorldX
                                        , double iWorldY
                                        , double iPickingRadius );
        void SetPathColor( FOdysseyVectorPath* iPath );
        void RecordUndoPathAdd( FOdysseyVectorScene* iScene, FOdysseyVectorPath* iPath  );
        void RecordUndoPathExtend( FOdysseyVectorScene* iScene, FOdysseyVectorPath* iPath );

    public:
        UPROPERTY( EditAnywhere, Category = PathDrawingTool )
        ePathDrawingToolColorSource ColorSource;

        UPROPERTY( EditAnywhere, Category = PathDrawingTool )
        FOdysseyVectorBrush Brush;

        //UPROPERTY( EditAnywhere, Category="Odyssey PathDrawing Tool" )
        eTracingType TracingType;

        UPROPERTY( EditAnywhere, Category = PathDrawingTool )
        eTracingFidelity TracingFidelity;

        UPROPERTY( EditAnywhere, Category = PathDrawingTool, meta = (ClampMin = "0.0", UIMin = "0.0") )
        double Radius;
        // computed based upon whether or not the pencil size is relative to the object's transformation matrix
        double mRealSize;

        UPROPERTY( EditAnywhere, Category = PathDrawingTool, meta = (ClampMin = "0.0",UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0" ))
        double Opacity;

        UPROPERTY( EditAnywhere, Category = PathDrawingTool )
        bool PressureSensitive;

        UPROPERTY( EditAnywhere, Category = PathDrawingTool )
        bool Absolute;

        UPROPERTY( EditAnywhere, Category = PathDrawingTool )
        bool UpdatePaintGroups;

        UPROPERTY( EditAnywhere, Category = PathDrawingTool )
        bool Stitch;
        bool StitchAtKeyDown;

        UPROPERTY( EditAnywhere, Category = PathDrawingTool )
        bool AverageStitchedRadius;

        UPROPERTY( EditAnywhere, Category = PathDrawingTool, meta = (ClampMin = "0.0", UIMin = "0.0") )
        double StitchingRadius;

        //UPROPERTY( EditAnywhere, Category="Odyssey PathDrawing Tool" )
        bool Debug;

    private:
        FOdysseyPainterEditorVectorPathDrawingToolHUD* mPathDrawingHUD;
        FOdysseyVectorPathTracer mPathTracer;
        FOdysseyVectorVertex* mStitchedVertex;
        FOdysseyVectorUndoPathExtend* mUndoPathExtend;
};

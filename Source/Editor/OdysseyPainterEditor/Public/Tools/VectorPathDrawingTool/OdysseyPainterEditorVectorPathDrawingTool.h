// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/DefaultTool/OdysseyPainterEditorDefaultTool.h"

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
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorPathDrawingTool : public UOdysseyPainterEditorDefaultTool
{
    public:
        GENERATED_BODY()

    public:
        // Destructor
        virtual ~UOdysseyPainterEditorVectorPathDrawingTool();

        //Constructor
        UOdysseyPainterEditorVectorPathDrawingTool();

        //OdysseyPainterEditorTool overrides
        virtual bool IsActivable() const override;
        virtual void Load() override;
        virtual void Unload() override;
        virtual bool OnKeyDown( const FKey& iKey ) override;
        virtual bool OnKeyUp( const FKey& iKey ) override;
        virtual bool OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
        virtual void OnMouseHover( const FOdysseyPoint& iPointInTexture ) override;
        virtual void OnMouseDrag( const FOdysseyPoint& iPointInTexture ) override;
        virtual bool OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
        virtual TSharedRef<SWidget> CreateTopTabWidget() override;

        bool OnKeyDownVector( FOdysseyVectorScene* iScene, const FKey& iKey );
        bool OnKeyUpVector( FOdysseyVectorScene* iScene, const FKey& iKey );
        void UnloadVector( FOdysseyVectorScene* iScene );
        void LoadVector( FOdysseyVectorScene* iScene );
        bool OnMouseDownVector( FOdysseyVectorScene* iScene, const FOdysseyPoint& iPointInTexture, const FKey& iKey );
        void OnMouseHoverVector( FOdysseyVectorScene* iScene, const FOdysseyPoint& iPointInTexture );
        void OnMouseDragVector( FOdysseyVectorScene* iScene, const FOdysseyPoint& iPointInTexture );
        bool OnMouseUpVector( FOdysseyVectorScene* iScene, const FOdysseyPoint& iPointInTexture, const FKey& iKey );

        //OdysseyPainterEditorTool overrides
        virtual void Commit() override;

        FOdysseyVectorPathTracer& GetPathTracer();

    protected:
        virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
        void PropertyChangedVector( FOdysseyVectorScene* iScene, const FName& iPropertyName );

    private:
        void OnSizeChanged();
        bool HasMedia() const;
        FOdysseyVectorVertex* PickVertex( FOdysseyVectorEngine* iVectorEngine
                                        , FOdysseyVectorScene* iScene
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

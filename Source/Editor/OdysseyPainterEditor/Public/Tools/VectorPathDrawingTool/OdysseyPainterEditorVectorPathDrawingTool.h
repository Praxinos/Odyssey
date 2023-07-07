// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/DefaultTool/OdysseyPainterEditorDefaultTool.h"

#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndo.h"

#include "OdysseyPainterEditorVectorPathDrawingTool.generated.h"

class FOdysseyPainterEditorVectorPathDrawingToolHUD;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorPathDrawingTool : public UOdysseyPainterEditorDefaultTool
{
    public:
        GENERATED_BODY()
/*
        DECLARE_MULTICAST_DELEGATE_OneParam(FSelectionChanged,FOdysseyVectorScene*)
        FSelectionChanged mSelectionChanged;
*/
    public:
        // Destructor
        virtual ~UOdysseyPainterEditorVectorPathDrawingTool();

        //Constructor
        UOdysseyPainterEditorVectorPathDrawingTool();

        //OdysseyPainterEditorTool overrides
        virtual bool IsActivable() const override;
        virtual void Load() override;
        virtual void Unload() override;

        virtual bool OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
        virtual void OnMouseHover( const FOdysseyPoint& iPointInTexture ) override;
        virtual void OnMouseDrag( const FOdysseyPoint& iPointInTexture ) override;
        virtual bool OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
 
        void UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
        void LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
        bool OnMouseDownVector( FOdysseyVectorEngine* iEngine
                              , FOdysseyVectorScene* iScene
                              , const FOdysseyPoint& iPointInTexture
                              , const FKey& iKey );
        ::ULIS::FRectI OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                                         , FOdysseyVectorScene* iScene
                                         , const FOdysseyPoint& iPointInTexture );
        ::ULIS::FRectI OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                        , FOdysseyVectorScene* iScene
                                        , const FOdysseyPoint& iPointInTexture );
        bool OnMouseUpVector( FOdysseyVectorEngine* iEngine
                            , FOdysseyVectorScene* iScene
                            , const FOdysseyPoint& iPointInTexture
                            , const FKey& iKey );

        //OdysseyPainterEditorTool overrides
        virtual void Commit() override;

        FOdysseyVectorPathBuilder* GetPathBuilder();

    protected:
        virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
        void PropertyChangedVector( FOdysseyVectorEngine* iEngine
                                  , FOdysseyVectorScene* iScene
                                  , const FName& iPropertyName );
        FOdysseyVectorVertex* PickVertex( FOdysseyVectorEngine* iVectorEngine
                                        , FOdysseyVectorScene* iScene
                                        , double iWorldX
                                        , double iWorldY
                                        , double iPickingRadius );
    private:
        FOdysseyVectorPathBuilder* MakePathBuilder( FOdysseyVectorEngine* iVectorEngine
                                                  , FOdysseyVectorScene* iScene
                                                  , double iWorldX
                                                  , double iWorldY );
        void OnSizeChanged();

    public:
        UPROPERTY( EditAnywhere, Category="Odyssey PathDrawing Tool", meta = (ClampMin = "0.0", UIMin = "0.0") )
        double Radius;
        // computed based upon whether or not the pencil size is relative to the object's transformation matrix
        double mRealSize;

        UPROPERTY( EditAnywhere, Category="Odyssey PathDrawing Tool" )
        bool Absolute;

        UPROPERTY( EditAnywhere, Category="Odyssey PathDrawing Tool" )
        bool Stitch;

        UPROPERTY(EditAnywhere,Category="Odyssey PathDrawing Tool")
        bool AverageStitchedRadius;

        UPROPERTY( EditAnywhere, Category="Odyssey PathDrawing Tool", meta = (ClampMin = "0.0", UIMin = "0.0") )
        double StitchingRadius;

    private:
        double mPointRadius;
        FOdysseyVectorPathBuilder* mPathBuilder;
        ::ULIS::FVec2D mOldPointInTexture;
        FOdysseyPainterEditorVectorPathDrawingToolHUD* mPathDrawingHUD;
        FOdysseyVectorVertex* mPreviousVertex;
        bool mStitched;


        std::vector<FOdysseyVectorVertex*> mVertexArray;
        std::vector<FOdysseyVectorSegment*> mSegmentArray;
};

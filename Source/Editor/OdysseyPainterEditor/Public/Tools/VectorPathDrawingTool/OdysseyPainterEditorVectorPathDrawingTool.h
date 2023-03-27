// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"

#include "OdysseyVector.h"

#include "OdysseyPainterEditorVectorPathDrawingTool.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorPathDrawingTool : public UOdysseyPainterEditorTool
{
    public:
        GENERATED_BODY()

        DECLARE_MULTICAST_DELEGATE(FSelectionChanged)
        FSelectionChanged mSelectionChanged;

    public:
        // Destructor
        virtual ~UOdysseyPainterEditorVectorPathDrawingTool();

        //Constructor
        UOdysseyPainterEditorVectorPathDrawingTool();
 
        //OdysseyPainterEditorTool overrides
        virtual void Activate() override;

        virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
        virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture) override;
        virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture) override;
        virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
        virtual void Commit() override;

    protected:
        void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent );
        void PropertyChanged( const FName& iPropertyName );
        FOdysseyVectorVertexCubic* PickVertex( FOdysseyVectorEngine* iVectorEngine
                                             , FOdysseyVectorScene* iScene
                                             , double iWorldX
                                             , double iWorldY
                                             , double iPickingRadius );
    private:
        FOdysseyVectorPathBuilder* MakePathBuilder( FOdysseyVectorEngine* iVectorEngine
                                                  , FOdysseyVectorScene* iScene
                                                  , double iWorldX
                                                  , double iWorldY );

    public:
        // Setters
        virtual bool CanDraw();

        UPROPERTY( EditAnywhere, Category="Odyssey PathDrawing Tool" )
        double Radius;
        // computed based upon whether or not the pencil size is relative to the object's transformation matrix
        double mRealSize;

        UPROPERTY( EditAnywhere, Category="Odyssey PathDrawing Tool" )
        bool Absolute;

        UPROPERTY( EditAnywhere, Category="Odyssey PathDrawing Tool" )
        bool Stitch;

        UPROPERTY( EditAnywhere, Category="Odyssey PathDrawing Tool" )
        double StitchingRadius;

    protected:

    private:
        FOdysseyVectorHUDPathDrawing mPathDrawingHUD;
        FOdysseyVectorVertexCubic* mPreviousVertex;
};

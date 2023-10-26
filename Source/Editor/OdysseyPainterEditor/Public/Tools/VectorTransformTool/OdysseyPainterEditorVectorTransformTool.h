// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionTool.h"
#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndoPointPosition.h"
#include "Undo/OdysseyVectorUndoObjectTransform.h"

#include "OdysseyPainterEditorVectorTransformTool.generated.h"

class FOdysseyPainterEditorVectorTransformToolHUD;

UCLASS( HideCategories = (SelectionTool) )
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorTransformTool : public UOdysseyPainterEditorVectorSelectionTool
{
    public:
        GENERATED_BODY()

    public:
        // Destructor
        virtual ~UOdysseyPainterEditorVectorTransformTool();

        //Constructor
        UOdysseyPainterEditorVectorTransformTool();

        virtual bool IsActivable() const override;

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
        void TranslateObjectSelection( FOdysseyVectorEngine* iEngine
                                     , FOdysseyVectorScene* iScene
                                     , const FOdysseyPoint& iPointInTexture );
        void RotateObjectSelection( FOdysseyVectorEngine* iEngine
                                  , FOdysseyVectorScene* iScene
                                  , const FOdysseyPoint& iPointInTexture );
        double GetRotationAngle( const FOdysseyPoint& iPointInTexture );
        void ScaleObjectSelection( FOdysseyVectorEngine* iEngine
                                 , FOdysseyVectorScene* iScene
                                 , const FOdysseyPoint& iPointInTexture );

    private:
        FOdysseyPainterEditorVectorTransformToolHUD* mTransformHUD;
        std::vector<FOdysseyVectorPoint*> mSelectedPoints;
        FOdysseyVectorUndo* mUndo;

        ::ULIS::FVec2D* mPickedPivot;
        bool mDragging;
        FVector2D mScreenMouseAtDown;

    public:
        UPROPERTY( EditAnywhere, Category = TransformTool, meta = (ClampMin = "0.0", UIMin = "0.0") )
        double PickingRadius;

        UPROPERTY( EditAnywhere, Category = TransformTool)
        bool Uniform;
        bool UniformAtKeyDown;

        UPROPERTY( EditAnywhere, Category = TransformTool )
        bool World;
};

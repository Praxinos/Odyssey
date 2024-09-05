// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionTool.h"

#include "OdysseyPainterEditorVectorTransformTool.generated.h"

class FOdysseyPainterEditorVectorTransformToolHUD;
class FOdysseyVectorTagInbetweener;
class FOdysseyVectorUndo;
class FInbetweenerBreakdown;

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

    private:
        void TranslateObjectSelection( FOdysseyVectorEngine* iEngine
                                     , FOdysseyVectorGroupPaint* iScene
                                     , const FOdysseyPoint& iPointInTexture );
        void RotateObjectSelection( FOdysseyVectorEngine* iEngine
                                  , FOdysseyVectorGroupPaint* iScene
                                  , const FOdysseyPoint& iPointInTexture );
        double GetRotationAngle( const FOdysseyPoint& iPointInTexture );
        void ScaleObjectSelection( FOdysseyVectorEngine* iEngine
                                 , FOdysseyVectorGroupPaint* iScene
                                 , const FOdysseyPoint& iPointInTexture );
        void GetTransformedObjectList( FOdysseyVectorGroupPaint* iScene
                                     , std::list<FOdysseyVectorObject*>& oObjectList );

    private:
        std::list<FInbetweenerBreakdown*> mTransformedBreakdownList;
        FOdysseyPainterEditorVectorTransformToolHUD* mTransformHUD;
        std::vector<FOdysseyVectorVertex*> mTransformedVertexArray;
        std::vector<FOdysseyVectorHandleSegment*> mTransformedHandleArray;
        FOdysseyVectorUndo* mUndo;

        ::ULIS::FVec2D* mPickedPivot;
        bool mDragging;
        FVector2D mScreenMouseAtDown;

    public:
        UPROPERTY( EditAnywhere
                 , Category = TransformTool
                 , meta = ( ToolTip  = "PickingRadius"
                          , ClampMin = "0.0"
                          , UIMin    = "0.0" ) )
        double PickingRadius;

        UPROPERTY( EditAnywhere
                 , Category = TransformTool
                 , meta = ( ToolTip = "Uniform" ) )
        bool Uniform;
        bool UniformAtKeyDown;

        UPROPERTY( EditAnywhere
                 , Category = TransformTool
                 , meta = ( ToolTip = "World" ) )
        bool World;
};

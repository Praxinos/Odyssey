// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"

#include "OdysseyPainterEditorVectorTransformTool.generated.h"

class FOdysseyPainterEditorVectorTransformToolHUD;
class FOdysseyVectorTagInbetweener;
class FOdysseyVectorUndo;
class FInbetweenerBreakdown;
class FOdysseyVectorObject;

UENUM()
enum class ETransformToolScalingCenter: uint8
{
    OppositeCorner = 0,
    BoxCenter = 1,
    Gizmo = 2
};

UCLASS( HideCategories = (SelectionTool) )
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorTransformTool : public UOdysseyPainterEditorVectorBaseTool
{
    struct TransformedObject
    {
        FOdysseyVectorObject* object;
        BLMatrix2D worldMatrix;
        BLMatrix2D parentInverseWorldMatrix;
    };

    struct TransformedBreakdown
    {
        FInbetweenerBreakdown* breakdown;
        BLMatrix2D worldMatrix;
        BLMatrix2D ownerInverseWorldMatrix;
    };

    public:
        GENERATED_BODY()

    public:
        // Destructor
        virtual ~UOdysseyPainterEditorVectorTransformTool();

        //Constructor
        UOdysseyPainterEditorVectorTransformTool();

        virtual bool IsActivable() const override;

        virtual FText GetTooltip() const override;

        virtual void ExtendToolbar( FToolBarBuilder& iBuilder ) override;

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual bool OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
                                      , const FKey& iKey, uint64& oSignalFlags ) override;
        virtual bool OnKeyUpVector( FOdysseyVectorGroupPaint* iScene, const FKey& iKey, uint64& oSignalFlags ) override;
        virtual bool OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey
                                        , uint64& oSignalFlags ) override;
        virtual void OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                         , const FOdysseyPoint& iPointInTexture, uint64& oSignalFlags ) override;
        virtual void OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture, uint64& oSignalFlags ) override;
        virtual bool OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey, uint64& oSignalFlags ) override;
        virtual uint64 PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                            , const FName& iPropertyName ) override;

    private:
        void TranslateObjectSelection( FOdysseyVectorGroupPaint* iScene
                                     , const FOdysseyPoint& iPointInTexture );
        void RotateObjectSelection( FOdysseyVectorGroupPaint* iScene
                                  , const FOdysseyPoint& iPointInTexture );
        double GetRotationAngle( const FOdysseyPoint& iPointInTexture );
        void ScaleObjectSelection( FOdysseyVectorGroupPaint* iScene
                                 , const FOdysseyPoint& iPointInTexture );
        void GetTransformedObjectList( FOdysseyVectorGroupPaint* iScene
                                     , std::list<FOdysseyVectorObject*>& oObjectList );

        EVisibility IsModeInbetween() const;
        void MakeSpaceMatrixForScaling();
        void MakeSpaceMatrixForRotation();
        void MakeSpaceMatrixForTranslation();
        void MakeSpaceMatrix();
        void TransformGizmo( BLMatrix2D& iTransformationMatrix );

    private:
        std::list<FInbetweenerBreakdown*> mTransformedBreakdownList;
        FOdysseyPainterEditorVectorTransformToolHUD* mTransformHUD;
        std::vector<FOdysseyVectorVertex*> mTransformedVertexArray;
        std::vector<::ULIS::FVec2D> mTransformedVertexPositionArray;
        std::vector<FOdysseyVectorHandleSegment*> mTransformedHandleArray;
        std::vector<::ULIS::FVec2D> mTransformedHandlePositionArray;
        FOdysseyVectorUndo* mUndo;

        ::ULIS::FVec2D* mPickedPivot;
        bool mDragging;
        FVector2D mScreenMouseAtDown;
        ::ULIS::FVec2D mMouseAtDown;
        BLMatrix2D mSpaceMatrix;
        BLMatrix2D mInverseSpaceMatrix;
        ::ULIS::FVec2D mSpaceGizmo;
        std::vector<TransformedObject> mTransformedObjectBuffer;
        std::vector<TransformedBreakdown> mTransformedBreakdownBuffer;

    public:
        UPROPERTY( EditAnywhere
                 , Category = TransformTool
                 , meta = ( ToolTip = "Show Inbetweens"
                          , EditCondition = "bInbetweenMode"
                          , EditConditionHides ) )
        eShowInbetweens ShowInbetweens;

        UPROPERTY( EditAnywhere
                 , Category=TransformTool
                 , meta = ( ToolTip  = "PickingRadius"
                          , ClampMin = "0"
                          , UIMin    = "0" ) )
        uint32 PickingRadius;

        UPROPERTY( EditAnywhere
                 , Category=TransformTool
                 , meta = ( ToolTip = "Uniform" ) )
        bool Uniform;
        bool UniformAtKeyDown;

        UPROPERTY( EditAnywhere
                 , Category=TransformTool
                 , meta = ( ToolTip = "Scaling Center" ) )
        ETransformToolScalingCenter ScalingCenter;

        UPROPERTY( EditAnywhere
                 , Category=TransformTool
                 , meta = ( ToolTip = "World" ) )
        bool World;
/*
        UPROPERTY( EditAnywhere
                 , Category=TransformTool
                 , meta = ( ToolTip = "KeepPathWidth" ) )
        bool KeepPathWidth;
*/
        UPROPERTY( EditDefaultsOnly
                 , Category = TransformTool )
        bool bInbetweenMode;
};

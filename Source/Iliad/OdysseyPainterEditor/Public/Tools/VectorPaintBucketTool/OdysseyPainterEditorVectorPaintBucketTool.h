// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"
#include "OdysseyPaintEngine.h"

#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndo.h"

#include "OdysseyPainterEditorVectorPaintBucketTool.generated.h"

class FOdysseyPaintEngine;
class FOdysseyPainterEditorVectorPaintBucketToolHUD;
class FOdysseyPainterEditorVectorPaintBucketToolContextMenu;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorPaintBucketTool : public UOdysseyPainterEditorVectorBaseTool
{
    GENERATED_BODY()

    public:
        //Inactivates the tool
        virtual bool IsActivable() const override;

        // Destructor
        virtual ~UOdysseyPainterEditorVectorPaintBucketTool();

        static FOdysseyVectorBucket& GetCopiedBucket();
        static void CopyBucketParam( FOdysseyVectorBucket* iSourceBucket );
        void PasteBucketParam( FOdysseyVectorBucket* iDestinationBucket );
        static void BucketProperties( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket );

        //Constructor
        UOdysseyPainterEditorVectorPaintBucketTool();

        bool GetShowControls();

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual bool OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                            , const FKey& iKey, uint64& oSignalFlags ) override;
        virtual bool OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                          , const FKey& iKey, uint64& oSignalFlags ) override;
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
        //virtual uint64 PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
        //                                  , const FName& iPropertyName ) override;
        virtual TSharedRef<SWidget> CreateTopTabWidget() override;

        virtual FText GetTooltip() const override;

    protected:
        virtual void ExtendContextMenu( FMenuBuilder& menu );

    private:
        void SetBucketColor( FOdysseyVectorBucket* iBucket );
        void OnMouseUpVectorClearBucket( FOdysseyVectorGroupPaint* iScene
                                       , FOdysseyVectorBucket* iBucket );
        void OnMouseUpVectorCreateBucket( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey );
        void OnMouseUpVectorRemoveBucket( FOdysseyVectorGroupPaint* iScene
                                        , FOdysseyVectorBucket* iBucket );
        void OnMouseUpVectorMovePoint( FOdysseyVectorGroupPaint* iScene
                                     , FOdysseyVectorPoint* iPoint
                                     , ::ULIS::FVec2D iPointOriginalPosition );
        void OnMouseUpVectorPropagateBucket( FOdysseyVectorGroupPaint* iScene
                                            , FOdysseyVectorBucket* iBucket
                                            , bool iPropagate );
        void OnMouseUpVectorColorBucket( FOdysseyVectorGroupPaint* iScene
                                        , FOdysseyVectorBucket* iBucket );
        void OnMouseDownVectorRotateBucket( FOdysseyVectorGroupPaint* iScene
                                          , FOdysseyVectorBucket* iBucket );
        double GetRotationAngle( FOdysseyVectorBucket* iBucket
                                , const FOdysseyPoint& iPointInTexture );

    public:
        UPROPERTY( EditAnywhere
                 , Category=PaintBucketTool
                 , meta = ( ToolTip  = "Propagate" ) )
        bool Propagate;

        UPROPERTY( EditAnywhere
                 , Category=PaintBucketTool
                 , meta = ( ToolTip  = "Color Mode" ) )
        eBucketColorMode ColorMode;

        UPROPERTY( EditAnywhere
                 , Category=PaintBucketTool
                 , meta = ( ToolTip  = "Opacity"
                          , ClampMin = "0.0"
                          , UIMin    = "0.0"
                          , ClampMax = "1.0"
                          , UIMax    = "1.0" ) )
        double Opacity;

        UPROPERTY( EditAnywhere
                 , Category=PaintBucketTool
                 , meta = ( ToolTip = "Color1"
                          , EditCondition = "(ColorMode == eBucketColorMode::LinearGradient) || (ColorMode == eBucketColorMode::RadialGradient)"
                          , EditConditionHides ) )
        FColor Color1;

        UPROPERTY( EditAnywhere
                 , Category=PaintBucketTool
                 , meta = ( ToolTip = "Color2"
                          , EditCondition = "(ColorMode == eBucketColorMode::LinearGradient) || (ColorMode == eBucketColorMode::RadialGradient)"
                          , EditConditionHides ) )
        FColor Color2;

        UPROPERTY( EditAnywhere
                 , Category=PaintBucketTool
                 , meta = ( ToolTip = "Picking Radius" ) )
        double PickingRadius;

    protected:
        FOdysseyVectorBucket* mPickedBucket;
        //FOdysseyVectorObject* mPickedObject;
        double mOldLocalMouseX;
        double mOldLocalMouseY;
        double mDownMouseX;
        double mDownMouseY;
        ::ULIS::FVec2D mPointPosition;
        double mPointRotation;
        FOdysseyPainterEditorVectorPaintBucketToolHUD* mBucketHUD;
        FOdysseyPainterEditorVectorPaintBucketToolContextMenu* mContextMenu;
        uint32 mPickedArea;
        ::ULIS::FVec2D mOldPointInTexture;
        bool mShowControls;
};

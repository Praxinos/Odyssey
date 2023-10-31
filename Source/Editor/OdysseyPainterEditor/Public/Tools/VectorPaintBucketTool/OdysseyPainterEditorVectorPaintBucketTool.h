// IDDN FR.001.250001.005.S.P.2019.000.00000
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
        static void PasteBucketParam( FOdysseyVectorBucket* iDestinationBucket );
        static void BucketProperties( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket );

        //Constructor
        UOdysseyPainterEditorVectorPaintBucketTool();

        bool GetShowControls();

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
        //virtual uint64 PropertyChangedVector( FOdysseyVectorScene* iScene
        //                                  , const FName& iPropertyName ) override;

    protected:
        virtual void ExtendContextMenu( FMenuBuilder& menu );

    private:
        void SetBucketColor( FOdysseyVectorBucket* iBucket );
        void OnMouseUpVectorClearBucket( FOdysseyVectorScene* iScene
                                       , FOdysseyVectorBucket* iBucket );
        void OnMouseUpVectorCreateBucket( FOdysseyVectorScene* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey );
        void OnMouseUpVectorRemoveBucket( FOdysseyVectorScene* iScene
                                        , FOdysseyVectorBucket* iBucket );
        void OnMouseUpVectorMovePoint( FOdysseyVectorScene* iScene
                                     , FOdysseyVectorPoint* iPoint
                                     , ::ULIS::FVec2D iPointOriginalPosition );
        void OnMouseUpVectorPropagateBucket( FOdysseyVectorScene* iScene
                                            , FOdysseyVectorBucket* iBucket
                                            , bool iPropagate );
        void OnMouseUpVectorColorBucket( FOdysseyVectorScene* iScene
                                        , FOdysseyVectorBucket* iBucket );
        void OnMouseDownVectorRotateBucket( FOdysseyVectorScene* iScene
                                          , FOdysseyVectorBucket* iBucket );
        double GetRotationAngle( FOdysseyVectorBucket* iBucket
                                , const FOdysseyPoint& iPointInTexture );

    public:
        UPROPERTY( EditAnywhere, Category = PaintBucketTool )
        bool Propagate;

        UPROPERTY( EditAnywhere, Category = PaintBucketTool )
        eBucketColorMode ColorMode;

        UPROPERTY( EditAnywhere, Category = PaintBucketTool )
        FColor Color1;

        UPROPERTY( EditAnywhere, Category = PaintBucketTool )
        FColor Color2;

        UPROPERTY( EditAnywhere, Category = PaintBucketTool )
        double PickingRadius;

        UPROPERTY( EditAnywhere, Category = PaintBucketTool )
        bool RestrictToSelectedPaintGroups;

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

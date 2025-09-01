// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"
#include "OdysseyPaintEngine.h"

#include "OdysseyPainterEditorVectorPaintBucketTool.generated.h"

class FOdysseyPaintEngine;
class FOdysseyPainterEditorVectorPaintBucketToolHUD;
class FOdysseyPainterEditorVectorPaintBucketToolContextMenu;
class FOdysseyVectorBucket;
class FOdysseyPainterEditor;

UENUM()
enum class EPaintBucketToolColorMode : uint8
{
    Color = 0,
    LinearGradient = 1,
    RadialGradient = 2,
};

UENUM()
enum class eVectorPaintBucketEditionMode : uint8
{
    Default = 0,
    Control = 1,
    Remove = 2
};

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

        eVectorPaintBucketEditionMode GetEditionMode();

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual bool OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                          , const FKeyEvent& InKeyEvent ) override;
        virtual bool OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                        , const FKeyEvent& InKeyEvent ) override;
        virtual bool OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey ) override;
        virtual void OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                         , const FOdysseyPoint& iPointInTexture ) override;
        virtual void OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture ) override;
        virtual bool OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey ) override;
        //virtual uint64 PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
        //                                  , const FName& iPropertyName ) override;

        virtual FText GetTooltip() const override;

        virtual void ExtendToolbar( FToolBarBuilder& iBuilder ) override;

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
        void OnMouseUpVectorPropagateBucket( FOdysseyVectorGroupPaint* iScene
                                           , FOdysseyVectorBucket* iBucket
                                           , bool iPropagate );
        void OnMouseUpVectorColorBucket( FOdysseyVectorGroupPaint* iScene
                                       , FOdysseyVectorBucket* iBucket );
        double GetRotationAngle( FOdysseyVectorBucket* iBucket
                                , const FOdysseyPoint& iPointInTexture );
        TSharedRef<SWidget> CreateModifierSegmentControl();
        const FSlateBrush* GetBackgroundColor( eVectorPaintBucketEditionMode iMode ) const;
        void SetEditionMode( eVectorPaintBucketEditionMode iMode );

    public:
        UPROPERTY( EditAnywhere
                 , Category=PaintBucketTool
                 , meta = ( ToolTip  = "Propagate" ) )
        bool Propagate;

        UPROPERTY( EditAnywhere
                 , Category=PaintBucketTool
                 , meta = ( ToolTip  = "Color Mode" ) )
        EPaintBucketToolColorMode ColorMode;

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
                          , EditCondition = "(ColorMode == EPaintBucketToolColorMode::LinearGradient) || (ColorMode == EPaintBucketToolColorMode::RadialGradient)"
                          , EditConditionHides ) )
        FColor Color1;

        UPROPERTY( EditAnywhere
                 , Category=PaintBucketTool
                 , meta = ( ToolTip = "Color2"
                          , EditCondition = "(ColorMode == EPaintBucketToolColorMode::LinearGradient) || (ColorMode == EPaintBucketToolColorMode::RadialGradient)"
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
        FOdysseyPainterEditorVectorPaintBucketToolHUD* mBucketHUD;
        FOdysseyPainterEditorVectorPaintBucketToolContextMenu* mContextMenu;
        uint32 mPickedArea;
        ::ULIS::FVec2D mOldPointInTexture;
        eVectorPaintBucketEditionMode mEditionMode;
};

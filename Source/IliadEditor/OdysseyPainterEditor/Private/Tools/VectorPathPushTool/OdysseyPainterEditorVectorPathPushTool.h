// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyVectorPoint.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"
#include <ULIS>

#include "OdysseyPainterEditorVectorPathPushTool.generated.h"

class FOdysseyPainterEditorVectorPathPushToolHUD;

typedef struct _FPushedPoint
{
     FOdysseyVectorPoint* point;
     double distance;
     FOdysseyVectorSegment* smoothingGuideSegment;
     ::ULIS::FVec2D originalCoords;

     _FPushedPoint( FOdysseyVectorPoint* iPoint
                  , double iDistance
                  , FOdysseyVectorSegment* iSmoothingGuideSegment )
     {
         point = iPoint;
         distance = iDistance;
         smoothingGuideSegment = iSmoothingGuideSegment;
         originalCoords = iPoint->GetCoords();
     }
} FPushedPoint;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorPathPushTool : public UOdysseyPainterEditorVectorBaseTool
{
    public:
        GENERATED_BODY()

    public:
        // Destructor
        virtual ~UOdysseyPainterEditorVectorPathPushTool();

        //Constructor
        UOdysseyPainterEditorVectorPathPushTool();

        virtual bool IsActivable() const override;

        virtual FText GetTooltip() const override;

        virtual void ExtendToolbar( FToolBarBuilder& iBuilder ) override;

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        //virtual bool OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
        //                              , const FKey& iKey ) override;
        //virtual bool OnKeyUpVector( FOdysseyVectorGroupPaint* iScene, const FKey& iKey ) override;
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
        //virtual void PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
        //                                  , const FName& iPropertyName ) override;

    private:
        FPushedPoint* GetPushedPoint( FOdysseyVectorPoint* iPoint );
        std::vector<FPushedPoint> mPushedPointArray;
        std::vector<FOdysseyVectorSegment*> mSegmentArray;
        //FOdysseyVectorHUDPicking mPickingHUD;
        FOdysseyPainterEditorVectorPathPushToolHUD *mPathPushHUD;
        double mMaxDistance;

    public:
        UPROPERTY( EditAnywhere
                 , Category=PathPushTool
                 , meta = ( ToolTip  = "Radius"
                          , ClampMin = "0"
                          , UIMin    = "0" ) )
        uint32 Radius;

        UPROPERTY( EditAnywhere
                 , Category=PathPushTool
                 , meta = ( ToolTip  = "Restrict To Selected Objects" ) )
        bool RestrictToSelectedObjects;
};

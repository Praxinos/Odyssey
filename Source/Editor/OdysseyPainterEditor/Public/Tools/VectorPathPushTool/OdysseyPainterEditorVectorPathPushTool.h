// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"
#include "OdysseyVector.h"

#include "OdysseyPainterEditorVectorPathPushTool.generated.h"

class FOdysseyPainterEditorVectorPathPushToolHUD;

typedef struct _FPushedPoint
{
     FOdysseyVectorPoint* point;
     double distance;
     FOdysseyVectorSegment* smoothingGuideSegment;

     _FPushedPoint( FOdysseyVectorPoint* iPoint
                  , double iDistance
                  , FOdysseyVectorSegment* iSmoothingGuideSegment )
     {
         point = iPoint;
         distance = iDistance;
         smoothingGuideSegment = iSmoothingGuideSegment;
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

        virtual TSharedRef<SWidget> CreateTopTabWidget() override;

        virtual FText GetTooltip() const override;

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        //virtual bool OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
        //                              , const FKey& iKey, uint64& oSignalFlags ) override;
        //virtual bool OnKeyUpVector( FOdysseyVectorGroupPaint* iScene, const FKey& iKey, uint64& oSignalFlags ) override;
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
                          , ClampMin = "0.0"
                          , UIMin    = "0.0" ) )
        double Radius;

        UPROPERTY( EditAnywhere
                 , Category=PathPushTool
                 , meta = ( ToolTip  = "Restrict To Selected Objects" ) )
        bool RestrictToSelectedObjects;
};

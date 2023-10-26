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
     double ratio;
     bool isSmooth;
     FOdysseyVectorSegment* smoothingGuideSegment;

     _FPushedPoint( FOdysseyVectorPoint* iPoint
                  , double iRatio
                  , bool iIsSmooth
                  , FOdysseyVectorSegment* iSmoothingGuideSegment )
     {
         point = iPoint;
         ratio = iRatio;
         isSmooth = iIsSmooth;
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

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorScene* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorScene* iScene ) override;
        //virtual uint64 OnKeyDownVector( FOdysseyVectorScene* iScene
        //                              , const FKey& iKey ) override;
        //virtual uint64 OnKeyUpVector( FOdysseyVectorScene* iScene, const FKey& iKey ) override;
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
        //virtual void PropertyChangedVector( FOdysseyVectorScene* iScene
        //                                  , const FName& iPropertyName ) override;

    private:
        FPushedPoint* GetPushedPoint( FOdysseyVectorPoint* iPoint );
        std::vector<FPushedPoint> mPushedPointArray;
        std::vector<FOdysseyVectorSegment*> mSegmentArray;
        //FOdysseyVectorHUDPicking mPickingHUD;
        FOdysseyPainterEditorVectorPathPushToolHUD *mPathPushHUD;

    public:
        UPROPERTY( EditAnywhere, Category = PathPushTool, meta = (ClampMin = "0.0", UIMin = "0.0"))
        double Radius;

        UPROPERTY( EditAnywhere, Category = PathPushTool )
        bool PreserveSmoothness;
};

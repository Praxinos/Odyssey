// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"

#include "OdysseyPainterEditorVectorTrajectoryTool.generated.h"

struct FInbetweenerInbetween;
class FInbetweenerHandleTrajectory;
class FOdysseyPainterEditorVectorTrajectoryToolHUD;

UENUM()
enum class eTrajectoryPickingMode : uint8
{
    Add = 0,
    Alter = 1,
    Remove = 2
};

UCLASS( HideCategories = (SelectionTool) )
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorTrajectoryTool : public UOdysseyPainterEditorVectorBaseTool
{
    public:
        GENERATED_BODY()

    public:
        // Destructor
        virtual ~UOdysseyPainterEditorVectorTrajectoryTool();

        virtual bool IsActivable() const override;

        UOdysseyPainterEditorVectorTrajectoryTool();

        virtual TSharedRef<SWidget> CreateTopTabWidget() override;

        virtual FText GetTooltip() const override;

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                            , const FKey& iKey ) override;
        virtual uint64 OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                          , const FKey& iKey ) override;
        //virtual bool OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
        //                            , const FKey& iKey ) override;
        //virtual bool OnKeyUpVector( FOdysseyVectorGroupPaint* iScene, const FKey& iKey ) override;
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
        FOdysseyPainterEditorVectorTrajectoryToolHUD* mTrajectoryHUD;
        FInbetweenerHandleTrajectory* mPickedHandle;
        eTrajectoryPickingMode mPickingMode;

    public:
        UPROPERTY( EditAnywhere
                 , Category = TrajectoryTool
                 , meta = ( ToolTip  = "Picking Radius"
                          , ClampMin = "0.0"
                          , UIMin    = "0.0" ) )
        double PickingRadius;
};

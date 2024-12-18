// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"

#include "OdysseyPainterEditorVectorTrajectoryTool.generated.h"

struct FInbetweenerInbetween;
class FInbetweenerHandleTrajectory;
class FOdysseyPainterEditorVectorTrajectoryToolHUD;
class FInbetweenerQuad;
class FInbetweenerWaypoint;
class FInbetweenerStep;
class FInbetweenerRoute;


UENUM()
enum class eTrajectoryEditionMode : uint8
{
    Add     = 0 UMETA( ToolTip = "Add (default)" ),
    Curve   = 1 UMETA( ToolTip = "Curve (Ctrl/Cmd)" ),
    Spacing = 2 UMETA( ToolTip = "Spacing (Shift)" ),
    //Remove  = 3 UMETA( ToolTip = "Remove (Alt)" )
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

        FInbetweenerQuad* GetHoveredQuad();
        void ResetHoveredQuad();

        virtual TSharedRef<SWidget> CreateTopTabWidget() override;

        virtual FText GetTooltip() const override;

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual bool OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                            , const FKey& iKey
                                            , uint64& oSignalFlags ) override;
        virtual bool OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                            , const FKey& iKey
                                            , uint64& oSignalFlags ) override;
        //virtual bool OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
        //                            , const FKey& iKey ) override;
        //virtual bool OnKeyUpVector( FOdysseyVectorGroupPaint* iScene, const FKey& iKey ) override;
        virtual bool OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey
                                        , uint64& oSignalFlags ) override;
        virtual void OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , uint64& oSignalFlags ) override;
        virtual void OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , uint64& oSignalFlags ) override;
        virtual bool OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey
                                        , uint64& oSignalFlags ) override;
        virtual uint64 PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                            , const FName& iPropertyName ) override;
        virtual EMouseCursor::Type GetMouseCursor() const override;
        virtual void ExtendContextMenuInbetween( FOdysseyVectorGroupPaint* iScene
                                               , FMenuBuilder& menu
                                               , uint64 iInbetweenMenuFlags ) override;
        void ResetRoute();
        void DeleteRoute();

    private:
        FOdysseyPainterEditorVectorTrajectoryToolHUD* mTrajectoryHUD;
        FInbetweenerHandleTrajectory* mPickedHandle;
        FInbetweenerStep* mPickedStep;
        FInbetweenerWaypoint* mPickedWaypoint;
        FInbetweenerRoute* mPickedRoute;
        FInbetweenerQuad* mHoveredQuad;

    public:
        UPROPERTY( EditAnywhere
                 , Category = TrajectoryTool
                 , meta = ( ToolTip  = "Picking Radius"
                          , ClampMin = "0.0"
                          , UIMin    = "0.0" ) )
        double PickingRadius;

        UPROPERTY( EditAnywhere
                 , Category = MatchingTool
                 , meta = ( ToolTip  = "Show Inbetweens" ) )
        bool ShowInbetweens;

        UPROPERTY( EditAnywhere
                 , Category = MatchingTool
                 , meta = ( ToolTip  = "Edition Mode" ) )
        eTrajectoryEditionMode EditionMode;
        eTrajectoryEditionMode EditionModeAtKeyDown;
};

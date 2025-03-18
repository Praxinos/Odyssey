// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

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
enum class eVectorTrajectoryEditionMode : uint8
{
    Add     = 0 UMETA( DisplayName = "Add (default)" ),
    Curve   = 1 UMETA( DisplayName = "Curve (Ctrl/Cmd)" ),
    Spacing = 2 UMETA( DisplayName = "Spacing (Shift)" ),
    //Remove  = 3 UMETA( ToolTip = "Remove (Alt)" )
};

UENUM()
enum class eVectorTrajectoryGridDisplayMode : uint8
{
    AsQuads  = 0 UMETA( ToolTip = "Show grid as quads" ),
    AsPoints = 1 UMETA( ToolTip = "Show grid as points" )
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

        virtual FText GetTooltip() const override;

        virtual void ExtendToolbar( FToolBarBuilder& iBuilder ) override;

        eVectorTrajectoryEditionMode GetEditionMode();

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual bool OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                          , const FKeyEvent& InKeyEvent
                                          , uint64& oSignalFlags ) override;
        virtual bool OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                        , const FKeyEvent& InKeyEvent
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
        void SetEditionMode( eVectorTrajectoryEditionMode iMode );
        const FSlateBrush* GetBackgroundColor( eVectorTrajectoryEditionMode iMode ) const;
        TSharedRef<SWidget> CreateModifierSegmentControl();

    private:
        FOdysseyPainterEditorVectorTrajectoryToolHUD* mTrajectoryHUD;
        FInbetweenerHandleTrajectory* mPickedHandle;
        FInbetweenerStep* mPickedStep;
        FInbetweenerWaypoint* mPickedWaypoint;
        FInbetweenerRoute* mPickedRoute;
        FInbetweenerQuad* mHoveredQuad;
        eVectorTrajectoryEditionMode mEditionMode;

    public:
        UPROPERTY( EditAnywhere
                 , Category = TrajectoryTool
                 , meta = ( ToolTip  = "Picking Radius"
                          , ClampMin = "1"
                          , UIMin    = "1"
                          , ClampMax = "100"
                          , UIMax    = "100" ) )
        uint32 PickingRadius;

        UPROPERTY( EditAnywhere
                 , Category = MatchingTool
                 , meta = ( ToolTip  = "Show Inbetweens" ) )
        bool ShowInbetweens;

        UPROPERTY( EditAnywhere
                 , Category = MatchingTool
                 , meta = ( ToolTip  = "Grid Display Mode" ) )
        eVectorTrajectoryGridDisplayMode GridDisplayMode;
};

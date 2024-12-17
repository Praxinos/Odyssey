// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"

#include "InbetweenerTag/InbetweenerChart.h"

#include "OdysseyPainterEditorVectorChartTool.generated.h"

struct FInbetweenerDrawing;
class FOdysseyPainterEditorVectorChartToolHUD;

UENUM()
enum class eChartType : uint8
{
    Full    = 0,
    Partial = 1
};

UENUM()
enum class eChartPickingMode : uint8
{
    Default = 0,
    Control = 1,
    Shift   = 2
};

UENUM()
enum class eChartShiftingOp : uint8
{
    Relative     = 0,
    EaseInOrOut  = 1,
    EaseInAndOut = 2
};

UCLASS( HideCategories = (SelectionTool) )
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorChartTool : public UOdysseyPainterEditorVectorBaseTool
{
    public:
        GENERATED_BODY()

    public:
        // Destructor
        virtual ~UOdysseyPainterEditorVectorChartTool();

        virtual bool IsActivable() const override;

        UOdysseyPainterEditorVectorChartTool();

        virtual TSharedRef<SWidget> CreateTopTabWidget() override;

        virtual FText GetTooltip() const override;

        eChartPickingMode GetPickingMode();
        FInbetweenerChart::Inbetween* GetHoveredInbetween();

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;

        virtual bool OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                            , const FKey& iKey
                                            , uint64& oSignalFlags ) override;
        virtual bool OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
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

    private:
        FOdysseyPainterEditorVectorChartToolHUD* mChartHUD;
        FInbetweenerChart::Inbetween* mPickedInbetween;
        FInbetweenerBreakdown* mPickedBreakdown;
        FInbetweenerChart::Inbetween* mHoveredInbetween;
        FInbetweenerChart::HUDBezier::Point* mPickedBezierPoint;
        eChartPickingMode mPickingMode;
        ::ULIS::FVec2D mMouseAtDown;
        float mEasing;

    public:
        UPROPERTY( EditAnywhere
                 , Category = ChartTool
                 , meta = ( ToolTip  = "Picking Radius"
                          , ClampMin = "0.0"
                          , UIMin    = "0.0" ) )
        double PickingRadius;

        UPROPERTY( EditAnywhere
                 , Category = ChartTool
                 , meta = ( ToolTip  = "Shifting Mode" ) )
        eChartShiftingOp ShiftingOp;

        UPROPERTY( EditAnywhere
                 , Category = ChartTool
                 , meta = ( ToolTip  = "Chart Type" ) )
        eChartType ChartType;
};

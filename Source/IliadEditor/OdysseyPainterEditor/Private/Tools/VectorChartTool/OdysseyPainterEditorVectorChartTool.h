// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"

#include "InbetweenerTag/InbetweenerChart.h"

#include "OdysseyPainterEditorVectorChartTool.generated.h"

struct FInbetweenerDrawing;
class FOdysseyPainterEditorVectorChartToolHUD;
class FOdysseyVectorUndo;

UENUM()
enum class eChartType : uint8
{
    Full    = 0,
    Partial = 1
};

UENUM()
enum class eVectorChartEditionMode : uint8
{
    OneByOne     = 0 UMETA( ToolTip = "One by one (default)" ),
    Relative     = 1 UMETA( ToolTip = "Relative (Ctrl/Cmd)" ),
    EaseInOrOut  = 2 UMETA( ToolTip = "Shift" ),
    Magnet       = 3 UMETA( ToolTip = "Alt" ),
    Reshape      = 4 UMETA( ToolTip = "Ctrl + Shift" ),
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

        virtual FText GetTooltip() const override;

        FInbetweenerChart::Inbetween* GetHoveredInbetween();

        virtual void ExtendToolbar( UToolMenu* iToolMenu ) override;
        eVectorChartEditionMode GetEditionMode();
        virtual bool HasRadius() const override;
        virtual float GetRadius() const override;
        virtual void SetRadius(float iRadius)override;

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;

        virtual bool OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                        , const FKeyEvent& InKeyEvent ) override;
        virtual bool OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                          , const FKeyEvent& InKeyEvent ) override;
        //virtual bool OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
        //                            , const FKey& iKey ) override;
        //virtual bool OnKeyUpVector( FOdysseyVectorGroupPaint* iScene, const FKey& iKey ) override;
        virtual bool OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                      , const FOdysseyPoint&
                                      , const FKey& iKey ) override;
        virtual void OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture ) override;
        virtual void OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture ) override;
        virtual bool OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey ) override;
        virtual void PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                            , const FName& iPropertyName ) override;
        TSharedRef<SWidget> CreateModifierSegmentControl();
        void SetEditionMode( eVectorChartEditionMode iMode );
        const FSlateBrush* GetBackgroundColor( eVectorChartEditionMode iMode ) const;
        void OnVectorLayerUpdate( const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags
                                , uint32 iUpdateFlags );

    private:
        FOdysseyPainterEditorVectorChartToolHUD* mChartHUD;
        FInbetweenerChart::Inbetween* mPickedInbetween;
        FInbetweenerBreakdown* mPickedBreakdown;
        FInbetweenerChart::Inbetween* mHoveredInbetween;
        FInbetweenerChart::HUDBezier::Point* mPickedBezierPoint;
        ::ULIS::FVec2D mMouseAtDown;
        float mStrength;
        FOdysseyVectorUndo* mUndo;

    public:
        UPROPERTY( EditAnywhere
                 , Category = ChartTool
                 , meta = ( ToolTip  = "Picking Radius"
                          , ClampMin = "0"
                          , UIMin    = "0"
                          , LinearDeltaSensitivity = "15"
                          , Delta = "1" ) )
        uint32 PickingRadius;

        UPROPERTY()
        eVectorChartEditionMode EditionMode;

        UPROPERTY( EditAnywhere
                 , Category = ChartTool
                 , meta = ( ToolTip  = "Factor"
                          , ClampMin = "1"
                          , UIMin    = "1"
                          , ClampMax = "5"
                          , UIMax    = "5"
                          , EditCondition = "( EditionMode == eVectorChartEditionMode::EaseInOrOut ) || ( EditionMode == eVectorChartEditionMode::Magnet )"
                          , EditConditionHides ) )
        uint32 Factor;

        UPROPERTY( EditAnywhere
                 , Category = ChartTool
                 , meta = ( ToolTip  = "Chart Type" ) )
        eChartType ChartType;
};

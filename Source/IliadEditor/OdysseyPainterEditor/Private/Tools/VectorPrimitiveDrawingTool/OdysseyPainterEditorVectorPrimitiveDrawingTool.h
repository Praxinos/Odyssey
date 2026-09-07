// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyShape.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"
#include "OdysseyVectorBrush.h"

#include "OdysseyPainterEditorVectorPrimitiveDrawingTool.generated.h"


class FOdysseyVectorLine;
class FOdysseyVectorPrimitive;

UENUM()
enum class EOdysseyVectorPrimitiveType : uint8
{
    Ellipse,
    Rectangle,
    Line
};

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorPrimitiveDrawingTool : public UOdysseyPainterEditorVectorBaseTool
{
    public:
        GENERATED_BODY()

    public:
        // Destructor
        virtual ~UOdysseyPainterEditorVectorPrimitiveDrawingTool();

        //Constructor
        UOdysseyPainterEditorVectorPrimitiveDrawingTool();

        virtual bool IsActivable() const override;

        virtual FText GetTooltip() const override;

        virtual void ExtendToolbar( UToolMenu* iToolMenu ) override;

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual bool OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
                                    , const FKey& iKey ) override;
        virtual bool OnKeyUpVector( FOdysseyVectorGroupPaint* iScene, const FKey& iKey ) override;
        virtual bool OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey ) override;
        //virtual void OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
        //                                 , const FOdysseyPoint& iPointInTexture ) override;
        virtual void OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                      , const FOdysseyPoint& iPointInTexture ) override;
        virtual bool OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                    , const FOdysseyPoint& iPointInTexture
                                    , const FKey& iKey ) override;
        //virtual uint64 PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
        //                                    , const FName& iPropertyName ) override;

        FOdysseyVectorObject* GetParentObject( FOdysseyVectorGroupPaint* iScene );

    private:
        double GetLineRotationAngle( FOdysseyVectorLine* iLine, const FOdysseyPoint& iPointInTexture );

    public:
        UPROPERTY( EditAnywhere
                 , Category = "Shape" )
        FOdysseyShapes Shapes;

        UPROPERTY( EditAnywhere
                 , Category = "Parameters"
                 , meta = ( ToolTip = "Brush" ) )
        FOdysseyVectorBrush Brush;

        UPROPERTY( EditAnywhere
                 , Category = "Parameters"
                 , meta = ( ToolTip  = "Stroke Width"
                          , ClampMin = "0.0"
                          , UIMin    = "0.0"
                          , LinearDeltaSensitivity = "15"
                          , Delta = "1" ) )
        double StrokeWidth;

        UPROPERTY( EditAnywhere
                 , Category = "Parameters"
                 , meta = ( ToolTip  = "Uniform" ) )
        bool Uniform;
        bool UniformAtKeyDown;

        FOdysseyVectorPrimitive* mPrimitive;
        ::ULIS::FVec2D mMouseDown;
        uint32 mRectangleNumber;
        uint32 mLineNumber;
        uint32 mEllipseNumber;
};

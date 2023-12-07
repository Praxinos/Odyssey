// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"
#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyPainterEditorVectorPrimitiveDrawingTool.generated.h"

UENUM()
enum class ePrimitiveDrawingToolColorSource : uint8
{
    ColorWheel = uint8(eBucketColorMode::SolidColor),
    Palette  = uint8(eBucketColorMode::Palette)
};

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

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
                                      , const FKey& iKey ) override;
        virtual uint64 OnKeyUpVector( FOdysseyVectorGroupPaint* iScene, const FKey& iKey ) override;
        virtual uint64 OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey ) override;
        //virtual uint64 OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
        //                                 , const FOdysseyPoint& iPointInTexture ) override;
        virtual uint64 OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture ) override;
        virtual uint64 OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey ) override;
        //virtual uint64 PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
        //                                    , const FName& iPropertyName ) override;

    private:
        double GetLineRotationAngle( FOdysseyVectorLine* iLine, const FOdysseyPoint& iPointInTexture );

    public:
        UPROPERTY( EditAnywhere, Category = PrimitiveDrawingTool )
        EOdysseyVectorPrimitiveType PrimitiveType;

        UPROPERTY( EditAnywhere, Category = PrimitiveDrawingTool )
        eBaseToolColorSource ColorSource;

        UPROPERTY( EditAnywhere, Category = PrimitiveDrawingTool, meta = (ClampMin = "0.0",UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0" ))
        double Opacity;

        UPROPERTY( EditAnywhere, Category = PrimitiveDrawingTool )
        FOdysseyVectorBrush Brush;

        UPROPERTY( EditAnywhere, Category = PrimitiveDrawingTool , meta = (ClampMin = "0.0", UIMin = "0.0"))
        double StrokeWidth;

        UPROPERTY( EditAnywhere, Category = PrimitiveDrawingTool )
        bool Uniform;
        bool UniformAtKeyDown;

       ::ULIS::FVec2D mMouseDown;
};

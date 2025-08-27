// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyShape.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"
#include <ULIS>

#include "OdysseyPainterEditorVectorSelectionTool.generated.h"

UENUM()
enum class EOdysseyVectorSelectionShape : uint8
{
    Freehand,
    Circle,
    Rectangle
};

class FOdysseyPainterEditorVectorBaseToolHUD;
class FOdysseyPainterEditorVectorSelectionToolHUD;
class FOdysseyVectorEngine;
class FOdysseyVectorGroupPaint;
class FOdysseyVectorBucket;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorSelectionTool : public UOdysseyPainterEditorVectorBaseTool
{
    public:
        GENERATED_BODY()

    public:
        static bool DoubleClicked();
        // Destructor
        virtual ~UOdysseyPainterEditorVectorSelectionTool();

        //Constructor
        UOdysseyPainterEditorVectorSelectionTool();

        //Constructor
        UOdysseyPainterEditorVectorSelectionTool( TSharedPtr<FOdysseyPainterEditorVectorBaseToolHUD> iHUD );

        virtual bool IsActivable() const override;

        virtual FText GetTooltip() const override;

        virtual void ExtendToolbar( FToolBarBuilder& iBuilder ) override;

        std::vector<::ULIS::FVec2D>& GetPointArray();

        EOdysseyVectorSelectionShape GetSelectionShape();

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        //virtual bool OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
        //                            , const FKey& iKey, uint64& oSignalFlags ) override;
        //virtual bool OnKeyUpVector( FOdysseyVectorGroupPaint* iScene, const FKey& iKey, uint64& oSignalFlags ) override;
        virtual bool OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey
                                        , uint64& oSignalFlags ) override;
        //virtual void OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
        //                                , const FOdysseyPoint& iPointInTexture, uint64& oSignalFlags ) override;
        virtual void OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture, uint64& oSignalFlags ) override;
        virtual bool OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey, uint64& oSignalFlags ) override;
        //virtual uint64 PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
        //                                  , const FName& iPropertyName ) override;

        void Copy( FOdysseyVectorEngine* iEngine, FOdysseyVectorGroupPaint* iScene );
        void Paste( FOdysseyVectorEngine* iEngine, FOdysseyVectorGroupPaint* iScene );

protected:
    void OnMouseUpVectorObjectMode( FOdysseyVectorGroupPaint* iScene
                                  , const FOdysseyPoint& iPointInTexture
                                  , const FKey& iKey );
    void OnMouseUpVectorVertexMode( FOdysseyVectorGroupPaint* iScene
                                  , const FOdysseyPoint& iPointInTexture
                                  , const FKey& iKey );
    ::ULIS::FRectD GenerateMask();

protected:
    FOdysseyPainterEditorVectorSelectionToolHUD* mPickHUD;
    std::vector<::ULIS::FVec2D> mPointArray;
    ::ULIS::FVec2D mPressedMouseCoords;

public:
    UPROPERTY( EditAnywhere
             , Category = "Shape" )
    FOdysseyShapes Shapes;
};

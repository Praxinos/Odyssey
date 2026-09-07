// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyShape.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"
#include <ULIS>

#include "OdysseyPainterEditorVectorCutTool.generated.h"

UENUM()
enum class EOdysseyVectorCutShape : uint8
{
    Freehand,
    Circle,
    Rectangle,
    Line
};

class FOdysseyPainterEditorVectorBaseToolHUD;
class FOdysseyPainterEditorVectorCutToolHUD;
class FOdysseyVectorEngine;
class FOdysseyVectorGroupPaint;
class FOdysseyVectorBucket;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorCutTool : public UOdysseyPainterEditorVectorBaseTool
{
    public:
        GENERATED_BODY()

    public:
        static bool DoubleClicked();
        // Destructor
        virtual ~UOdysseyPainterEditorVectorCutTool();

        //Constructor
        UOdysseyPainterEditorVectorCutTool();

        virtual bool IsActivable() const override;

        virtual FText GetTooltip() const override;

        virtual void ExtendToolbar( UToolMenu* iToolMenu ) override;

        std::vector<::ULIS::FVec2D>& GetPointArray();

        EOdysseyVectorCutShape GetCutShape();

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        //virtual bool OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
        //                            , const FKey& iKey ) override;
        //virtual bool OnKeyUpVector( FOdysseyVectorGroupPaint* iScene, const FKey& iKey ) override;
        virtual bool OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey ) override;
        //virtual void OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
        //                                , const FOdysseyPoint& iPointInTexture ) override;
        virtual void OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture ) override;
        virtual bool OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey ) override;
        virtual void OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                       , const FOdysseyPoint& iPointInTexture ) override;

        //virtual uint64 PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
        //                                  , const FName& iPropertyName ) override;

        void Copy( FOdysseyVectorEngine* iEngine, FOdysseyVectorGroupPaint* iScene );
        void Paste( FOdysseyVectorEngine* iEngine, FOdysseyVectorGroupPaint* iScene );

        void CutPaths( FOdysseyVectorGroupPaint* iScene
                     , const ::ULIS::FRectD& iErasureArea
                     , std::vector<FOdysseyVectorObject*>& oAddedObjectArray
                     , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                     , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                     , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                     , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                     , std::vector<FOdysseyVectorObject*>& oRemovedObjectArray );

    protected:
        uint64 OnMouseUpVectorObjectMode( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey );
        ::ULIS::FRectD GenerateMask( bool iStroke );

    protected:
        FOdysseyPainterEditorVectorCutToolHUD* mPickHUD;
        std::vector<::ULIS::FVec2D> mPointArray;
        bool mIsLayerHovered = false;

    public:
        UPROPERTY( EditAnywhere
                 , Category = "Shape" )
        FOdysseyShapes Shapes;
};

// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"
#include "OdysseyVector.h"
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
        UOdysseyPainterEditorVectorSelectionTool( FOdysseyPainterEditorVectorBaseToolHUD* iHUD );

        virtual bool IsActivable() const override;

        std::vector<::ULIS::FVec2D>& GetPointArray();

        EOdysseyVectorSelectionShape GetSelectionShape();

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        //virtual uint64 OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
        //                            , const FKey& iKey ) override;
        //virtual uint64 OnKeyUpVector( FOdysseyVectorGroupPaint* iScene, const FKey& iKey ) override;
        virtual uint64 OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey ) override;
        //virtual uint64 OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
        //                                , const FOdysseyPoint& iPointInTexture ) override;
        virtual uint64 OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture ) override;
        virtual uint64 OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey ) override;
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

    void PickVertexFromPath( FOdysseyVectorPath* iPath
                           , std::vector<FOdysseyVectorVertex*>& oPickedVertexArray );

    void PickBucketFromPaintGroup( FOdysseyVectorGroupPaint* iPaintGroup
                                 , std::vector<FOdysseyVectorBucket*>& oPickedBucketArray );

protected:
    FOdysseyPainterEditorVectorSelectionToolHUD* mPickHUD;
    std::vector<::ULIS::FVec2D> mPointArray;
    ::ULIS::FVec2D mPressedMouseCoords;

public:
    UPROPERTY( EditAnywhere, Category = SelectionTool )
    EOdysseyVectorSelectionShape SelectionShape;
};

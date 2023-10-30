// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"
#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndoSelect.h"
#include "OdysseyPainterEditorVectorSelectionTool.generated.h"

UENUM()
enum class EOdysseyVectorSelectionShape : uint8
{
    Freehand,
    Circle,
    Rectangle
};

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

        virtual bool IsActivable() const override;

        std::vector<::ULIS::FVec2D>& GetPointArray();

        EOdysseyVectorSelectionShape GetSelectionShape();

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorScene* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorScene* iScene ) override;
        //virtual uint64 OnKeyDownVector( FOdysseyVectorScene* iScene
        //                            , const FKey& iKey ) override;
        //virtual uint64 OnKeyUpVector( FOdysseyVectorScene* iScene, const FKey& iKey ) override;
        virtual uint64 OnMouseDownVector( FOdysseyVectorScene* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey ) override;
        //virtual uint64 OnMouseHoverVector( FOdysseyVectorScene* iScene
        //                                , const FOdysseyPoint& iPointInTexture ) override;
        virtual uint64 OnMouseDragVector( FOdysseyVectorScene* iScene
                                        , const FOdysseyPoint& iPointInTexture ) override;
        virtual uint64 OnMouseUpVector( FOdysseyVectorScene* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey ) override;
        //virtual uint64 PropertyChangedVector( FOdysseyVectorScene* iScene
        //                                  , const FName& iPropertyName ) override;

        void Copy( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
        void Paste( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );

protected:
    void OnMouseUpVectorObjectMode( FOdysseyVectorScene* iScene
                                  , const FOdysseyPoint& iPointInTexture
                                  , const FKey& iKey );
    void OnMouseUpVectorVertexMode( FOdysseyVectorScene* iScene
                                  , const FOdysseyPoint& iPointInTexture
                                  , const FKey& iKey );
    ::ULIS::FRectD GenerateMask();

    void SelectBucketFromPaintGroup( FOdysseyVectorGroupPaint* iPaintGroup );
    void SelectVertexFromPaintGroup( FOdysseyVectorGroupPaint* iPaintGroup );
    void SelectVertexFromPath( FOdysseyVectorPath* iPath );

protected:
    FOdysseyPainterEditorVectorSelectionToolHUD* mPickHUD;
    std::vector<::ULIS::FVec2D> mPointArray;
    ::ULIS::FVec2D mPressedMouseCoords;

public:
    UPROPERTY( EditAnywhere, Category = SelectionTool )
    EOdysseyVectorSelectionShape SelectionShape;

    //UPROPERTY( EditAnywhere, Category = BehaviorInVertexMode )
    //bool RestrictToSelectedObjects;
};

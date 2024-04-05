// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionTool.h"
#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndoPointPosition.h"

#include "OdysseyPainterEditorVectorGridTool.generated.h"

class FGridNode;
class FOdysseyPainterEditorVectorGridToolHUD;

UCLASS( HideCategories = (SelectionTool) )
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorGridTool : public UOdysseyPainterEditorVectorSelectionTool
{
    public:
        GENERATED_BODY()

    public:
        // Destructor
        virtual ~UOdysseyPainterEditorVectorGridTool();

        virtual bool IsActivable() const override;

        UOdysseyPainterEditorVectorGridTool();

        virtual TSharedRef<SWidget> CreateTopTabWidget() override;

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        //virtual bool OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
        //                            , const FKey& iKey ) override;
        //virtual bool OnKeyUpVector( FOdysseyVectorGroupPaint* iScene, const FKey& iKey ) override;
        virtual uint64 OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey ) override;
        virtual uint64 OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                         , const FOdysseyPoint& iPointInTexture ) override;
        virtual uint64 OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture ) override;
        virtual uint64 OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey ) override;
        virtual uint64 PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                            , const FName& iPropertyName ) override;

    private:
        std::vector<FOdysseyVectorPoint*> mPointArray;
        FOdysseyPainterEditorVectorGridToolHUD* mGridHUD;
        std::vector<FGridNode *> mGridNodeArray;
        bool mMultipleSelectionMode;

    public:
        UPROPERTY( EditAnywhere
                 , Category = GridTool
                 , meta = ( ToolTip  = "Divisions X"
                          , ClampMin = "1"
                          , ClampMax = "32"
                          , UIMin    = "1"
                          , UIMax    = "32" ) )
        uint32 DivisionsX;

        UPROPERTY( EditAnywhere
                 , Category = GridTool
                 , meta = ( ToolTip  = "Divisions Y"
                          , ClampMin = "1"
                          , ClampMax = "32"
                          , UIMin    = "1"
                          , UIMax    = "32" ) )
        uint32 DivisionsY;

        UPROPERTY( EditAnywhere
                 , Category = GridTool
                 , meta = ( ToolTip  = "Picking Radius"
                          , ClampMin = "0.0"
                          , UIMin    = "0.0" ) )
        double PickingRadius;

        UPROPERTY( EditAnywhere
                 , Category = GridTool
                 , meta = ( ToolTip  = "World" ) )
        bool World;
};

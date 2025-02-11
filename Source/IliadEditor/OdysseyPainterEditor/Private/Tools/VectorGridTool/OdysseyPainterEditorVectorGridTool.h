// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionTool.h"

#include "OdysseyPainterEditorVectorGridTool.generated.h"

class FGridNode;
class FOdysseyPainterEditorVectorGridToolHUD;
class FOdysseyVectorPoint;

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

        virtual FText GetTooltip() const override;

        virtual void ExtendToolbar( FToolBarBuilder& iBuilder ) override;

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
        virtual void OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                         , const FOdysseyPoint& iPointInTexture, uint64& oSignalFlags ) override;
        virtual void OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture, uint64& oSignalFlags ) override;
        virtual bool OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey, uint64& oSignalFlags ) override;
        virtual uint64 PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                            , const FName& iPropertyName ) override;

    private:
        std::vector<FOdysseyVectorPoint*> mPointArray;
        FOdysseyPainterEditorVectorGridToolHUD* mGridHUD;
        std::vector<FGridNode *> mGridNodeArray;
        bool mMultipleSelectionMode;

    public:
        UPROPERTY( EditAnywhere
                 , Category=GridTool
                 , meta = ( ToolTip  = "Divisions X"
                          , ClampMin = "1"
                          , ClampMax = "32"
                          , UIMin    = "1"
                          , UIMax    = "32" ) )
        uint32 DivisionsX;

        UPROPERTY( EditAnywhere
                 , Category=GridTool
                 , meta = ( ToolTip  = "Divisions Y"
                          , ClampMin = "1"
                          , ClampMax = "32"
                          , UIMin    = "1"
                          , UIMax    = "32" ) )
        uint32 DivisionsY;

        UPROPERTY( EditAnywhere
                 , Category=GridTool
                 , meta = ( ToolTip  = "Picking Radius"
                          , ClampMin = "0"
                          , UIMin    = "0" ) )
        uint32 PickingRadius;

        UPROPERTY( EditAnywhere
                 , Category=GridTool
                 , meta = ( ToolTip  = "World" ) )
        bool World;
};

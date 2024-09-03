// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"

#include "OdysseyPainterEditorVectorMatchingTool.generated.h"

class  FGridNode;
class  FOdysseyPainterEditorVectorMatchingToolHUD;
class FOdysseyVectorTagInbetweener;
struct FInbetweenerInbetween;
class  FInbetweenerPoint;
class  FInbetweenerGrid;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorMatchingTool : public UOdysseyPainterEditorVectorBaseTool
{
    public:
        GENERATED_BODY()

    public:
        // Destructor
        virtual ~UOdysseyPainterEditorVectorMatchingTool();

        virtual bool IsActivable() const override;

        UOdysseyPainterEditorVectorMatchingTool();

        virtual TSharedRef<SWidget> CreateTopTabWidget() override;

        virtual FText GetTooltip() const override;

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
        FOdysseyPainterEditorVectorMatchingToolHUD* mMatchingHUD;
        std::vector<FInbetweenerPoint*> mPickedPointArray;
        std::vector<FInbetweenerGrid*> mPickedGridArray;
        FOdysseyVectorTagInbetweener* mPickedInbetweenerTag;

    public:
        UPROPERTY( EditAnywhere
                 , Category = MatchingTool
                 , meta = ( ToolTip  = "Picking Radius"
                          , ClampMin = "0.0"
                          , UIMin    = "0.0" ) )
        double PickingRadius;
};

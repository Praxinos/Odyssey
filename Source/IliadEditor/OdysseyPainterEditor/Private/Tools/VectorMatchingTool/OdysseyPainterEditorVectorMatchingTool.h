// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"
#include <vector> //...

#include "OdysseyPainterEditorVectorMatchingTool.generated.h"

class  FGridNode;
class  FOdysseyPainterEditorVectorMatchingToolHUD;
class FOdysseyVectorTagInbetweener;
struct FInbetweenerInbetween;
class  FInbetweenerPoint;
class  FInbetweenerGrid;

UENUM()
enum class eMatchingInfluence : uint8
{
    Uniform = 0,
    Radial  = 1
};

UENUM()
enum class eMatchingGridDisplayMode : uint8
{
    AsQuads  = 0 UMETA( ToolTip = "Show grid as quads" ),
    AsPoints = 1 UMETA( ToolTip = "Show grid as points" )
};

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

        virtual FText GetTooltip() const override;

        virtual void ExtendToolbar( FToolBarBuilder& iBuilder ) override;

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        //virtual bool OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
        //                            , const FKey& iKey
                                            //, uint64& oSignalFlags ) override;
        //virtual bool OnKeyUpVector( FOdysseyVectorGroupPaint* iScene, const FKey& iKey
                                            //, uint64& oSignalFlags ) override;
        virtual bool OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey
                                        , uint64& oSignalFlags ) override;
        virtual void OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , uint64& oSignalFlags ) override;
        virtual void OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , uint64& oSignalFlags ) override;
        virtual bool OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey
                                        , uint64& oSignalFlags ) override;
        virtual uint64 PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                            , const FName& iPropertyName ) override;

    private:
        FOdysseyPainterEditorVectorMatchingToolHUD* mMatchingHUD;
        std::vector<FInbetweenerPoint*> mPickedPointArray;
        std::vector<float> mWorldDistanceArray;
        std::vector<FInbetweenerGrid*> mPickedGridArray;

    public:
        UPROPERTY( EditAnywhere
                 , Category = MatchingTool
                 , meta = ( ToolTip  = "Picking Radius"
                          , ClampMin = "0"
                          , UIMin    = "0" ) )
        uint32 PickingRadius;

        UPROPERTY( EditAnywhere
                 , Category = MatchingTool
                 , meta = ( DisplayName = "Picking Influence"
                          , ToolTip  = "Picking Influence" ) )
        eMatchingInfluence MatchingInfluence;

        UPROPERTY( EditAnywhere
                 , Category = MatchingTool
                 , meta = ( DisplayName = "Grid Rigidity"
                          , ToolTip  = "Grid Rigidity"
                          , ClampMin = "0"
                          , UIMin    = "0"
                          , ClampMax = "20"
                          , UIMax    = "20" ) )
        uint32 Rigidity;

        UPROPERTY( EditAnywhere
                 , Category = MatchingTool
                 , meta = ( DisplayName = "Move picked area only"
                          , ToolTip  = "Move picked area only" ) )
        bool RigidifySelectionOnly;

        UPROPERTY( EditAnywhere
                 , Category = MatchingTool
                 , meta = ( DisplayName = "Show Inbetweens"
                          , ToolTip  = "Show Inbetweens" ) )
        bool ShowInbetweens;

        UPROPERTY( EditAnywhere
                 , Category = MatchingTool
                 , meta = ( DisplayName = "Grid Display Mode"
                          , ToolTip  = "Grid Display Mode" ) )
        eMatchingGridDisplayMode GridDisplayMode;
};

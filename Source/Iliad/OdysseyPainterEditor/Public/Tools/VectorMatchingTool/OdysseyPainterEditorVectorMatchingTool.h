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

UENUM()
enum class eMatchingInfluence : uint8
{
    Uniform = 0,
    Radial  = 1
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

        virtual TSharedRef<SWidget> CreateTopTabWidget() override;

        virtual FText GetTooltip() const override;

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
                 , meta = ( ToolTip  = "Matching Influence" ) )
        eMatchingInfluence MatchingInfluence;

        UPROPERTY( EditAnywhere
                 , Category = MatchingTool
                 , meta = ( ToolTip  = "Rigidity"
                          , ClampMin = "0"
                          , UIMin    = "0"
                          , ClampMax = "20"
                          , UIMax    = "20" ) )
        uint32 Rigidity;

        UPROPERTY( EditAnywhere
                 , Category = MatchingTool
                 , meta = ( ToolTip  = "Rigidify Selection Only" ) )
        bool RigidifySelectionOnly;

        UPROPERTY( EditAnywhere
                 , Category = MatchingTool
                 , meta = ( ToolTip  = "Show Inbetweens" ) )
        bool ShowInbetweens;
};

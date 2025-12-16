// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionTool.h"

#include "OdysseyPainterEditorVectorGridTool.generated.h"

class FGridNode;
class FOdysseyPainterEditorVectorGridToolHUD;
class FOdysseyVectorPoint;

UENUM()
enum class eVectorGridEditionMode : uint8
{
    Single = 0,
    Multi = 1,
};

UCLASS( HideCategories = (SelectionTool) )
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorGridTool : public UOdysseyPainterEditorVectorBaseTool
{
    public:
        GENERATED_BODY()

    public:
        // Destructor
        virtual ~UOdysseyPainterEditorVectorGridTool();

        virtual bool IsActivable() const override;

        UOdysseyPainterEditorVectorGridTool();

        virtual FText GetTooltip() const override;

        virtual void ExtendToolbar( UToolMenu* iToolMenu ) override;
        eVectorGridEditionMode GetEditionMode();

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        bool OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                , const FKeyEvent& InKeyEvent );
        bool OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                  , const FKeyEvent& InKeyEvent );

        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        //virtual bool OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
        //                            , const FKey& iKey ) override;
        //virtual bool OnKeyUpVector( FOdysseyVectorGroupPaint* iScene, const FKey& iKey ) override;
        virtual bool OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey ) override;
        virtual void OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                       , const FOdysseyPoint& iPointInTexture ) override;
        virtual void OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                      , const FOdysseyPoint& iPointInTexture ) override;
        virtual bool OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                    , const FOdysseyPoint& iPointInTexture
                                    , const FKey& iKey ) override;
        virtual bool OnMouseClickVector( FOdysseyVectorGroupPaint* iScene
                                       , const FOdysseyPoint& iPointInTexture
                                       , const FKey& iKey ) override;
        virtual void PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                          , const FName& iPropertyName ) override;
        TSharedRef<SWidget> CreateModifierSegmentControl();
        const FSlateBrush* GetBackgroundColor( eVectorGridEditionMode iMode ) const;
        void SetEditionMode( eVectorGridEditionMode iMode );
        bool NodesAlreadySelected( std::vector<FGridNode *>& iNodeArray );
        virtual void OnVectorLayerUpdate( const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags
                                        , uint32 iUpdateFlags ) override;

    private:
        std::vector<FOdysseyVectorPoint*> mPointArray;
        FOdysseyPainterEditorVectorGridToolHUD* mGridHUD;
        eVectorGridEditionMode mEditionMode;

    public:
        UPROPERTY( EditAnywhere
                 , Category=GridTool
                 , meta = ( ToolTip  = "Divisions X"
                          , ClampMin = "1"
                          , ClampMax = "32"
                          , UIMin    = "1"
                          , UIMax    = "32"
                          , ToolConfiguration ) )
        uint32 DivisionsX;

        UPROPERTY( EditAnywhere
                 , Category=GridTool
                 , meta = ( ToolTip  = "Divisions Y"
                          , ClampMin = "1"
                          , ClampMax = "32"
                          , UIMin    = "1"
                          , UIMax    = "32"
                          , ToolConfiguration ) )
        uint32 DivisionsY;

        UPROPERTY( EditAnywhere
                 , Category=GridTool
                 , meta = ( ToolTip  = "Picking Radius"
                          , ClampMin = "0"
                          , UIMin    = "0"
                          , ToolConfiguration ) )
        uint32 PickingRadius;

        UPROPERTY( EditAnywhere
                 , Category=GridTool
                 , meta = ( ToolTip  = "World"
                 , ToolConfiguration ) )
        bool World;
};

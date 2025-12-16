// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"
#include <ULIS>

#include "OdysseyPainterEditorVectorEraserTool.generated.h"

class FOdysseyPainterEditorVectorEraserToolHUD;
class FOdysseyVectorObject;
class FOdysseyVectorSegment;
class FOdysseyVectorVertex;

UENUM()
enum class eVectorEraserEditionMode : uint8
{
    Default = 0,
    Section = 1,
    Path    = 2,
};

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorEraserTool : public UOdysseyPainterEditorVectorBaseTool
{
    public:
        GENERATED_BODY()

    public:
        // Destructor
        virtual ~UOdysseyPainterEditorVectorEraserTool();

        //Constructor
        UOdysseyPainterEditorVectorEraserTool();

        virtual bool IsActivable() const override;

        virtual FText GetTooltip() const override;

        virtual void ExtendToolbar( UToolMenu* iToolMenu ) override;

    protected:
        //OdysseyPainterVectorBaseEditorTool overrides
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ) override;
        virtual bool OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                          , const FKeyEvent& InKeyEvent ) override;
        virtual bool OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                        , const FKeyEvent& InKeyEvent ) override;
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
        //virtual void PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
        //                                 , const FName& iPropertyName ) override;


        void ErasePaths( FOdysseyVectorGroupPaint* iScene
                       , const ::ULIS::FRectD& iErasureArea
                       , std::vector<FOdysseyVectorObject*>& oAddedObjectArray
                       , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                       , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                       , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                       , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                       , std::vector<FOdysseyVectorObject*>& oRemovedObjectArray );
        void EraseSections( FOdysseyVectorGroupPaint* iScene
                          , const ::ULIS::FRectD& iErasureArea
                          , std::vector<FOdysseyVectorObject*>& oAddedObjectArray
                          , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                          , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                          , std::vector<FOdysseyVectorObject*>& oRemovedObjectArray
                          , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                          , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray );
        TSharedRef<SWidget> CreateModifierSegmentControl();
        const FSlateBrush* GetBackgroundColor( eVectorEraserEditionMode iMode ) const;
        void SetEditionMode( eVectorEraserEditionMode iMode );

    private:
        FOdysseyPainterEditorVectorEraserToolHUD* mEraserHUD;
        eVectorEraserEditionMode mEditionMode;
        ::ULIS::FVec2D mMin;
        ::ULIS::FVec2D mMax;

    public:
        UPROPERTY( EditAnywhere
                 , Category=EraserTool
                 , meta = ( Tooltip = "Split Path",
                            ToolConfiguration ) )
        bool SplitPath;

        UPROPERTY( EditAnywhere
                 , Category=EraserTool
                 , meta = ( Tooltip  = "Radius"
                          , ClampMin = "0"
                          , UIMin    = "0",
                            ToolConfiguration ) )
        uint32 Radius;
};

// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include <vector> //...
#include "OdysseyPainterEditorVectorBaseTool.generated.h"


class FOdysseyPainterEditorVectorBaseToolHUD;
class ISinglePropertyView;
class SOdysseyViewport;
class SViewport;

class FOdysseyVectorGroupPaint;
class FOdysseyVectorSegment;
class FOdysseyVectorPath;
class FOdysseyVectorHandleSegment;
class FOdysseyVectorVertex;
class FOdysseyVectorHandleSegment;
class FOdysseyVectorCell;
class SOdysseyPainterEditorVectorSceneDetailsView;
struct FOdysseyVectorObjectInvalidationFlags;

enum class eMouseEventName : uint8
{
    MouseHover = 0,
    MouseDown  = 1,
    MouseDrag  = 2,
    MouseUp    = 3
};

UENUM()
enum class eShowInbetweens : uint8
{
    None                    = 0 UMETA( ToolTip = "" ),
    Surrounding             = 1 UMETA( ToolTip = "" ),
    SourceAndBreakdownsOnly = 2 UMETA( ToolTip = "Source and breakdowns only" ),
    SourceOnly              = 3 UMETA( ToolTip = "Source only" ),
    All                     = 4 UMETA( ToolTip = "" )
};

UCLASS(Abstract, HideDropdown)
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorBaseTool : public UOdysseyPainterEditorTool
{
    public:
        GENERATED_BODY()

    protected:
        // reserved flags returned by callbaks, the eight higher bits
        //static const uint64 NOMENU = ( FOdysseyVectorEngine::SIGNAL_USER0_RESERVED );

    public:
        // Destructor
        virtual ~UOdysseyPainterEditorVectorBaseTool();

        //Constructor
        UOdysseyPainterEditorVectorBaseTool();
        UOdysseyPainterEditorVectorBaseTool( TSharedPtr<FOdysseyPainterEditorVectorBaseToolHUD> iBaseHUD
                                           , bool iAutoCreateMedia
                                           , bool iMouseEventViaHUD );

        static bool DoubleClicked();

        TSharedPtr<SWidget> CreatePropertyWidget( TSharedPtr<class IPropertyHandle> iPropertyHandle
                                                , const TSharedPtr<ISinglePropertyView> iView );

        //virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
        //virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;

        virtual void Load();
        virtual void Unload();
        virtual bool OnKeyDown( const FKey& iKey ) override;
        virtual bool OnKeyUp( const FKey& iKey ) override;
        virtual bool OnKeyDownGlobal( const FKeyEvent& InKeyEvent ) override;
        virtual bool OnKeyUpGlobal( const FKeyEvent& InKeyEvent ) override;

        virtual void OnMouseHover( const FOdysseyPoint& iPointInTexture ) override;
        virtual bool OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
        virtual void OnMouseDrag( const FOdysseyPoint& iPointInTexture ) override;
        virtual bool OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
        virtual bool OnMouseClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;

        virtual void OnMouseHoverViaHUD( const FOdysseyPoint& iPointInTexture );
        virtual bool OnMouseDownViaHUD( const FOdysseyPoint& iPointInTexture, const FKey& iKey );
        virtual void OnMouseDragViaHUD( const FOdysseyPoint& iPointInTexture );
        virtual bool OnMouseUpViaHUD( const FOdysseyPoint& iPointInTexture, const FKey& iKey );
        virtual bool OnMouseClickViaHUD(const FOdysseyPoint& iPointInTexture, const FKey& iKey );

        virtual void Commit();
        virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
        virtual void ExtendMenu( TSharedRef<FExtender> iExtender ) override;
        virtual void ExtendContextMenu( FMenuBuilder& menu );
        virtual void BindShortcuts( TSharedPtr<FUICommandList> iCommandList );
        virtual void ExtendToolbar( UToolMenu* iToolMenu ) override;

    protected:
        virtual TOptional<FMouseCursor> GetMouseCursorOverride() const override;

    public:
        virtual bool SupportsColorType(EOdysseyPainterEditorColorType iType) override;

        bool IsDragging();

        TSharedPtr<SOdysseyViewport> GetViewport();
        FOdysseyVectorCell* GetWorkingCell();
        uint32 GetViewportWidth();
        uint32 GetViewportHeight();
        FOdysseyVectorGroup* GetWorkingGroup() const;
        void UnbindLayerDelegates();
        void BindLayerDelegates();

    protected:
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ){ return 0; };
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ){ return 0; };
        virtual bool OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                          , const FKeyEvent& InKeyEvent );
        virtual bool OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                        , const FKeyEvent& InKeyEvent );
        virtual bool OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
                                        , const FKey& iKey );
        virtual bool OnKeyUpVector( FOdysseyVectorGroupPaint* iScene
                                      , const FKey& iKey );
        virtual bool OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey ){ return false; };
        virtual void OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture ){};
        virtual void OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture ){};
        virtual bool OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey ){ return false; };

        virtual bool OnMouseClickVector(FOdysseyVectorGroupPaint* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey );

        virtual void PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                          , const FName& iPropertyName );
        bool FilterMouseEvent( eMouseEventName iCurrentMouseEvent );

        void PopupContextMenu();
        TSharedPtr<SWidget> CreateContextMenu();
        void OnUndoRedo();
        virtual void OnVectorLayerUpdate( const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags
                                        , uint32 iUpdateFlags );



    private:
        void Copy();
        void Paste();
        void SelectAll();
        void IncreaseContourWidth();
        void DecreaseContourWidth();
        void Cut();

        void ActionDelete();
        void ActionResetView();
        void ActionMakePaintGroup();
        void ActionGroup();
        void ActionUngroup();
        void ActionBringForward();
        void ActionSendBackward();
        void ActionFlipHorizontal();
        void ActionFlipVertical();
        void ActionClearColoring();
        void ActionApplyTransformations();
        void ActionSubdivideSegments();
        void ActionAlignPointSelection();
        void ActionUnalignPointSelection();
        void ActionLockPointSelection();
        void ActionUnlockPointSelection();

    protected:
        // returns the closest segment among those picked.
        FOdysseyVectorSegment* PickSegments( FOdysseyVectorGroupPaint* iScene
                                           , double iWorldX
                                           , double iWorldY
                                           , double iWorldRadius
                                           , bool iRestrictToSelection
                                           , bool iStopImmediately
                                           , std::vector<FOdysseyVectorSegment*>& oPickedSegmentArray );

        void GetSelectedVertices( FOdysseyVectorGroupPaint* iScene
                                , std::vector<FOdysseyVectorVertex*>& oSelectedVertexArray );
        // static
        void GetSegmentHandlesFromVertices( const std::vector<FOdysseyVectorVertex*>& iVertexArray
                                          , std::vector<FOdysseyVectorHandleSegment*>& oSegmentHandleArray );
        void SetPathColor( FOdysseyVectorPath* iPath );
        void MakeTest( FOdysseyVectorGroupPaint* iScene );

        // for testing purpose
        void MakeDemoBrush( FOdysseyVectorGroupPaint* iScene );

        void SetVectorEditionFlags( uint64 iViewMode );

    protected:
        virtual void ExtendContextMenuObject( FOdysseyVectorGroupPaint* iScene
                                            , FMenuBuilder& menu
                                            , uint64 iObjectMenuFlags );
        virtual void ExtendContextMenuVertex( FOdysseyVectorGroupPaint* iScene
                                            , FMenuBuilder& menu
                                            , uint64 iVertexMenuFlags );
        virtual void ExtendContextMenuInbetween( FOdysseyVectorGroupPaint* iScene
                                               , FMenuBuilder& menu
                                               , uint64 iInbetweenMenuFlags );
        void ResetGridMenu( FMenuBuilder& menu, FOdysseyVectorGroupPaint* vectorScene );
        void ResetSpacingMenu( FMenuBuilder& menu, FOdysseyVectorGroupPaint* iScene );
        void CopySpacingMenu( FMenuBuilder& menu, FOdysseyVectorGroupPaint* iScene );
        void PasteSpacingMenu( FMenuBuilder& menu, FOdysseyVectorGroupPaint* iScene );
        static bool CanAddTag( FOdysseyVectorGroupPaint* iScene );
        static bool CanAlterTag( FOdysseyVectorGroupPaint* iScene );
        bool CanBePainted();
        bool CanUngroup();

    protected:
        static const uint64 OBJECTMENU_HASSUBDIVIDE = ( 1ULL << 0 );

    protected:
        TSharedPtr<FOdysseyPainterEditorVectorBaseToolHUD> mBaseHUD;
        bool mHasContextMenu;
        bool mDragging;
        bool mAutoCreateMedia;
        bool bMouseEventViaHUD;
        // to prevent a double mouse down bug detected in
        // FOdysseyPainterEditorViewportClient::InputKey
        // FOdysseyPainterEditorViewportClient::OnStylusStateChanged
        // they sometimes are both called and both trigger
        // FOdysseyPainterEditorViewportClient::InputKeyWithStrokePoint
        eMouseEventName mPreviousMouseEvent; // filter faulty stylus events
        // we need the focus on the viewport for keyboard
        // and some tools need to know the viewport size
        TWeakPtr<SOdysseyViewport> mViewport;
        // Note: we use FOdysseyVectorCell and not FOdysseyVectorGroupPaint (the scene) because the cell
        // never changes, unlike the scene that is attached to it that can change for example when a cell
        // is cleaned (a new scene is then assigned to this cell) and there is a risk to work with an
        // orphaned scene.
        FOdysseyVectorCell* mWorkingCell;
        // We also have to store the layer, as FOdysseyVectorCell::GetLayer() is going to return null because
        // UOdysseyPainterEditorVectorBaseTool::Unload() is not called in the same context as Load()
        FOdysseyVectorLayer* mWorkingLayer;

        TSharedPtr<::ULIS::FBlock> mVectorBlock;

    public:
        //UPROPERTY( EditAnywhere, Category=Behavior )
        //bool RestrictToSelectedObjects;
};

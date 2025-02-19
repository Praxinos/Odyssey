// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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

enum class eMouseEventName : uint8
{
    MouseHover = 0,
    MouseDown  = 1,
    MouseDrag  = 2,
    MouseUp    = 3
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
                                           , bool iAutoCreateFrame );

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

        virtual void OnMouseHoverViaHUD( const FOdysseyPoint& iPointInTexture );

        virtual bool OnMouseDownViaHUD( const FOdysseyPoint& iPointInTexture, const FKey& iKey );
        virtual void OnMouseDragViaHUD( const FOdysseyPoint& iPointInTexture );
        virtual bool OnMouseUpViaHUD( const FOdysseyPoint& iPointInTexture, const FKey& iKey );

        virtual bool OnMouseClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
        virtual void Commit();
        virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
        virtual void ExtendMenu( TSharedRef<FExtender> iExtender ) override;
        virtual void ExtendContextMenu( FMenuBuilder& menu );
        virtual void BindShortcuts( FBaseToolkit* iToolkit );
        virtual EMouseCursor::Type GetMouseCursor() const override;
        virtual void ExtendToolbar( FToolBarBuilder& iBuilder ) override;

        virtual bool SupportsColorType(EOdysseyPainterEditorColorType iType) override;

        bool IsDragging();

        TSharedPtr<SOdysseyViewport> GetViewport();

    protected:
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ){ return 0; };
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ){ return 0; };
        virtual bool OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                          , const FKeyEvent& InKeyEvent
                                          , uint64& oSignalFlags );
        virtual bool OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                        , const FKeyEvent& InKeyEvent
                                        , uint64& oSignalFlags );
        virtual bool OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
                                        , const FKey& iKey
                                        , uint64& oSignalFlags );
        virtual bool OnKeyUpVector( FOdysseyVectorGroupPaint* iScene
                                      , const FKey& iKey
                                      , uint64& oSignalFlags );
        virtual bool OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey
                                        , uint64& oSignalFlags ){ return false; };
        virtual void OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , uint64& oSignalFlags ){};
        virtual void OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , uint64& oSignalFlags ){};
        virtual bool OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey
                                      , uint64& oSignalFlags ){ return false; };

        virtual bool OnMouseClickVector(FOdysseyVectorGroupPaint* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey
                                      , uint64& oSignalFlags );

        virtual uint64 PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                            , const FName& iPropertyName );
        bool FilterMouseEvent( eMouseEventName iCurrentMouseEvent );

        void PopupContextMenu();
        TSharedPtr<SWidget> CreateContextMenu();


    private:
        void Copy();
        void Paste();
        void SelectAll();
        void Delete();
        void IncreaseContourWidth();
        void DecreaseContourWidth();
        void Cut();

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
        bool CanAddTag( FOdysseyVectorGroupPaint* iScene );
        bool CanAlterTag( FOdysseyVectorGroupPaint* iScene );

    protected:
        static const uint64 OBJECTMENU_HASSUBDIVIDE = ( 1ULL << 0 );

    protected:
        TSharedPtr<FOdysseyPainterEditorVectorBaseToolHUD> mBaseHUD;
        bool mHasContextMenu;
        bool mDragging;
        bool mAutoCreateMedia;
        // to prevent a double mouse down bug detected in
        // FOdysseyPainterEditorViewportClient::InputKey
        // FOdysseyPainterEditorViewportClient::OnStylusStateChanged
        // they sometimes are both called and both trigger
        // FOdysseyPainterEditorViewportClient::InputKeyWithStrokePoint
        eMouseEventName mPreviousMouseEvent; // filter faulty stylus events
        // we need the focus on the viewport for keyboard
        // and some tools need to know the viewport size
        TWeakPtr<SOdysseyViewport> mViewport;

    public:
        //UPROPERTY( EditAnywhere, Category=Behavior )
        //bool RestrictToSelectedObjects;
};

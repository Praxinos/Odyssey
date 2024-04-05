// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyVector.h"
#include "Widgets/Tools/SOdysseyPainterEditorVectorEditionMode.h"
#include "OdysseyPainterEditorVectorBaseTool.generated.h"

class FOdysseyPainterEditorVectorBaseToolHUD;

enum class eMouseEventName : uint8
{
    MouseHover = 0,
    MouseDown  = 1,
    MouseDrag  = 2,
    MouseUp    = 3
};

UCLASS(Abstract)
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
        UOdysseyPainterEditorVectorBaseTool( FOdysseyPainterEditorVectorBaseToolHUD* iBaseHUD
                                           , bool iAutoCreateFrame );

        static bool DoubleClicked();

        virtual TSharedRef<SWidget> CreateTopTabWidget() override;
        TSharedPtr<SWidget> CreatePropertyWidget( TSharedPtr<class IPropertyHandle> iPropertyHandle
                                                , const TSharedPtr<ISinglePropertyView> iView );

	    //virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
	    //virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;

        virtual void Load();
        virtual void Unload();
        virtual bool OnKeyDown( const FKey& iKey ) override;
        virtual bool OnKeyUp( const FKey& iKey ) override;
        virtual bool OnKeyDownGlobal( const FKey& iKey ) override;
        virtual bool OnKeyUpGlobal( const FKey& iKey ) override;
        virtual bool OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey );
        virtual void OnMouseHover( const FOdysseyPoint& iPointInTexture );
        virtual void OnMouseDrag( const FOdysseyPoint& iPointInTexture );
        virtual bool OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey );
        virtual void Commit();
        virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
        virtual void ExtendContextMenu( FMenuBuilder& menu );
        virtual void BindShortcuts( FBaseToolkit* iToolkit );
        virtual EMouseCursor::Type GetMouseCursor() const override;

        bool IsDragging();

    protected:
        virtual uint64 LoadVector( FOdysseyVectorGroupPaint* iScene ){ return 0; };
        virtual uint64 UnloadVector( FOdysseyVectorGroupPaint* iScene ){ return 0; };
        virtual uint64 OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                            , const FKey& iKey );
        virtual uint64 OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                          , const FKey& iKey );
        virtual uint64 OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
                                        , const FKey& iKey );
        virtual uint64 OnKeyUpVector( FOdysseyVectorGroupPaint* iScene
                                      , const FKey& iKey );
        virtual uint64 OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey ){ return false; };
        virtual uint64 OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture ){ return 0; };
        virtual uint64 OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                        , const FOdysseyPoint& iPointInTexture ){ return 0; };
        virtual uint64 OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey ){ return false; };
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
        void GetSelectedVertices( FOdysseyVectorGroupPaint* iScene
                                , std::vector<FOdysseyVectorVertex*>& oSelectedVertexArray );
        // static
        void GetSegmentHandlesFromVertices( const std::vector<FOdysseyVectorVertex*>& iVertexArray
                                          , std::vector<FOdysseyVectorHandleSegment*>& oSegmentHandleArray );
        void SetPathColor( FOdysseyVectorPath* iPath
                         , eForegroundColorMode iColorSource );
        void MakeTest( FOdysseyVectorGroupPaint* iScene );

        // for testing purpose
        void MakeDemoBrush( FOdysseyVectorGroupPaint* iScene );

    private:
        void ExtendContextMenuObject( FMenuBuilder& menu );
        void ExtendContextMenuVertex( FMenuBuilder& menu );


    protected:
        // to store the top tab widget in order to create it only once. this will prevent sizing 
        // issues in the top bar.
        // to force keyboard focus on mouse hover.
        // Prevents the user from having to click at least once in the viewport.
        TSharedPtr< SViewport > mViewportWidget;
        FOdysseyPainterEditorVectorBaseToolHUD* mBaseHUD;
        bool mHasContextMenu;
        bool mDragging;
        bool mAutoCreateMedia;
        // to prevent a double mouse down bug detected in
        // FOdysseyPainterEditorViewportClient::InputKey
        // FOdysseyPainterEditorViewportClient::OnStylusStateChanged
        // they sometimes are both called and both trigger 
        // FOdysseyPainterEditorViewportClient::InputKeyWithStrokePoint
        eMouseEventName mPreviousMouseEvent; // filter faulty stylus events

    public:
        //UPROPERTY( EditAnywhere, Category = Behavior )
        //bool RestrictToSelectedObjects;
};

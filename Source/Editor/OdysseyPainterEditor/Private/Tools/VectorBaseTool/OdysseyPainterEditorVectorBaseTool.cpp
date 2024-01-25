// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
//#include "Widgets/Tools/SOdysseyPainterEditorVectorEditionMode.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Framework/Commands/GenericCommands.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "Palette/OdysseyPaletteEntryColor.h"
#include "Palette/OdysseyPalette.h"
#include "PainterEditor/OdysseyPainterEditorColorPaletteTab.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorBaseTool::~UOdysseyPainterEditorVectorBaseTool()
{
    if( mBaseHUD )
    {
        delete mBaseHUD;
    }
}

UOdysseyPainterEditorVectorBaseTool::UOdysseyPainterEditorVectorBaseTool()
    : mBaseHUD( nullptr )
    , mHasContextMenu( true )
{
}

UOdysseyPainterEditorVectorBaseTool::UOdysseyPainterEditorVectorBaseTool( FOdysseyPainterEditorVectorBaseToolHUD* iBaseHUD, bool iAutoCreateMedia )
    : mBaseHUD( iBaseHUD )
    , mHasContextMenu(true)
    , mAutoCreateMedia( iAutoCreateMedia )
{
}

//static
bool
UOdysseyPainterEditorVectorBaseTool::DoubleClicked()
{
    uint64 clickTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    static uint64 previousClickTime = 0;
    bool doubleClicked = ( ( clickTime - previousClickTime ) < 200 ) ? true : false;

    previousClickTime = clickTime;

    return doubleClicked;
}

void
UOdysseyPainterEditorVectorBaseTool::SetPathColor( FOdysseyVectorPath* iPath
                                                 , eBaseToolColorSource iColorSource )
{
    switch( iColorSource )
    {
        case eBaseToolColorSource::ColorWheel:
        {
            ::ULIS::FColor color = GetEditor()->PaintColor().GetValue();
            ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );
            FColor ueColor = FColor( rgba8.R8(), rgba8.G8(), rgba8.B8(), rgba8.A8() );

            iPath->GetForegroundBucket().SetSolidColor( ueColor );
        }
        break;

        case eBaseToolColorSource::Palette:
        {
            TSharedPtr<FOdysseyPainterEditorPaletteTab> colorPaletteTab = GetEditor()->FindTab<FOdysseyPainterEditorPaletteTab>();
            UOdysseyPalette* palette = colorPaletteTab->PaletteWidget()->GetColorPalette()->GetPalette();

            if( palette )
            {
                UOdysseyPaletteEntry * paletteEntry = palette->CurrentEntry.Get();

                if( paletteEntry && paletteEntry->IsA( UOdysseyPaletteEntryColor::StaticClass() ) )
                {
                    iPath->GetForegroundBucket().SetPaletteEntry( paletteEntry );
                }
            }
        }
        break;

        default:
        break;
    }

    iPath->GetForegroundBucket().SetColorMode( (eBucketColorMode) iColorSource );
}

void
UOdysseyPainterEditorVectorBaseTool::GetSelectedVertices( FOdysseyVectorGroupPaint* iScene
                                                        , std::vector<FOdysseyVectorVertex*>& oSelectedVertexArray )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    oSelectedVertexArray.clear();

    vectorEngine->Traverse
    ( iScene
    , iScene
    , 0
    , [ this
      , iScene
      , vectorEngine
      , &oSelectedVertexArray ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
      {
          if( vectorEngine->ObjectHasFocus( iScene, object, traversalFlags ) )
          {
              if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
              {
                  FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);
 
                  path->GetSelectedVertices( oSelectedVertexArray );
              }

              return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
          }

          return 0;
      } );
}

bool
UOdysseyPainterEditorVectorBaseTool::IsDragging()
{
    return mDragging;
}

void
UOdysseyPainterEditorVectorBaseTool::GetSegmentHandlesFromVertices( const std::vector<FOdysseyVectorVertex*>& iVertexArray
                                                                  , std::vector<FOdysseyVectorHandleSegment*>& oSegmentHandleArray )
{
    for( FOdysseyVectorVertex* vertex : iVertexArray )
    {
        for( FOdysseyVectorSegment* segment : vertex->GetSegmentList() )
        {
            if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
            {
                FOdysseyVectorHandleSegment* handle = segment->GetHandle( vertex );

                oSegmentHandleArray.push_back( handle );
            }
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::Unload()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        //Should be done in UnloadVector directly
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() > 0 )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 signalFlags;

            signalFlags = UnloadVector( vectorScene );

            vectorEngine->Signal( signalFlags );

            if( mBaseHUD )
            {
                mBaseHUD->Unload( vectorScene );
                vectorEngine->RemoveHUD( mBaseHUD );
            }
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::Load()
{
    // we need the focus on the viewport for keyboard 
    TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = GetEditor()->FindTab<FOdysseyPainterEditorViewportTab>();
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    mViewportWidget = viewportTab->GetViewport()->GetViewportWidget();

    mPreviousMouseEvent = eMouseEventName::MouseHover;

    if( hasVector )
    {
        // It would be better if this is done in OnMouseDown()
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() > 0 )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 signalFlags;

            vectorEngine->ClearHUD();

            if( mBaseHUD )
            {
                mBaseHUD->Load( vectorScene );

                vectorEngine->AddHUD( mBaseHUD );
                vectorEngine->ResetHUD();
            }

            signalFlags = LoadVector( vectorScene );

            vectorEngine->Signal( signalFlags );
        }
    }
}

uint64
UOdysseyPainterEditorVectorBaseTool::OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FKey& iKey )
{
    return 0;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnKeyDownGlobal( const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    bool ret = false;

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 signalFlags;

            signalFlags = OnKeyDownGlobalVector( vectorScene, iKey );

            vectorEngine->Signal( signalFlags );
        }
    }

    // always return false to allow other widget to get the event
    return false;
}

uint64
UOdysseyPainterEditorVectorBaseTool::OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
                                                    , const FKey& iKey )
{
    if( iKey == EKeys::Delete )
    {
        Delete();

        return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
    }

    return 0;
}

/** Key down input */
/*bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent )
{
}*/

/** Key up input */
/*bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
}*/

bool
UOdysseyPainterEditorVectorBaseTool::OnKeyDown( const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    bool ret = false;

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 signalFlags;

            signalFlags = OnKeyDownVector( vectorScene, iKey );

            vectorEngine->Signal( signalFlags );

            return signalFlags ? true : false;
        }
    }

    return false;
}

uint64
UOdysseyPainterEditorVectorBaseTool::OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FKey& iKey )
{
    return 0;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnKeyUpGlobal( const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 signalFlags;

            signalFlags = OnKeyUpGlobalVector(vectorScene,iKey);

            vectorEngine->Signal( signalFlags );
        }
    }

    // always return false to allow other widget to get the event
    return false;
}

uint64
UOdysseyPainterEditorVectorBaseTool::OnKeyUpVector( FOdysseyVectorGroupPaint* iScene
                                                  , const FKey& iKey )
{
    return 0;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnKeyUp( const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 signalFlags;

            signalFlags = OnKeyUpVector(vectorScene,iKey);

            vectorEngine->Signal( signalFlags );

            return signalFlags ? true : false;
        }
    }

    return false;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnMouseDown( const FOdysseyPoint& iPointInTexture
                                                , const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    mDragging = false;

    // workaround for buggy stylus drivers
    if( FilterMouseEvent( eMouseEventName::MouseDown ) == false ) 
        return false;

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mAutoCreateMedia ? GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>()
                                                                                : GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 signalFlags;

            signalFlags = OnMouseDownVector( vectorScene, iPointInTexture, iKey );

            vectorEngine->Signal( signalFlags );
        }

       return true;
    }

    return false;
}

void
UOdysseyPainterEditorVectorBaseTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    // workaround for buggy stylus drivers
    if( FilterMouseEvent( eMouseEventName::MouseHover ) == false )
        return;

    // we need the focus on the viewport for keyboard 
    //FSlateApplication::Get().SetKeyboardFocus( mViewportWidget );

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 signalFlags;

            signalFlags = OnMouseHoverVector( vectorScene, iPointInTexture );

            vectorEngine->Signal( signalFlags );
        }
    }
}

// WorkAround for faulty stylus drivers
bool // true = allow, false = reject
UOdysseyPainterEditorVectorBaseTool::FilterMouseEvent( eMouseEventName iCurrentMouseEvent )
{
    bool ret = false;

//UE_LOG(LogTemp, Warning, TEXT("%d : %d"), mPreviousMouseEvent, iCurrentMouseEvent );

    switch( iCurrentMouseEvent )
    {
        case eMouseEventName::MouseDown :
            ret = ( ( mPreviousMouseEvent == eMouseEventName::MouseUp    )
                 || ( mPreviousMouseEvent == eMouseEventName::MouseHover ) ) ? true : false;
        break;

        case eMouseEventName::MouseUp :
            ret = ( ( mPreviousMouseEvent == eMouseEventName::MouseDown  )
                 || ( mPreviousMouseEvent == eMouseEventName::MouseDrag  ) ) ? true : false;
        break;

        case eMouseEventName::MouseDrag :
            ret = ( ( mPreviousMouseEvent == eMouseEventName::MouseDown  )
                 || ( mPreviousMouseEvent == eMouseEventName::MouseDrag  ) ) ? true : false;
        break;

        case eMouseEventName::MouseHover :
            ret = ( ( mPreviousMouseEvent == eMouseEventName::MouseHover )
                 || ( mPreviousMouseEvent == eMouseEventName::MouseUp    ) ) ? true : false;
        break;

        default :
        break;
    }

    if( ret == true )
    {
        mPreviousMouseEvent = iCurrentMouseEvent;
    }

    return ret;
}

void
UOdysseyPainterEditorVectorBaseTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    // workaround for buggy stylus drivers
    if( FilterMouseEvent( eMouseEventName::MouseDrag ) == false )
        return;

    mDragging = true;

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 signalFlags;

            signalFlags = OnMouseDragVector( vectorScene, iPointInTexture );

            vectorEngine->Signal( signalFlags );
        }
    }
}

bool
UOdysseyPainterEditorVectorBaseTool::OnMouseUp( const FOdysseyPoint& iPointInTexture
                                              , const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    bool ret = false;

    // workaround for buggy stylus drivers
    if( FilterMouseEvent( eMouseEventName::MouseUp ) == false )
        return false;

    mDragging = false;

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 signalFlags;

            signalFlags = OnMouseUpVector( vectorScene, iPointInTexture, iKey );

            vectorEngine->Signal( signalFlags );

            if( iKey == EKeys::RightMouseButton )
            {
                if( mHasContextMenu )
                {
                    PopupContextMenu();
                }
            }
        }

       return true;
    }

    return false;
}

uint64
UOdysseyPainterEditorVectorBaseTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FName& iPropertyName )
{
    // RestrictToSelection was changed, return redraw flag
    if( iPropertyName == "RestrictToSelectedObjects" )
    {
        iScene->GetEngine()->ResetHUD();
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

void
UOdysseyPainterEditorVectorBaseTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    // redraw
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if (hasVector)
    {
        //Should be done in OnKeyUpVector directly
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
        if( mediaVectors.Num() > 0 )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 signalFlags;

            signalFlags = PropertyChangedVector( vectorScene, PropertyChangedEvent.GetPropertyName() );

            vectorEngine->Signal( signalFlags );
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::Commit()
{

}

void
UOdysseyPainterEditorVectorBaseTool::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();

    Super::BindShortcuts(iToolkit);

    toolkitCommands->MapAction(
        FGenericCommands::Get().SelectAll,
        FExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::SelectAll )
    );

    toolkitCommands->MapAction(
        FGenericCommands::Get().Delete,
        FExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::Delete )
    );

    toolkitCommands->MapAction(
        FGenericCommands::Get().Copy,
        FExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::Copy )
    );

    toolkitCommands->MapAction(
        FGenericCommands::Get().Paste,
        FExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::Paste )
    );
}

void
UOdysseyPainterEditorVectorBaseTool::Copy()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        // It would be better if this is done in OnMouseDown()
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() > 0 )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();

            if( GetEditor()->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_OBJECT )
            {
                FOdysseyPainterEditor::CopyObjects( vectorScene );
            }
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::Paste()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        // It would be better if this is done in OnMouseDown()
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() > 0 )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();

            if( GetEditor()->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_OBJECT )
            {
                FOdysseyPainterEditor::PasteObjects( vectorScene );
            }
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::SelectAll()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        // It would be better if this is done in OnMouseDown()
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() > 0 )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();

            if( GetEditor()->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_OBJECT )
            {
                FOdysseyPainterEditor::SelectAllObjects( vectorScene );
            }

            if( GetEditor()->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX )
            {
                FOdysseyPainterEditor::SelectAllPoints( vectorScene );
            }
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::Delete()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        // It would be better if this is done in OnMouseDown()
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() > 0 )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();

            if( GetEditor()->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_OBJECT )
            {
                FOdysseyPainterEditor::DeleteObjects( vectorScene );
            }

            if( GetEditor()->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX )
            {
                FOdysseyPainterEditor::DeletePointSelection( vectorScene );
            }
        }
    }
}

TSharedPtr<SWidget>
UOdysseyPainterEditorVectorBaseTool::CreatePropertyWidget( TSharedPtr<class IPropertyHandle> iPropertyHandle
                                                         , const TSharedPtr<ISinglePropertyView> iView )
{
    if (!iPropertyHandle)
        return nullptr;

    TSharedRef<SWidget> nameWidget = iPropertyHandle->CreatePropertyNameWidget();
    TSharedRef<SWidget> valueWidget = iPropertyHandle->CreatePropertyValueWidget(false);

    iView->SetVisibility(EVisibility::Collapsed);

    return SNew(SHorizontalBox)
    + SHorizontalBox::Slot()
    .AutoWidth()
    [
        //PATCH:
        iView.ToSharedRef()
    ]
    + SHorizontalBox::Slot()
    .AutoWidth()
    .Padding(0.f, 0.f, 3.f, 0.f)
    [
        nameWidget
    ]
    + SHorizontalBox::Slot()
    [
        valueWidget
    ];
}

TSharedRef<SWidget>
UOdysseyPainterEditorVectorBaseTool::CreateTopTabWidget()
{
    return SNew(SUniformWrapPanel)
           .SlotPadding(FVector2D(3.f, 0.f))
           .EvenRowDistribution(true)
           .HAlign(HAlign_Left)
           + SUniformWrapPanel::Slot()
           [
               SNew( SOdysseyPainterEditorVectorEditionMode, GetEditor() )
           ];
}

void
UOdysseyPainterEditorVectorBaseTool::PopupContextMenu()
{
    TSharedPtr<SWidget> contextMenu = CreateContextMenu();

    TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = GetEditor()->FindTab<FOdysseyPainterEditorViewportTab>();
    FSlateApplication::Get().PushMenu( viewportTab->Widget().ToSharedRef(),
                                       FWidgetPath(),
                                       contextMenu.ToSharedRef(),
                                       FSlateApplication::Get().GetCursorPos(),
                                       FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu) );
}

TSharedPtr<SWidget>
UOdysseyPainterEditorVectorBaseTool::CreateContextMenu()
{
    FMenuBuilder menu( true, nullptr );

    ExtendContextMenu( menu );

    return menu.MakeWidget();
}

void
UOdysseyPainterEditorVectorBaseTool::ExtendContextMenu( FMenuBuilder& menu )
{
    if( GetEditor()->GetVectorHUDFlags() &  FOdysseyVectorHUD::HUD_MODE_OBJECT )
    {
        ExtendContextMenuObject( menu );
    }

    if( GetEditor()->GetVectorHUDFlags() &  FOdysseyVectorHUD::HUD_MODE_VERTEX )
    {
        ExtendContextMenuVertex( menu );
    }
}

void
UOdysseyPainterEditorVectorBaseTool::ExtendContextMenuObject( FMenuBuilder& menu )
{
    //FMenuBuilder menu( true, nullptr );
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();

        // Commented-out: sections are not needed here as they would conflict with the section
        // just created by the Edit Menu when this tool's menu appears in the Edit Menu
        // See FOdysseyPainterEditor::AddEditMenuEntry() for details
        //    menu.BeginSection("Context");
        //    {
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.reset-view.name", "Reset View")
                    , LOCTEXT("vector-tool.object-context-menu.reset-view.tooltip", "Reset View")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::ResetView, vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.group-paint.name", "Make Paint Group")
                    , LOCTEXT("vector-tool.object-context-menu.group-paint.tooltip", "Make Paint Group")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::MakePaintGroup, vectorScene )));
                /*menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.trim.name", "Trim")
                    , LOCTEXT("vector-tool.object-context-menu.trim.tooltip", "Trim")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::Trim, vectorScene)));*/
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.group.name", "Group")
                    , LOCTEXT("vector-tool.object-context-menu.group.tooltip", "Group")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::Group, vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.ungroup.name", "Ungroup")
                    , LOCTEXT("vector-tool.object-context-menu.ungroup.tooltip", "Ungroup")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::Ungroup, vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.bring-forward.name", "Bring forward")
                    , LOCTEXT("vector-tool.object-context-menu.bring-forward.tooltip", "Bring forward")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::BringForward, vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.send-backward.name", "Send backward")
                    , LOCTEXT("vector-tool.object-context-menu.send-backward.tooltip", "Send backward")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::SendBackward, vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.delete-selection.name","Delete Selection")
                    , LOCTEXT("vector-tool.object-context-menu.delete-selection.tooltip","Delete Selection")
                    , FSlateIcon("OdysseyStyle","OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::DeleteObjects, vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.flip-horizontal.name","Flip Horizontal")
                    , LOCTEXT("vector-tool.object-context-menu.flip-horizontal.tooltip","Flip Horizontal")
                    , FSlateIcon("OdysseyStyle","OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::FlipHorizontal, vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.flip-vertical.name","Flip Vertical")
                    , LOCTEXT("vector-tool.object-context-menu.flip-vertical.tooltip","Flip Vertical")
                    , FSlateIcon("OdysseyStyle","OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::FlipVertical, vectorScene )));
                menu.AddMenuEntry(
                    LOCTEXT("vector-tool.object-context-menu.clear-coloring.name", "Clear Coloring")
                    , LOCTEXT("vector-tool.object-context-menu.clear-coloring.tooltip", "Clear Coloring")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::ClearColoring, vectorScene )));
                menu.AddMenuEntry(
                    LOCTEXT("vector-tool.object-context-menu.apply-transformations.name", "Apply Transformations")
                    , LOCTEXT("vector-tool.object-context-menu.apply-transformations.tooltip", "Apply Transformations")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::ApplyTransformations, vectorScene )));
        //    }
        //    menu.EndSection();
        }
    }

    //return menu.MakeWidget();
}

void
UOdysseyPainterEditorVectorBaseTool::ExtendContextMenuVertex( FMenuBuilder& menu )
{
    //FMenuBuilder menu( true, nullptr );
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();

        // Commented-out: sections are not needed here as they would conflict with the section
        // just created by the Edit Menu when this tool's menu appears in the Edit Menu
        // See FOdysseyPainterEditor::AddEditMenuEntry() for details
        //     menu.BeginSection("Context");
        //     {
            menu.AddMenuEntry(
                  LOCTEXT("vector-tool.vertex-context-menu.delete-selection.name", "Delete Selection")
                , LOCTEXT("vector-tool.vertex-context-menu.delete-selection.tooltip", "Delete Selection")
                , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::DeletePointSelection, vectorScene )));
            menu.AddMenuEntry(
                  LOCTEXT("vector-tool.vertex-context-menu.align-point-selection.name", "Align Point Selection")
                , LOCTEXT("vector-tool.vertex-context-menu.align-point-selection.tooltip", "Align Point Selection")
                , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::AlignPointSelection, vectorScene )));
            menu.AddMenuEntry(
                  LOCTEXT("vector-tool.vertex-context-menu.unalign-point-selection.name", "Unalign Point Selection")
                , LOCTEXT("vector-tool.vertex-context-menu.unalign-point-selection.tooltip", "Unalign Point Selection")
                , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::UnalignPointSelection, vectorScene )));
        //    }
        //    menu.EndSection();
        }
    }

    //return menu.MakeWidget();
}

#ifndef M_PI
#define M_PI 3.14159265359f
#endif

void
UOdysseyPainterEditorVectorBaseTool::MakeTest( FOdysseyVectorGroupPaint* iScene )
{
    uint32 rayCount = 100;
    double width = 4.0f;
    double radius = 250;
    double angleStep = 2.0f * M_PI / rayCount;
    double angle = angleStep / 2.0f;
    FOdysseyVectorEllipse* ellipse = new FOdysseyVectorEllipse( "ellipse"
                                                              , 60.0f
                                                              , 60.0f
                                                              , width );
    std::vector<FOdysseyVectorPath*> raysArray;

    raysArray.reserve( rayCount );

    for( uint32 i = 0; i < rayCount; i++ )
    {
        FOdysseyVectorPath* path = new FOdysseyVectorPath( "test" );
        FOdysseyVectorVertex* vertex0 = new FOdysseyVectorVertex( 0.0f, 0.0f, width );
        FOdysseyVectorVertex* vertex1 = new FOdysseyVectorVertex( cos( angle ) * radius
                                                                , sin( angle ) * radius
                                                                , width );
        FOdysseyVectorSegmentCubic* cubicSegment = new FOdysseyVectorSegmentCubic( path
                                                                                 , vertex0
                                                                                 , vertex1
                                                                                 , true );
        path->AddVertex( vertex0 );
        path->AddVertex( vertex1 );
        path->AddSegment( cubicSegment );

        iScene->AppendChild( path );

        raysArray.push_back( path );

        angle += angleStep;
    }

    iScene->AppendChild( ellipse->Convert() );

    iScene->UpdateMatrix();
    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    // checks multiple times
    for( int i = 0; i < 2; i++ )
    {
        UE_LOG(LogTemp, Error, TEXT("Check: %d"), i );

        // each vertex should have 64 sections.
        for( FOdysseyVectorPath* ray : raysArray )
        {
            FOdysseyVectorSegment* segment = ray->GetFirstSegment();
            FOdysseyVectorVertex* vertex0 = segment->GetVertex(0);
            FOdysseyVectorVertex* vertex1 = segment->GetVertex(1);

            // check number of sections.
            if( vertex0->GetSectionCount() != rayCount )
            {
                UE_LOG(LogTemp, Error, TEXT("Inconsistency in test at ray/vertex0 : real:%d - exp:%d"), vertex0->GetSectionCount(), rayCount);
            }

            // at vertex 1 section are simplified, hence 0
            if( vertex1->GetSectionCount() != 0 )
            {
                UE_LOG(LogTemp, Error, TEXT("Inconsistency in test at ray/vertex1 : real:%d - exp:%d"), vertex1->GetSectionCount(), rayCount);
            }
        }

        // check intersection
        for( FOdysseyVectorIntersection* intersection : iScene->GetIntersectionArray() )
        {
            FOdysseyVectorVertexIntersection* vertex0 = intersection->GetVertex(0);
            FOdysseyVectorVertexIntersection* vertex1 = intersection->GetVertex(1);

            // check number of sections.
            if( vertex0->GetSectionCount() != 3 )
            {
                UE_LOG(LogTemp, Error, TEXT("Inconsistency in test at paintgroup/vertex0 : %d"), vertex0->GetSectionCount());
            }

            if( vertex1->GetSectionCount() != 3 )
            {
                UE_LOG(LogTemp, Error, TEXT("Inconsistency in test at paintgroup/vertex1 : %d"), vertex1->GetSectionCount());
            }
        }

        // force finding cycles again
        iScene->SetPainted( false );
        iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );
        iScene->SetPainted( true );
        iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );
    }
}

#undef LOCTEXT_NAMESPACE

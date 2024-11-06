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
#include "ISinglePropertyView.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEllipse.h"

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

void
UOdysseyPainterEditorVectorBaseTool::ExtendMenu(TSharedRef<FExtender> iExtender)
{
    TSharedPtr<FUICommandList> commandList = MakeShared<FUICommandList>();
    iExtender->AddMenuExtension(
        "OdysseyEdit",
        EExtensionHook::After,
        commandList,
        FMenuExtensionDelegate::CreateLambda(
            [this](FMenuBuilder& iBuilder)
            {
                if (!IsActivated())
                    return;

                iBuilder.BeginSection("ToolOptions", LOCTEXT("vector-base-tool.edit-menu.tool-options", "Tool Options"));
                {
                    ExtendContextMenu( iBuilder );
                }
                iBuilder.EndSection();
            }
        )
    );

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
                                                 , eForegroundColorMode iColorMode )
{
    switch( iColorMode )
    {
        case eForegroundColorMode::SolidColor:
        {
            ::ULIS::FColor color = GetEditor()->PaintColor().GetValue();
            ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );
            FColor ueColor = FColor( rgba8.R8(), rgba8.G8(), rgba8.B8(), rgba8.A8() );

            iPath->GetForegroundBucket().SetSolidColor( ueColor );
        }
        break;

        case eForegroundColorMode::Palette:
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

    iPath->GetForegroundBucket().SetColorMode( (eBucketColorMode) iColorMode );
}

FOdysseyVectorSegment*
UOdysseyPainterEditorVectorBaseTool::PickSegments( FOdysseyVectorGroupPaint* iScene
                                                 , double iWorldX
                                                 , double iWorldY
                                                 , double iWorldRadius
                                                 , bool iRestrictToSelection
                                                 , bool iStopImmediately
                                                 , std::vector<FOdysseyVectorSegment*>& oPickedSegmentArray )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    FOdysseyVectorSegment* closestSegment = nullptr;
    std::vector<double> distanceArray;
    double smallestDistance = DBL_MAX;

    oPickedSegmentArray.clear();

    vectorEngine->Traverse
    ( iScene
    , 0
    , [ iScene
      , vectorEngine
      , &closestSegment
      , &iWorldX
      , &iWorldY
      , &iWorldRadius
      , &iRestrictToSelection
      , &iStopImmediately
      , &oPickedSegmentArray
      , &distanceArray ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
      {
          if( vectorEngine->ObjectHasFocus( iScene, object, traversalFlags ) || ( iRestrictToSelection == false ) )
          {
              if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
              {
                  FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

                  path->PickSegments( iWorldX
                                    , iWorldY
                                    , iWorldRadius
                                    , oPickedSegmentArray
                                    , &distanceArray );

                  if( oPickedSegmentArray.size() && iStopImmediately )
                  {
                      return FOdysseyVectorEngine::TRAVERSE_STOP;
                  }
              }

              return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
          }

          return 0;
      } );

    for( int i = 0; i < oPickedSegmentArray.size(); i++ )
    {
        if( distanceArray[i] < smallestDistance )
        {
            smallestDistance = distanceArray[i];

            closestSegment = oPickedSegmentArray[i];
        }
    }

    return closestSegment;
}


void
UOdysseyPainterEditorVectorBaseTool::GetSelectedVertices( FOdysseyVectorGroupPaint* iScene
                                                        , std::vector<FOdysseyVectorVertex*>& oSelectedVertexArray )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    oSelectedVertexArray.clear();

    vectorEngine->Traverse
    ( iScene
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
            uint64 notificationFlags;

            notificationFlags = UnloadVector( vectorScene );

            FOdysseyVectorEngine::Notify( vectorScene, notificationFlags );

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
    UOdysseyPainterEditorTool::Load();
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
            uint64 notificationFlags;

            vectorEngine->ClearHUD();

            if( mBaseHUD )
            {
                mBaseHUD->Load( vectorScene );

                vectorEngine->AddHUD( mBaseHUD );
                vectorEngine->ResetHUD();
            }

            notificationFlags = LoadVector( vectorScene );

            FOdysseyVectorEngine::Notify( vectorScene, notificationFlags );
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
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    bool hasVector = mediaProvider.HasMedia<FOdysseyMediaVector>();
    bool ret = false;

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mediaProvider.GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 notificationFlags;

            notificationFlags = OnKeyDownGlobalVector( vectorScene, iKey );

            FOdysseyVectorEngine::Notify( vectorScene, notificationFlags );
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

        // force redraw
        iScene->GetEngine()->Invalidate( 0 );

        return 0;
    }

    //if( iKey == EKeys::Add )
    //{
    //    IncreaseContourWidth();

    //    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
    //}

    //if( iKey == EKeys::Subtract )
    //{
    //    DecreaseContourWidth();

    //    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
    //}

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
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    bool hasVector = mediaProvider.HasMedia<FOdysseyMediaVector>();
    bool ret = false;

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mediaProvider.GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 notificationFlags;

            notificationFlags = OnKeyDownVector( vectorScene, iKey );

            FOdysseyVectorEngine::Notify( vectorScene, notificationFlags );

            return notificationFlags ? true : false;
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
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    bool hasVector = mediaProvider.HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mediaProvider.GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 notificationFlags;

            notificationFlags = OnKeyUpGlobalVector(vectorScene,iKey);

            FOdysseyVectorEngine::Notify( vectorScene, notificationFlags );
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
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    bool hasVector = mediaProvider.HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mediaProvider.GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 notificationFlags;

            notificationFlags = OnKeyUpVector(vectorScene,iKey);

            FOdysseyVectorEngine::Notify( vectorScene, notificationFlags );

            return notificationFlags ? true : false;
        }
    }

    return false;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnMouseDown( const FOdysseyPoint& iPointInTexture
                                                , const FKey& iKey )
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    bool hasVector = mediaProvider.HasMedia<FOdysseyMediaVector>();

    mDragging = false;

    // workaround for buggy stylus drivers
    if( FilterMouseEvent( eMouseEventName::MouseDown ) == false )
        return false;

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mAutoCreateMedia ? mediaProvider.GetOrCreateMedias<FOdysseyMediaVector>()
                                                                                : mediaProvider.GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 notificationFlags;

            notificationFlags = OnMouseDownVector( vectorScene, iPointInTexture, iKey );

            FOdysseyVectorEngine::Notify( vectorScene, notificationFlags );
        }

       return true;
    }

    return false;
}

void
UOdysseyPainterEditorVectorBaseTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;

    bool hasVector = mediaProvider.HasMedia<FOdysseyMediaVector>();

    // workaround for buggy stylus drivers
    if( FilterMouseEvent( eMouseEventName::MouseHover ) == false )
        return;

    // we need the focus on the viewport for keyboard
    //FSlateApplication::Get().SetKeyboardFocus( mViewportWidget );

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mediaProvider.GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 notificationFlags;

            notificationFlags = OnMouseHoverVector( vectorScene, iPointInTexture );

            FOdysseyVectorEngine::Notify( vectorScene, notificationFlags );
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
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;

    bool hasVector = mediaProvider.HasMedia<FOdysseyMediaVector>();

    // workaround for buggy stylus drivers
    if( FilterMouseEvent( eMouseEventName::MouseDrag ) == false )
        return;

    mDragging = true;

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mediaProvider.GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num())
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 notificationFlags;

            notificationFlags = OnMouseDragVector( vectorScene, iPointInTexture );

            FOdysseyVectorEngine::Notify( vectorScene, notificationFlags );
        }
    }
}

bool
UOdysseyPainterEditorVectorBaseTool::OnMouseUp( const FOdysseyPoint& iPointInTexture
                                              , const FKey& iKey )
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    bool hasVector = mediaProvider.HasMedia<FOdysseyMediaVector>();
    bool ret = false;

    // workaround for buggy stylus drivers
    if( FilterMouseEvent( eMouseEventName::MouseUp ) == false )
        return false;

    mDragging = false;

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mediaProvider.GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            uint64 notificationFlags;

            notificationFlags = OnMouseUpVector( vectorScene, iPointInTexture, iKey );

            FOdysseyVectorEngine::Notify( vectorScene, notificationFlags );

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
    /* if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorBaseTool, RestrictToSelectedObjects) )
    {
        iScene->GetEngine()->ResetHUD();
    } */

    return 0;
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
            uint64 notificationFlags;

            notificationFlags = PropertyChangedVector( vectorScene, PropertyChangedEvent.GetPropertyName() );

            FOdysseyVectorEngine::Notify( vectorScene, notificationFlags );
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
        FGenericCommands::Get().Cut,
        FExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::Cut )
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
                FOdysseyPainterEditor::PasteObjects( GetEditor(), vectorScene );
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
                FOdysseyPainterEditor::SelectAllObjects( GetEditor(), vectorScene );
            }

            if( GetEditor()->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX )
            {
                FOdysseyPainterEditor::SelectAllPoints( GetEditor(), vectorScene );
            }
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::IncreaseContourWidth()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        // It would be better if this is done in OnMouseDown()
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() > 0 )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();

            FOdysseyPainterEditor::AlterContourWidth( vectorScene, 1.1f, false );
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::DecreaseContourWidth()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        // It would be better if this is done in OnMouseDown()
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() > 0 )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();

            FOdysseyPainterEditor::AlterContourWidth( vectorScene, 0.9f, false );
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::Cut()
{
    UOdysseyPainterEditorVectorBaseTool::Copy();
    UOdysseyPainterEditorVectorBaseTool::Delete();
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
                FOdysseyPainterEditor::DeleteObjects( GetEditor(), vectorScene );
            }

            if( GetEditor()->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX )
            {
                FOdysseyPainterEditor::DeletePointSelection( GetEditor(), vectorScene );
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

    TSharedPtr<SWindow> window = FSlateApplication::Get().GetActiveTopLevelWindow();
    if (!window)
        return;

    FSlateApplication::Get().PushMenu( window.ToSharedRef(),
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

    if( GetEditor()->GetVectorHUDFlags() &  FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        ExtendContextMenuInbetween( menu );
    }
}

void
UOdysseyPainterEditorVectorBaseTool::ExtendContextMenuObject( FMenuBuilder& menu )
{
    //FMenuBuilder menu( true, nullptr );
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;

    bool hasVector = mediaProvider.HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mediaProvider.GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() )
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
                    , FSlateIcon()
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::ResetView, GetEditor(), vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.group-paint.name", "Make Paint Group")
                    , LOCTEXT("vector-tool.object-context-menu.group-paint.tooltip", "Make Paint Group")
                    , FSlateIcon()
                    , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::MakePaintGroup, GetEditor(), vectorScene )));
                /*menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.trim.name", "Trim")
                    , LOCTEXT("vector-tool.object-context-menu.trim.tooltip", "Trim")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::Trim, vectorScene)));*/
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.group.name", "Group")
                    , LOCTEXT("vector-tool.object-context-menu.group.tooltip", "Group")
                    , FSlateIcon()
                    , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::Group, GetEditor(), vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.ungroup.name", "Ungroup")
                    , LOCTEXT("vector-tool.object-context-menu.ungroup.tooltip", "Ungroup")
                    , FSlateIcon()
                    , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::Ungroup, GetEditor(), vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.bring-forward.name", "Bring forward")
                    , LOCTEXT("vector-tool.object-context-menu.bring-forward.tooltip", "Bring forward")
                    , FSlateIcon()
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::BringForward, GetEditor(), vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.send-backward.name", "Send backward")
                    , LOCTEXT("vector-tool.object-context-menu.send-backward.tooltip", "Send backward")
                    , FSlateIcon()
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::SendBackward, GetEditor(), vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.delete-selection.name","Delete Selection")
                    , LOCTEXT("vector-tool.object-context-menu.delete-selection.tooltip","Delete Selection")
                    , FSlateIcon()
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::DeleteObjects, GetEditor(), vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.flip-horizontal.name","Flip Horizontal")
                    , LOCTEXT("vector-tool.object-context-menu.flip-horizontal.tooltip","Flip Horizontal")
                    , FSlateIcon()
                    , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::FlipHorizontal, GetEditor(), vectorScene )));
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.object-context-menu.flip-vertical.name","Flip Vertical")
                    , LOCTEXT("vector-tool.object-context-menu.flip-vertical.tooltip","Flip Vertical")
                    , FSlateIcon()
                    , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::FlipVertical, GetEditor(), vectorScene )));
                menu.AddMenuEntry(
                    LOCTEXT("vector-tool.object-context-menu.clear-coloring.name", "Clear Coloring")
                    , LOCTEXT("vector-tool.object-context-menu.clear-coloring.tooltip", "Clear Coloring")
                    , FSlateIcon()
                    , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::ClearColoring, GetEditor(), vectorScene )));
                menu.AddMenuEntry(
                    LOCTEXT("vector-tool.object-context-menu.apply-transformations.name", "Apply Transformations")
                    , LOCTEXT("vector-tool.object-context-menu.apply-transformations.tooltip", "Apply Transformations")
                    , FSlateIcon()
                    , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::ApplyTransformations, GetEditor(), vectorScene )));
                //menu.AddMenuEntry(
                //    LOCTEXT("vector-tool.object-context-menu.apply-transformations.name", "Make DemoBrush")
                //    , LOCTEXT("vector-tool.object-context-menu.apply-transformations.tooltip", "Make DemoBrush")
                //    , FSlateIcon()
                //    , FUIAction(FExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::MakeDemoBrush, vectorScene )));
        //    }
        //    menu.EndSection();
        }
    }

    //return menu.MakeWidget();
}

// for testing purpose.
void
UOdysseyPainterEditorVectorBaseTool::MakeDemoBrush( FOdysseyVectorGroupPaint* iScene )
{
   std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetEngine()->GetSelectedObjectList();

   FOdysseyVectorBrush::MakeDemoBrush( selectedObjectList
                                     , FOdysseyVectorObject::GetBoundingBoxFromList( selectedObjectList ) );
}

void
UOdysseyPainterEditorVectorBaseTool::ExtendContextMenuVertex( FMenuBuilder& menu )
{
    //FMenuBuilder menu( true, nullptr );
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;

    bool hasVector = mediaProvider.HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mediaProvider.GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() )
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
                , FSlateIcon()
                , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::DeletePointSelection, GetEditor(), vectorScene )));
            menu.AddMenuEntry(
                  LOCTEXT("vector-tool.vertex-context-menu.align-point-selection.name", "Align Point Selection")
                , LOCTEXT("vector-tool.vertex-context-menu.align-point-selection.tooltip", "Align Point Selection")
                , FSlateIcon()
                , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::AlignPointSelection, GetEditor(), vectorScene )));
            menu.AddMenuEntry(
                  LOCTEXT("vector-tool.vertex-context-menu.unalign-point-selection.name", "Unalign Point Selection")
                , LOCTEXT("vector-tool.vertex-context-menu.unalign-point-selection.tooltip", "Unalign Point Selection")
                , FSlateIcon()
                , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::UnalignPointSelection, GetEditor(), vectorScene )));
            menu.AddMenuEntry(
                  LOCTEXT("vector-tool.vertex-context-menu.lock-point-selection.name", "Lock Point Selection")
                , LOCTEXT("vector-tool.vertex-context-menu.lock-point-selection.tooltip", "Lock Point Selection")
                , FSlateIcon()
                , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::LockPointSelection, GetEditor(), vectorScene )));
            menu.AddMenuEntry(
                  LOCTEXT("vector-tool.vertex-context-menu.unlock-point-selection.name", "Unlock Point Selection")
                , LOCTEXT("vector-tool.vertex-context-menu.unlock-point-selection.tooltip", "Unlock Point Selection")
                , FSlateIcon()
                , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::UnlockPointSelection, GetEditor(), vectorScene )));
        //    }
        //    menu.EndSection();
        }
    }

    //return menu.MakeWidget();
}

void
UOdysseyPainterEditorVectorBaseTool::ExtendContextMenuInbetween( FMenuBuilder& menu )
{
    //FMenuBuilder menu( true, nullptr );
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;

    bool hasVector = mediaProvider.HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mediaProvider.GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();

        // Commented-out: sections are not needed here as they would conflict with the section
        // just created by the Edit Menu when this tool's menu appears in the Edit Menu
        // See FOdysseyPainterEditor::AddEditMenuEntry() for details
        //     menu.BeginSection("Context");
        //     {
            if( vectorScene->GetEngine()->GetSelectedObjectList().size() > 1 )
            {
                menu.AddMenuEntry(
                      LOCTEXT("vector-tool.inbetween-context-menu.groupadd-inbetweener-tag.name", "Group and Add Inbetweener Grid")
                    , LOCTEXT("vector-tool.inbetween-context-menu.groupadd-inbetweener-tag.tooltip", "Group and Add Inbetweener Grid")
                    , FSlateIcon()
                    , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::GroupAndAddInbetweenerTag, GetEditor(), vectorScene )));
            }

            menu.AddMenuEntry(
                    LOCTEXT("vector-tool.inbetween-context-menu.add-inbetweener-tag.name", "Add Inbetweener Tag")
                  , LOCTEXT("vector-tool.inbetween-context-menu.add-inbetweener-tag.tooltip", "Add Inbetweener Tag")
                  , FSlateIcon()
                  , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::AddInbetweenerTag, GetEditor(), vectorScene )));

            menu.AddMenuEntry(
                    LOCTEXT("vector-tool.inbetween-context-menu.remove-inbetweener-tag.name", "Remove Inbetweener Tag")
                  , LOCTEXT("vector-tool.inbetween-context-menu.remove-inbetweener-tag.tooltip", "Remove Inbetweener Tag")
                  , FSlateIcon()
                  , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::RemoveInbetweenerTag, GetEditor(), vectorScene )));

            menu.AddMenuEntry(
                    LOCTEXT("vector-tool.inbetween-context-menu.commit-inbetweener-tag.name", "Commit Inbetweener Tag")
                  , LOCTEXT("vector-tool.inbetween-context-menu.commit-inbetweener-tag.tooltip", "Commit Inbetweener Tag")
                  , FSlateIcon()
                  , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::CommitSelectedInbetweenerTag, GetEditor(), vectorScene->GetSharedEnv() )));
/*
            menu.AddMenuEntry(
                  LOCTEXT("vector-tool.inbetween-context-menu.reset-grid.name", "Reset Grid")
                , LOCTEXT("vector-tool.inbetween-context-menu.reset-grid.tooltip", "Reset Grid")
                , FSlateIcon()
                , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::ResetInbetweenerGrid, GetEditor(), vectorScene )));
*/
            menu.AddSubMenu(
                  LOCTEXT("vector-tool.inbetween-context-menu.reset-grid.name", "Reset Grid")
                , LOCTEXT("vector-tool.inbetween-context-menu.reset-grid.tooltip", "Reset Grid")
                , FNewMenuDelegate::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::ResetGridMenu, vectorScene ) );

            menu.AddMenuEntry(
                  LOCTEXT("vector-tool.inbetween-context-menu.reset-breakdown-spacing-chart.name", "Reset Spacing" )
                , LOCTEXT("vector-tool.inbetween-context-menu.reset-breakdown-spacing-chart.tooltip", "Reset Spacing" )
                , FSlateIcon()
                , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::ResetBreakdownSpacingChart, GetEditor(), vectorScene, false )));

            menu.AddMenuEntry(
                  LOCTEXT("vector-tool.inbetween-context-menu.reset-breakdown-spacing-chart.name", "Reset Chart" )
                , LOCTEXT("vector-tool.inbetween-context-menu.reset-breakdown-spacing-chart.tooltip", "Reset Chart" )
                , FSlateIcon()
                , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::ResetBreakdownSpacingChart, GetEditor(), vectorScene, true )));

            menu.AddMenuEntry(
                  LOCTEXT("vector-tool.object-context-menu.delete-selection.name","Delete Selection")
                , LOCTEXT("vector-tool.object-context-menu.delete-selection.tooltip","Delete Selection")
                , FSlateIcon()
                , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::DeleteObjects, GetEditor(), vectorScene )));
/*
            menu.AddMenuEntry(
                  LOCTEXT("vector-tool.inbetween-context-menu.copy-spacing-chart.name", "Copy Spacing Chart")
                , LOCTEXT("vector-tool.inbetween-context-menu.copy-spacing-chart.tooltip", "Copy Spacing Chart")
                , FSlateIcon()
                , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::CopySpacingChart, GetEditor(), vectorScene )));

            menu.AddMenuEntry(
                  LOCTEXT("vector-tool.inbetween-context-menu.paste-spacing-chart.name", "Paste Spacing Chart")
                , LOCTEXT("vector-tool.inbetween-context-menu.paste-spacing-chart.tooltip", "Paste Spacing Chart")
                , FSlateIcon()
                , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::PasteSpacingChart, GetEditor(), vectorScene )));
*/
        }
    }

    //return menu.MakeWidget();
}

void
UOdysseyPainterEditorVectorBaseTool::ResetGridMenu( FMenuBuilder& menu
                                                  , FOdysseyVectorGroupPaint* iScene )
{
    menu.AddMenuEntry(
          LOCTEXT("vector-tool.inbetween-context-menu.reset-grid-transformation.name", "Transformation")
        , LOCTEXT("vector-tool.inbetween-context-menu.reset-grid-transformation.tooltip", "Transformation")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::ResetInbetweenerGrid, GetEditor(), iScene, true, false )));

    menu.AddMenuEntry(
          LOCTEXT("vector-tool.inbetween-context-menu.reset-grid-deformation.name", "Deformation")
        , LOCTEXT("vector-tool.inbetween-context-menu.reset-grid-deformation.tooltip", "Deformation")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::ResetInbetweenerGrid, GetEditor(), iScene, false, true )));

    menu.AddMenuEntry(
          LOCTEXT("vector-tool.inbetween-context-menu.reset-grid-both.name", "Both")
        , LOCTEXT("vector-tool.inbetween-context-menu.reset-grid-both.tooltip", "Both")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::ResetInbetweenerGrid, GetEditor(), iScene, true, true )));
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
    double angle = 0;
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
    iScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
}

EMouseCursor::Type
UOdysseyPainterEditorVectorBaseTool::GetMouseCursor() const
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return EMouseCursor::SlashedCircle;

    return UOdysseyPainterEditorTool::GetMouseCursor();
}

#undef LOCTEXT_NAMESPACE

// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
#include "ISinglePropertyView.h"
#include "Toolkits/BaseToolkit.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorEllipse.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorBaseTool::~UOdysseyPainterEditorVectorBaseTool()
{
    //if( mBaseHUD )
    //{
    //    delete mBaseHUD;
    //}
}

UOdysseyPainterEditorVectorBaseTool::UOdysseyPainterEditorVectorBaseTool()
    : mBaseHUD( nullptr )
    , mHasContextMenu( true )
{

}

UOdysseyPainterEditorVectorBaseTool::UOdysseyPainterEditorVectorBaseTool( TSharedPtr<FOdysseyPainterEditorVectorBaseToolHUD> iBaseHUD
                                                                        , bool iAutoCreateMedia
                                                                        , bool iMouseEventViaHUD )
    : mBaseHUD( iBaseHUD )
    , mHasContextMenu(true)
    , mAutoCreateMedia( iAutoCreateMedia )
    , bMouseEventViaHUD( iMouseEventViaHUD )
    , mHUDCell( nullptr )
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
UOdysseyPainterEditorVectorBaseTool::SetPathColor( FOdysseyVectorPath* iPath )
{
    eBucketColorMode colorMode = eBucketColorMode::SolidColor;
    switch( GetEditor()->GetColorType() )
    {
        case EOdysseyPainterEditorColorType::Raw:
        {
            ::ULIS::FColor color = GetEditor()->PaintColor().GetValue();
            ::ULIS::FColor rgba8 = color.ToFormat( ::ULIS::eFormat::Format_RGBA8 );
            FColor ueColor = FColor( rgba8.R8(), rgba8.G8(), rgba8.B8(), rgba8.A8() );

            iPath->GetForegroundBucket().SetSolidColor( ueColor );
            colorMode = eBucketColorMode::SolidColor;
        }
        break;

        case EOdysseyPainterEditorColorType::Indexed:
        {
            iPath->GetForegroundBucket().SetPaletteEntry( GetEditor()->GetCurrentPaletteColorEntry() );
            iPath->GetForegroundBucket().SetPaletteSet( GetEditor()->GetCurrentPaletteSet() );
            colorMode = eBucketColorMode::Palette;
        }
        break;

        default:
        break;
    }

    iPath->GetForegroundBucket().SetColorMode( colorMode );
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
    FOdysseyVectorSegment* closestSegment = nullptr;
    std::vector<double> distanceArray;
    double smallestDistance = DBL_MAX;

    oPickedSegmentArray.clear();

    FOdysseyVectorObject::Traverse
    ( iScene
    , 0
    , [ iScene
      , &closestSegment
      , &iWorldX
      , &iWorldY
      , &iWorldRadius
      , &iRestrictToSelection
      , &iStopImmediately
      , &oPickedSegmentArray
      , &distanceArray ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
      {
          if( iScene->GetCell()->ObjectHasFocus( object, traversalFlags ) || ( iRestrictToSelection == false ) )
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
                      return FOdysseyVectorObject::TRAVERSE_STOP;
                  }
              }

              return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED;
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
    oSelectedVertexArray.clear();

    FOdysseyVectorObject::Traverse
    ( iScene
    , 0
    , [ this
      , iScene
      , &oSelectedVertexArray ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
      {
          if( iScene->GetCell()->ObjectHasFocus( object, traversalFlags ) )
          {
              if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
              {
                  FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

                  path->GetSelectedVertices( oSelectedVertexArray );
              }

              return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED;
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
    FOdysseyVectorGroupPaint* vectorScene = nullptr;
    uint64 notificationFlags = 0;

    UOdysseyPainterEditorTool::Unload();

    if( hasVector )
    {
        //Should be done in UnloadVector directly
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();


        if( mediaVectors.Num() > 0 )
        {
            vectorScene = mediaVectors[0]->GetScene();
            notificationFlags = UnloadVector( vectorScene );
        }
    }

    FOdysseyVectorEngine::Notify( vectorScene, notificationFlags );

    if( mBaseHUD )
    {
        mBaseHUD->Unload();
        // 2D HUD
        // Note: Unload() might be call on a frame without a cell, thus vectorScene would be NULL
        // and we would not be able to unload the HUD from FOdysseyVectorCell:mHUDList
        if( mHUDCell )
        {
            mHUDCell->RemoveHUD( mBaseHUD.Get() );
        }
        // 3D HUD
        mHUD->RemoveElement( mBaseHUD );
    }
}

void
UOdysseyPainterEditorVectorBaseTool::Load()
{
    TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = GetEditor()->FindTab<FOdysseyPainterEditorViewportTab>();

    UOdysseyPainterEditorTool::Load();
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    // we need the focus on the viewport for keyboard
    // and some tools need to know the viewport size
    mViewport = viewportTab->GetViewport();

    mPreviousMouseEvent = eMouseEventName::MouseHover;

    UOdysseyPainterEditorTool::Load();

    mHUDCell = nullptr;

    if( hasVector )
    {
        // It would be better if this is done in OnMouseDown()
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() > 0 )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            uint64 notificationFlags;

            vectorScene->GetCell()->ClearHUD();

            if( mBaseHUD )
            {
                mBaseHUD->SetScene( vectorScene );
                mBaseHUD->Load();
                // 2D HUD
                // Note: Unload() might be call on a frame without a cell, thus vectorScene would be NULL
                // and we would not be able to unload the HUD from FOdysseyVectorCell:mHUDList
                mHUDCell = vectorScene->GetCell();
                mHUDCell->AddHUD( mBaseHUD.Get() );
                // 3D HUD
                mHUD->AddElement( mBaseHUD );

                vectorScene->GetCell()->ResetHUD();
            }

            notificationFlags = LoadVector( vectorScene );

            FOdysseyVectorEngine::Notify( vectorScene, notificationFlags );
        }
    }
}

bool
UOdysseyPainterEditorVectorBaseTool::OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FKeyEvent& InKeyEvent
                                                          , uint64& oSignalFlags )
{
    oSignalFlags = 0;
    return false;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnKeyDownGlobal( const FKeyEvent& InKeyEvent )
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    if( !mediaProvider.HasMedia<FOdysseyMediaVector>() )
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mediaProvider.GetMedias<FOdysseyMediaVector>();
    if( mediaVectors.IsEmpty() )
        return false;

    FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
    uint64 notificationFlags = 0;
    bool handled = OnKeyDownGlobalVector(vectorScene, InKeyEvent, notificationFlags);
    FOdysseyVectorEngine::Notify( vectorScene, notificationFlags );
    return handled;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
                                                    , const FKey& iKey
                                                    , uint64& oSignalFlags )
{
    if( iKey == EKeys::Delete )
    {
        Delete();
        // force redraw
        iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

        oSignalFlags = 0;

        return true;
    }

//    if( iKey == EKeys::Add )
//    {
//        IncreaseContourWidth();
//
//        return true;
//    }
//
//    if( iKey == EKeys::Subtract )
//    {
//        DecreaseContourWidth();
//
//        return true;
//    }

    return false;
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

    if( !mediaProvider.HasMedia<FOdysseyMediaVector>() )
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mediaProvider.GetMedias<FOdysseyMediaVector>();
    if( mediaVectors.IsEmpty() )
        return false;

    FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
    uint64 notificationFlags = 0;

    bool handled = OnKeyDownVector( vectorScene, iKey, notificationFlags );
    FOdysseyVectorEngine::Notify( vectorScene, notificationFlags );
    return handled;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FKeyEvent& InKeyEvent
                                                        , uint64& oSignalFlags )
{
    oSignalFlags = 0;
    return false;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnKeyUpGlobal( const FKeyEvent& InKeyEvent )
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    if( !mediaProvider.HasMedia<FOdysseyMediaVector>() )
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mediaProvider.GetMedias<FOdysseyMediaVector>();
    if( mediaVectors.IsEmpty() )
        return false;

    FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
    uint64 notificationFlags = 0;
    bool handled = OnKeyUpGlobalVector(vectorScene, InKeyEvent, notificationFlags);
    FOdysseyVectorEngine::Notify( vectorScene, notificationFlags );
    return handled;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnKeyUpVector( FOdysseyVectorGroupPaint* iScene
                                                  , const FKey& iKey
                                                  , uint64& oSignalFlags )
{
    oSignalFlags = 0;
    return false;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnKeyUp( const FKey& iKey )
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    if( !mediaProvider.HasMedia<FOdysseyMediaVector>() )
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mediaProvider.GetMedias<FOdysseyMediaVector>();
    if( mediaVectors.IsEmpty() )
        return false;

    FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
    uint64 notificationFlags = 0;

    bool handled = OnKeyUpVector(vectorScene,iKey, notificationFlags);
    FOdysseyVectorEngine::Notify( vectorScene, notificationFlags );
    return handled;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnMouseDown( const FOdysseyPoint& iPointInTexture
                                                , const FKey& iKey )
{
    if( bMouseEventViaHUD )
    {
        // do nothing
    }
    else
    {
        return OnMouseDownViaHUD( iPointInTexture, iKey );
    }

    return false;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnMouseDownViaHUD( const FOdysseyPoint& iPointInTexture
                                                      , const FKey& iKey )
{
    // workaround for buggy stylus drivers
    if( FilterMouseEvent( eMouseEventName::MouseDown ) == false )
        return false;

    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    mDragging = false;

    if( !mediaProvider.HasMedia<FOdysseyMediaVector>() )
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mAutoCreateMedia ? mediaProvider.GetOrCreateMedias<FOdysseyMediaVector>()
                                                                            : mediaProvider.GetMedias<FOdysseyMediaVector>();
    if( mediaVectors.IsEmpty() )
        return false;

    FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
    uint64 notificationFlags = 0;

    bool handled = OnMouseDownVector( vectorScene, iPointInTexture, iKey, notificationFlags );
    FOdysseyVectorEngine::Notify( vectorScene, notificationFlags );
    return handled;
}

void
UOdysseyPainterEditorVectorBaseTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    if( bMouseEventViaHUD )
    {
        // do nothing
    }
    else
    {
        return OnMouseHoverViaHUD( iPointInTexture );
    }
}

void
UOdysseyPainterEditorVectorBaseTool::OnMouseHoverViaHUD( const FOdysseyPoint& iPointInTexture )
{
    // workaround for buggy stylus drivers
    if( FilterMouseEvent( eMouseEventName::MouseHover ) == false )
        return;

    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;

    // we need the focus on the viewport for keyboard
    //FSlateApplication::Get().SetKeyboardFocus( mViewportWidget );

    if( !mediaProvider.HasMedia<FOdysseyMediaVector>() )
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mediaProvider.GetMedias<FOdysseyMediaVector>();
    if( mediaVectors.IsEmpty() )
        return;

    FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
    uint64 notificationFlags = 0;

    OnMouseHoverVector( vectorScene, iPointInTexture, notificationFlags );

    if( notificationFlags )
    {
        FOdysseyVectorEngine::Notify( vectorScene, notificationFlags );
    }
}

TSharedPtr<SOdysseyViewport>
UOdysseyPainterEditorVectorBaseTool::GetViewport()
{
    return mViewport.Pin();
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
    if( bMouseEventViaHUD )
    {
        // do nothing
    }
    else
    {
        OnMouseDragViaHUD( iPointInTexture );
    }
}

void
UOdysseyPainterEditorVectorBaseTool::OnMouseDragViaHUD( const FOdysseyPoint& iPointInTexture )
{
    // workaround for buggy stylus drivers
    if( FilterMouseEvent( eMouseEventName::MouseDrag ) == false )
        return;

    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;;

    mDragging = true;

    if( !mediaProvider.HasMedia<FOdysseyMediaVector>() )
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mediaProvider.GetMedias<FOdysseyMediaVector>();
    if( mediaVectors.IsEmpty())
        return;

    FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
    uint64 notificationFlags = 0;

    OnMouseDragVector( vectorScene, iPointInTexture, notificationFlags );

    if( notificationFlags )
    {
        FOdysseyVectorEngine::Notify( vectorScene, notificationFlags );
    }
}

bool
UOdysseyPainterEditorVectorBaseTool::OnMouseClick( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    if( bMouseEventViaHUD )
    {
        // do nothing
    }
    else
    {
        return OnMouseClickViaHUD( iPointInTexture, iKey );
    }

    return false;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnMouseClickViaHUD( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    if( !mediaProvider.HasMedia<FOdysseyMediaVector>() )
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mediaProvider.GetMedias<FOdysseyMediaVector>();
    if( mediaVectors.IsEmpty() )
        return false;

    FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();

    uint64 notificationFlags = 0;

    bool handled = OnMouseClickVector( vectorScene, iPointInTexture, iKey, notificationFlags );
    FOdysseyVectorEngine::Notify( vectorScene, notificationFlags );

    if (!handled)
    {
        if( iKey == EKeys::RightMouseButton )
        {
            if( mHasContextMenu )
            {
                PopupContextMenu();
                return true;
            }
        }
    }

    return handled;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnMouseClickVector( FOdysseyVectorGroupPaint* iScene
                                                       , const FOdysseyPoint& iPointInTexture
                                                       , const FKey& iKey
                                                       , uint64& oSignalFlags )
{
    return false;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnMouseUp( const FOdysseyPoint& iPointInTexture
                                              , const FKey& iKey )
{
    if( bMouseEventViaHUD )
    {
        // do nothing
    }
    else
    {
        return OnMouseUpViaHUD( iPointInTexture, iKey );
    }

    return false;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnMouseUpViaHUD( const FOdysseyPoint& iPointInTexture
                                                    , const FKey& iKey )
{
    // workaround for buggy stylus drivers
    if( FilterMouseEvent( eMouseEventName::MouseUp ) == false )
        return false;

    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    mDragging = false;

    if( !mediaProvider.HasMedia<FOdysseyMediaVector>() )
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mediaProvider.GetMedias<FOdysseyMediaVector>();
    if( mediaVectors.IsEmpty() )
        return false;

    FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
    uint64 notificationFlags = 0;

    bool handled = OnMouseUpVector( vectorScene, iPointInTexture, iKey, notificationFlags );
    FOdysseyVectorEngine::Notify( vectorScene, notificationFlags );
    return handled;
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

void
UOdysseyPainterEditorVectorBaseTool::SetVectorEditionFlags( uint64 iViewMode )
{
    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mEditor->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

    mEditor->SetVectorHUDFlags( ( mEditor->GetVectorHUDFlags() & (~FOdysseyVectorHUD::HUD_MODE_ALL) ) | iViewMode );

    mEditor->SanitizeCurrentTool();

    if( mediaVectors.Num() )
    {
        FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();

        // select a object with an inbetweener tag if none are selected
        if( iViewMode & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
        {
            FOdysseyVectorTag* selectedTag = vectorScene->GetLayer()->GetSelectedTagByClassType( FOdysseyVectorTagInbetweener::StaticClass() );

            if ( selectedTag == nullptr )
            {
                FOdysseyVectorTag* lastTag = nullptr;

                for( FOdysseyVectorTag* tag : vectorScene->GetLayer()->GetSharedTagList() )
                {
                    if( tag->GetClass() == FOdysseyVectorTagInbetweener::StaticClass() )
                    {
                        lastTag = tag;
                    }
                }

                if( lastTag )
                {
                    lastTag->GetOwner()->GetCell()->SelectObject( lastTag->GetOwner() );
                }
            }
        }

        FOdysseyVectorEngine::Notify( vectorScene, FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                                                 | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                                                 | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                                                 | FOdysseyPainterEditor::UI_UPDATE_HUD );

        vectorScene->GetLayer()->RequestRedraw( vectorScene->GetCell(), 0 );
    }
}

void
UOdysseyPainterEditorVectorBaseTool::ExtendToolbar( FToolBarBuilder& iBuilder )
{
    Super::ExtendToolbar(iBuilder);

    iBuilder.BeginSection( NAME_None );

    uint64 HUDFlags = mEditor->GetVectorHUDFlags();

    if( HUDFlags & FOdysseyVectorHUD::HUD_MODE_OBJECT_ALLOWED )
    {
        FButtonArgs objectModeButtonArgs;
        objectModeButtonArgs.ToolTipOverride = LOCTEXT("vector-edition-mode.object-mode.tooltip", "Object Mode");
        objectModeButtonArgs.ExtensionHook = "VectorObjectMode";
        objectModeButtonArgs.IconOverride = FSlateIcon("OdysseyStyle", "PainterEditor.TopBar.VectorModeObject32");
        objectModeButtonArgs.UserInterfaceActionType = EUserInterfaceActionType::RadioButton;
        objectModeButtonArgs.Action = FUIAction(
            FExecuteAction::CreateLambda(
                [this]()
                {
                    SetVectorEditionFlags(static_cast<uint64>(FOdysseyVectorHUD::HUD_MODE_OBJECT));
                }
            ),
            FCanExecuteAction(),
            FIsActionChecked::CreateLambda(
                [this]() -> bool
                {
                    return mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_OBJECT;
                }
            )
        );
        iBuilder.AddToolBarButton(objectModeButtonArgs);
    }

    if( HUDFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX_ALLOWED)
    {
        FButtonArgs vertexModeButtonArgs;
        vertexModeButtonArgs.ToolTipOverride = LOCTEXT("vector-edition-mode.vertex-mode.tooltip", "Vertex Mode");
        vertexModeButtonArgs.ExtensionHook = "VectorVertexMode";
        vertexModeButtonArgs.IconOverride = FSlateIcon("OdysseyStyle", "PainterEditor.TopBar.VectorModeVertex32");
        vertexModeButtonArgs.UserInterfaceActionType = EUserInterfaceActionType::RadioButton;
        vertexModeButtonArgs.Action = FUIAction(
            FExecuteAction::CreateLambda(
                [this]()
                {
                    SetVectorEditionFlags(static_cast<uint64>(FOdysseyVectorHUD::HUD_MODE_VERTEX));
                }
            ),
            FCanExecuteAction(),
            FIsActionChecked::CreateLambda(
                [this]() -> bool
                {
                    return mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX;
                }
            )
        );
        iBuilder.AddToolBarButton(vertexModeButtonArgs);
    }

    if( HUDFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN_ALLOWED)
    {
        FButtonArgs inbetweenModeButtonArgs;
        inbetweenModeButtonArgs.ToolTipOverride = LOCTEXT("vector-edition-mode.inbetween-mode.tooltip", "Inbetween Mode");
        inbetweenModeButtonArgs.ExtensionHook = "VectorVertexMode";
        inbetweenModeButtonArgs.IconOverride = FSlateIcon("OdysseyStyle", "PainterEditor.TopBar.VectorModeInbetween32");
        inbetweenModeButtonArgs.UserInterfaceActionType = EUserInterfaceActionType::RadioButton;
        inbetweenModeButtonArgs.Action = FUIAction(
            FExecuteAction::CreateLambda(
                [this]()
                {
                    SetVectorEditionFlags(static_cast<uint64>(FOdysseyVectorHUD::HUD_MODE_INBETWEEN));
                }
            ),
            FCanExecuteAction(),
            FIsActionChecked::CreateLambda(
                [this]() -> bool
                {
                    return mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_INBETWEEN;
                }
            )
        );
        iBuilder.AddToolBarButton(inbetweenModeButtonArgs);
    }

    iBuilder.EndSection();
}

bool
UOdysseyPainterEditorVectorBaseTool::SupportsColorType(EOdysseyPainterEditorColorType iType)
{
    return iType == EOdysseyPainterEditorColorType::Raw || iType == EOdysseyPainterEditorColorType::Indexed;
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

    menu.BeginSection("Context Menu");

    ExtendContextMenu( menu );

    menu.EndSection();

    return menu.MakeWidget();
}

void
UOdysseyPainterEditorVectorBaseTool::ExtendContextMenu( FMenuBuilder& menu )
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

            if( GetEditor()->GetVectorHUDFlags() &  FOdysseyVectorHUD::HUD_MODE_OBJECT )
            {
                ExtendContextMenuObject( vectorScene, menu, 0 );
            }

            if( GetEditor()->GetVectorHUDFlags() &  FOdysseyVectorHUD::HUD_MODE_VERTEX )
            {
                ExtendContextMenuVertex( vectorScene, menu, 0 );
            }

            if( GetEditor()->GetVectorHUDFlags() &  FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
            {
                ExtendContextMenuInbetween( vectorScene, menu, 0 );
            }
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::ExtendContextMenuObject( FOdysseyVectorGroupPaint* iScene
                                                            , FMenuBuilder& menu
                                                            , uint64 iObjectMenuFlags )
{

    menu.AddMenuEntry(
          LOCTEXT("vector-tool.object-context-menu.reset-view.name", "Reset View")
        , LOCTEXT("vector-tool.object-context-menu.reset-view.tooltip", "Reset View")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::ResetView, GetEditor(), iScene )));
    menu.AddMenuEntry(
            LOCTEXT("vector-tool.object-context-menu.group-paint.name", "Make Paint Group")
        , LOCTEXT("vector-tool.object-context-menu.group-paint.tooltip", "Make Paint Group")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::MakePaintGroup, GetEditor(), iScene )));
    menu.AddMenuEntry(
          LOCTEXT("vector-tool.object-context-menu.group.name", "Group")
        , LOCTEXT("vector-tool.object-context-menu.group.tooltip", "Group")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::Group, GetEditor(), iScene )));
    menu.AddMenuEntry(
          LOCTEXT("vector-tool.object-context-menu.ungroup.name", "Ungroup")
        , LOCTEXT("vector-tool.object-context-menu.ungroup.tooltip", "Ungroup")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::Ungroup, GetEditor(), iScene )));
    menu.AddMenuEntry(
          LOCTEXT("vector-tool.object-context-menu.bring-forward.name", "Bring forward")
        , LOCTEXT("vector-tool.object-context-menu.bring-forward.tooltip", "Bring forward")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::BringForward, GetEditor(), iScene )));
    menu.AddMenuEntry(
          LOCTEXT("vector-tool.object-context-menu.send-backward.name", "Send backward")
        , LOCTEXT("vector-tool.object-context-menu.send-backward.tooltip", "Send backward")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::SendBackward, GetEditor(), iScene )));
    menu.AddMenuEntry(
          LOCTEXT("vector-tool.object-context-menu.delete-selection.name","Delete Selection")
        , LOCTEXT("vector-tool.object-context-menu.delete-selection.tooltip","Delete Selection")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::DeleteObjects, GetEditor(), iScene )));
    menu.AddMenuEntry(
          LOCTEXT("vector-tool.object-context-menu.flip-horizontal.name","Flip Horizontal")
        , LOCTEXT("vector-tool.object-context-menu.flip-horizontal.tooltip","Flip Horizontal")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::FlipHorizontal, GetEditor(), iScene )));
    menu.AddMenuEntry(
          LOCTEXT("vector-tool.object-context-menu.flip-vertical.name","Flip Vertical")
        , LOCTEXT("vector-tool.object-context-menu.flip-vertical.tooltip","Flip Vertical")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::FlipVertical, GetEditor(), iScene )));
    menu.AddMenuEntry(
          LOCTEXT("vector-tool.object-context-menu.clear-coloring.name", "Clear Coloring")
        , LOCTEXT("vector-tool.object-context-menu.clear-coloring.tooltip", "Clear Coloring")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::ClearColoring, GetEditor(), iScene )));
    menu.AddMenuEntry(
          LOCTEXT("vector-tool.object-context-menu.apply-transformations.name", "Apply Transformations")
        , LOCTEXT("vector-tool.object-context-menu.apply-transformations.tooltip", "Apply Transformations")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::ApplyTransformations, GetEditor(), iScene )));
    //menu.AddMenuEntry(
    //    LOCTEXT("vector-tool.object-context-menu.apply-transformations.name", "Make DemoBrush")
    //    , LOCTEXT("vector-tool.object-context-menu.apply-transformations.tooltip", "Make DemoBrush")
    //    , FSlateIcon()
    //    , FUIAction(FExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::MakeDemoBrush, iScene )));
//    }
//    menu.EndSection();
}

// for testing purpose.
void
UOdysseyPainterEditorVectorBaseTool::MakeDemoBrush( FOdysseyVectorGroupPaint* iScene )
{
   std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetCell()->GetSelectedObjectList();
/*
   FOdysseyVectorBrush::MakeDemoBrush( selectedObjectList
                                     , FOdysseyVectorObject::GetBoundingBoxFromList( selectedObjectList ) );
*/
}

void
UOdysseyPainterEditorVectorBaseTool::ExtendContextMenuVertex( FOdysseyVectorGroupPaint* iScene
                                                            , FMenuBuilder& menu
                                                            , uint64 iVertexMenuFlags )
{
// Commented-out: sections are not needed here as they would conflict with the section
// just created by the Edit Menu when this tool's menu appears in the Edit Menu
// See FOdysseyPainterEditor::AddEditMenuEntry() for details
//     menu.BeginSection("Context");
//     {
    menu.AddMenuEntry(
          LOCTEXT("vector-tool.vertex-context-menu.delete-selection.name", "Subdivide segments")
        , LOCTEXT("vector-tool.vertex-context-menu.delete-selection.tooltip", "Subdivide segments")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::Subdivide, GetEditor(), iScene )));
    menu.AddMenuEntry(
          LOCTEXT("vector-tool.vertex-context-menu.delete-selection.name", "Delete Selection")
        , LOCTEXT("vector-tool.vertex-context-menu.delete-selection.tooltip", "Delete Selection")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::DeletePointSelection, GetEditor(), iScene )));
    menu.AddMenuEntry(
          LOCTEXT("vector-tool.vertex-context-menu.align-point-selection.name", "Align Point Selection")
        , LOCTEXT("vector-tool.vertex-context-menu.align-point-selection.tooltip", "Align Point Selection")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::AlignPointSelection, GetEditor(), iScene )));
    menu.AddMenuEntry(
          LOCTEXT("vector-tool.vertex-context-menu.unalign-point-selection.name", "Unalign Point Selection")
        , LOCTEXT("vector-tool.vertex-context-menu.unalign-point-selection.tooltip", "Unalign Point Selection")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::UnalignPointSelection, GetEditor(), iScene )));
    menu.AddMenuEntry(
          LOCTEXT("vector-tool.vertex-context-menu.lock-point-selection.name", "Lock Point Selection")
        , LOCTEXT("vector-tool.vertex-context-menu.lock-point-selection.tooltip", "Lock Point Selection")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::LockPointSelection, GetEditor(), iScene )));
    menu.AddMenuEntry(
          LOCTEXT("vector-tool.vertex-context-menu.unlock-point-selection.name", "Unlock Point Selection")
        , LOCTEXT("vector-tool.vertex-context-menu.unlock-point-selection.tooltip", "Unlock Point Selection")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::UnlockPointSelection, GetEditor(), iScene )));
//    }
//    menu.EndSection();
}

void
UOdysseyPainterEditorVectorBaseTool::ExtendContextMenuInbetween( FOdysseyVectorGroupPaint* iScene
                                                               , FMenuBuilder& menu
                                                               , uint64 iInbetweenMenuFlags )
{
// Commented-out: sections are not needed here as they would conflict with the section
// just created by the Edit Menu when this tool's menu appears in the Edit Menu
// See FOdysseyPainterEditor::AddEditMenuEntry() for details
//     menu.BeginSection("Context");
//     {
    if( iScene->GetCell()->GetSelectedObjectList().size() > 1 )
    {
        menu.AddMenuEntry(
                  LOCTEXT("vector-tool.inbetween-context-menu.groupadd-inbetweener-tag.name", "Group and Add Inbetweener Grid")
                , LOCTEXT("vector-tool.inbetween-context-menu.groupadd-inbetweener-tag.tooltip", "Group and Add Inbetweener Grid")
                , FSlateIcon()
                , FUIAction( FExecuteAction::CreateStatic( &FOdysseyPainterEditor::GroupAndAddInbetweenerTag, GetEditor(), iScene )
                           , FCanExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::CanAddTag, iScene ) ) );
    }

    menu.AddMenuEntry(
              LOCTEXT("vector-tool.inbetween-context-menu.add-inbetweener-tag.name", "Add Inbetweener Tag")
            , LOCTEXT("vector-tool.inbetween-context-menu.add-inbetweener-tag.tooltip", "Add Inbetweener Tag")
            , FSlateIcon()
            , FUIAction( FExecuteAction::CreateStatic( &FOdysseyPainterEditor::AddInbetweenerTag, GetEditor(), iScene )
                       , FCanExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::CanAddTag, iScene )));

    menu.AddMenuEntry(
              LOCTEXT("vector-tool.inbetween-context-menu.remove-inbetweener-tag.name", "Remove Inbetweener Tag")
            , LOCTEXT("vector-tool.inbetween-context-menu.remove-inbetweener-tag.tooltip", "Remove Inbetweener Tag")
            , FSlateIcon()
            , FUIAction( FExecuteAction::CreateStatic( &FOdysseyPainterEditor::RemoveInbetweenerTag, GetEditor(), iScene )
                       , FCanExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::CanAlterTag, iScene )));

    menu.AddMenuEntry(
            LOCTEXT("vector-tool.inbetween-context-menu.commit-inbetweener-tag.name", "Commit Inbetweener Tag")
            , LOCTEXT("vector-tool.inbetween-context-menu.commit-inbetweener-tag.tooltip", "Commit Inbetweener Tag")
            , FSlateIcon()
            , FUIAction( FExecuteAction::CreateStatic( &FOdysseyPainterEditor::CommitSelectedInbetweenerTag, GetEditor(), iScene->GetLayer() )
                       , FCanExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::CanAlterTag, iScene )));

    menu.AddSubMenu(
            LOCTEXT("vector-tool.inbetween-context-menu.reset-grid.name", "Reset Grid")
        , LOCTEXT("vector-tool.inbetween-context-menu.reset-grid.tooltip", "Reset Grid")
        , FNewMenuDelegate::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::ResetGridMenu, iScene ) );

    menu.AddSubMenu(
            LOCTEXT("vector-tool.inbetween-context-menu.reset-spacing.name", "Reset Spacing")
        , LOCTEXT("vector-tool.inbetween-context-menu.reset-spacing.tooltip", "Reset Spacing")
        , FNewMenuDelegate::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::ResetSpacingMenu, iScene ) );

    menu.AddSubMenu(
            LOCTEXT("vector-tool.inbetween-context-menu.copy-spacing.name", "Copy Spacing")
        , LOCTEXT("vector-tool.inbetween-context-menu.copy-spacing.tooltip", "Copy Spacing")
        , FNewMenuDelegate::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::CopySpacingMenu, iScene ) );

    menu.AddSubMenu(
            LOCTEXT("vector-tool.inbetween-context-menu.paste-spacing.name", "Paste Spacing")
        , LOCTEXT("vector-tool.inbetween-context-menu.paste-spacing.tooltip", "Paste Spacing")
        , FNewMenuDelegate::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::PasteSpacingMenu, iScene ) );

/*
    menu.AddMenuEntry(
            LOCTEXT("vector-tool.inbetween-context-menu.paste-spacing-chart.name", "Paste Spacing Chart")
        , LOCTEXT("vector-tool.inbetween-context-menu.paste-spacing-chart.tooltip", "Paste Spacing Chart")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::PasteSpacingChart, GetEditor(), iScene->GetLayer() )));
*/
    menu.AddMenuEntry(
            LOCTEXT("vector-tool.object-context-menu.delete-selection.name","Delete Selection")
        , LOCTEXT("vector-tool.object-context-menu.delete-selection.tooltip","Delete Selection")
        , FSlateIcon()
        , FUIAction(FExecuteAction::CreateStatic( &FOdysseyPainterEditor::DeleteObjects, GetEditor(), iScene )));


//    }
//    menu.EndSection();
}

bool
UOdysseyPainterEditorVectorBaseTool::CanAddTag( FOdysseyVectorGroupPaint* iScene )
{
    bool ret = false;

    for( FOdysseyVectorObject* selectedObject : iScene->GetCell()->GetSelectedObjectList() )
    {
        if( selectedObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() ) )
        {
            return false;
        }

        ret = true;
    }

    return ret;
}

bool
UOdysseyPainterEditorVectorBaseTool::CanAlterTag( FOdysseyVectorGroupPaint* iScene )
{
    std::list<FOdysseyVectorTag*> selectedTagList;

    iScene->GetLayer()->GetSelectedTagByClassType( FOdysseyVectorTagInbetweener::StaticClass(), selectedTagList );

    return selectedTagList.size() ? true : false;
}

void
UOdysseyPainterEditorVectorBaseTool::ResetGridMenu( FMenuBuilder& menu
                                                  , FOdysseyVectorGroupPaint* iScene )
{
    menu.AddMenuEntry(
          LOCTEXT("vector-tool.inbetween-context-menu.reset-grid-transformation.name", "Transformation")
        , LOCTEXT("vector-tool.inbetween-context-menu.reset-grid-transformation.tooltip", "Transformation")
        , FSlateIcon()
        , FUIAction( FExecuteAction::CreateStatic( &FOdysseyPainterEditor::ResetInbetweenerGrid, GetEditor(), iScene, true, false )
                   , FCanExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::CanAlterTag, iScene )));

    menu.AddMenuEntry(
          LOCTEXT("vector-tool.inbetween-context-menu.reset-grid-deformation.name", "Deformation")
        , LOCTEXT("vector-tool.inbetween-context-menu.reset-grid-deformation.tooltip", "Deformation")
        , FSlateIcon()
        , FUIAction( FExecuteAction::CreateStatic( &FOdysseyPainterEditor::ResetInbetweenerGrid, GetEditor(), iScene, false, true )
                   , FCanExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::CanAlterTag, iScene )));

    menu.AddMenuEntry(
          LOCTEXT("vector-tool.inbetween-context-menu.reset-grid-both.name", "Both")
        , LOCTEXT("vector-tool.inbetween-context-menu.reset-grid-both.tooltip", "Both")
        , FSlateIcon()
        , FUIAction( FExecuteAction::CreateStatic( &FOdysseyPainterEditor::ResetInbetweenerGrid, GetEditor(), iScene, true, true )
                   , FCanExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::CanAlterTag, iScene )));
}

void
UOdysseyPainterEditorVectorBaseTool::ResetSpacingMenu( FMenuBuilder& menu
                                                     , FOdysseyVectorGroupPaint* iScene )
{
    menu.AddMenuEntry(
          LOCTEXT("vector-tool.inbetween-context-menu.reset-spacing-current-breakdown.name", "Current Breakdown")
        , LOCTEXT("vector-tool.inbetween-context-menu.reset-spacing-current-breakdown.tooltip", "Current Breakdown")
        , FSlateIcon()
        , FUIAction( FExecuteAction::CreateStatic( &FOdysseyPainterEditor::ResetSpacingChart, GetEditor(), iScene, false, true )
                   , FCanExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::CanAlterTag, iScene )));

    menu.AddMenuEntry(
          LOCTEXT("vector-tool.inbetween-context-menu.reset-spacing-all-breakdowns.name", "All Breakdonws")
        , LOCTEXT("vector-tool.inbetween-context-menu.reset-spacing-all-breakdowns.tooltip", "All Breakdonws")
        , FSlateIcon()
        , FUIAction( FExecuteAction::CreateStatic( &FOdysseyPainterEditor::ResetSpacingChart, GetEditor(), iScene, false, false )
                   , FCanExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::CanAlterTag, iScene )));
}

void
UOdysseyPainterEditorVectorBaseTool::CopySpacingMenu( FMenuBuilder& menu
                                                    , FOdysseyVectorGroupPaint* iScene )
{
    menu.AddMenuEntry(
          LOCTEXT("vector-tool.inbetween-context-menu.copy-spacing-current-breakdown.name", "Current Breakdown")
        , LOCTEXT("vector-tool.inbetween-context-menu.copy-spacing-current-breakdown.tooltip", "Current Breakdown")
        , FSlateIcon()
        , FUIAction( FExecuteAction::CreateStatic( &FOdysseyPainterEditor::CopySpacingChart, GetEditor(), iScene, true )
                   , FCanExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::CanAlterTag, iScene )));

    menu.AddMenuEntry(
          LOCTEXT("vector-tool.inbetween-context-menu.copy-spacing-all-breakdowns.name", "All Breakdonws")
        , LOCTEXT("vector-tool.inbetween-context-menu.copy-spacing-all-breakdowns.tooltip", "All Breakdonws")
        , FSlateIcon()
        , FUIAction( FExecuteAction::CreateStatic( &FOdysseyPainterEditor::CopySpacingChart, GetEditor(), iScene, false )
                   , FCanExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::CanAlterTag, iScene )));
}

void
UOdysseyPainterEditorVectorBaseTool::PasteSpacingMenu( FMenuBuilder& menu
                                                     , FOdysseyVectorGroupPaint* iScene )
{
    menu.AddMenuEntry(
          LOCTEXT("vector-tool.inbetween-context-menu.paste-spacing-current-breakdown.name", "Current Breakdown")
        , LOCTEXT("vector-tool.inbetween-context-menu.paste-spacing-current-breakdown.tooltip", "Current Breakdown")
        , FSlateIcon()
        , FUIAction( FExecuteAction::CreateStatic( &FOdysseyPainterEditor::PasteSpacingChart, GetEditor(), iScene, true )
                   , FCanExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::CanAlterTag, iScene )));

    menu.AddMenuEntry(
          LOCTEXT("vector-tool.inbetween-context-menu.paste-spacing-all-breakdowns.name", "All Breakdonws")
        , LOCTEXT("vector-tool.inbetween-context-menu.paste-spacing-all-breakdowns.tooltip", "All Breakdonws")
        , FSlateIcon()
        , FUIAction( FExecuteAction::CreateStatic( &FOdysseyPainterEditor::PasteSpacingChart, GetEditor(), iScene, false )
                   , FCanExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::CanAlterTag, iScene )));
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
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
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

// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"

#include "PropertyHandle.h"
#include "ToolMenu.h"

#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditTool.h"
//#include "Widgets/Tools/SOdysseyPainterEditorVectorEditionMode.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Framework/Commands/GenericCommands.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorCommands.h"
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
#include "Undo/OdysseyVectorUndo.h"
#include "FOdysseySceneViewport.h"
#include <chrono>

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorBaseTool::~UOdysseyPainterEditorVectorBaseTool()
{
    // commented-out: FOdysseyUndoDelegates cannot be used because its delegates are cleared after the first call
    //FOdysseyUndoDelegates::Get().OnAfterUndoRedo().RemoveAll( this );
    FOdysseyVectorUndo::OnPostUndoRedoDelegate().RemoveAll( this );
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
    , mWorkingCell( nullptr )
    , mWorkingLayer( nullptr )
{
    // 3D HUD
    mHUD->AddElement( mBaseHUD );
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

            colorMode = eBucketColorMode::Palette;
        }
        break;

        default:
        break;
    }

    iPath->GetForegroundBucket().SetColorMode( colorMode );
}

uint32
UOdysseyPainterEditorVectorBaseTool::GetViewportWidth()
{
    return mViewport.Pin()->GetViewport().Get()->GetSize().X;
}

uint32
UOdysseyPainterEditorVectorBaseTool::GetViewportHeight()
{
    return mViewport.Pin()->GetViewport().Get()->GetSize().Y;
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

FOdysseyVectorCell*
UOdysseyPainterEditorVectorBaseTool::GetWorkingCell()
{
    return mWorkingCell;
}

FOdysseyVectorGroup*
UOdysseyPainterEditorVectorBaseTool::GetWorkingGroup() const
{
    if( mWorkingCell )
    {
        FOdysseyVectorGroupPaint* scene = mWorkingCell->GetScene();
        FOdysseyVectorGroup* workingGroup = scene;

        // Add the path to the current unique selected group
        if( mWorkingCell->GetSelectedObjectList().size() == 1 )
        {
            FOdysseyVectorObject* selectedObject = mWorkingCell->GetLastSelectedObject();

            workingGroup = static_cast<FOdysseyVectorGroup*>( selectedObject->GetAncestorByClass( FOdysseyVectorGroup::StaticClass()
                                                                                                , true
                                                                                                , true ) );
        }

        return workingGroup;
    }

    return nullptr;
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
UOdysseyPainterEditorVectorBaseTool::UnbindLayerDelegates()
{
    mWorkingLayer->OnUpdateDelegate().RemoveAll( this );
}

void
UOdysseyPainterEditorVectorBaseTool::BindLayerDelegates()
{
    mWorkingLayer->OnUpdateDelegate().AddUObject( this, &UOdysseyPainterEditorVectorBaseTool::OnVectorLayerUpdate );
}

void
UOdysseyPainterEditorVectorBaseTool::Unload()
{
    UOdysseyPainterEditorTool::Unload();

    // commented-out: FOdysseyUndoDelegates cannot be used because its delegates are cleared after the first call
    //FOdysseyUndoDelegates::Get().OnAfterUndoRedo().RemoveAll( this );
    FOdysseyVectorUndo::OnPostUndoRedoDelegate().RemoveAll( this );

    if( mWorkingCell )
    {
        UnloadVector( mWorkingCell->GetScene() );

        /**
            * ERIC PATCH
            * See LoadVector() comment for explanations
            */
        mVectorBlock = nullptr;
        //END PATCH
    }

    if( mWorkingLayer )
    {
        UnbindLayerDelegates();

        if( mBaseHUD )
        {
            mBaseHUD->Unload();
            // 2D HUD
            mWorkingLayer->RemoveHUD( mBaseHUD.Get() );
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::Load()
{
    TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = GetEditor()->FindTab<FOdysseyPainterEditorViewportTab>();

    // commented-out: FOdysseyUndoDelegates cannot be used because its delegates are cleared after the first call
    //FOdysseyUndoDelegates::Get().OnAfterUndoRedo().AddUObject( this, &UOdysseyPainterEditorVectorBaseTool::OnUndoRedo );
    FOdysseyVectorUndo::OnPostUndoRedoDelegate().AddUObject( this, &UOdysseyPainterEditorVectorBaseTool::OnUndoRedo );

    UOdysseyPainterEditorTool::Load();

    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    // we need the focus on the viewport for keyboard
    // and some tools need to know the viewport size
    mViewport = viewportTab->GetViewport();

    mPreviousMouseEvent = eMouseEventName::MouseHover;

    UOdysseyPainterEditorTool::Load();

    mWorkingCell = nullptr;
    mWorkingLayer = nullptr;

    if( hasVector )
    {
        // It would be better if this is done in OnMouseDown()
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() > 0 )
        {
            mWorkingCell = mediaVectors[0]->GetScene()->GetCell();
            mWorkingLayer = mWorkingCell->GetLayer();

            BindLayerDelegates();

            mWorkingCell->GetLayer()->ClearHUD();

            if( mBaseHUD )
            {
                mBaseHUD->Load();
                // 2D HUD
                mWorkingCell->GetLayer()->AddHUD( mBaseHUD.Get() );
                mWorkingCell->GetLayer()->ResetHUD( mWorkingCell->GetScene() );
            }

            /**
             * ERIC PATCH
             *
             * Keeps the vectorBlock in memory to avoid reloading it each time we need to redraw
             * This fixes a huge performance issue when using the vector tool.
             */
            mVectorBlock = mWorkingCell->GetScene()->GetCell()->GetCellInterface()->GetBlock();
            //END PATCH

            LoadVector( mWorkingCell->GetScene() );
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::OnVectorLayerUpdate( const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags
                                                        , uint32 iUpdateFlags )
{
    if( ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::OBJECT_SELECTION] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_OBJECT_SELECTION] ) )
    {
        if( mBaseHUD )
        {
            mBaseHUD->Reset();
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::OnUndoRedo()
{
                         // The notification might be called after the cell has been removed from the layer,
                         // then mWorkingCell->GetLayer() woul dbe null and that would likely create crashes
                         // in HUD Reset methods. so we check that.
    if ( mWorkingCell && ( mWorkingCell->GetLayer() == mWorkingLayer ) )
    {
        mWorkingLayer->ResetHUD( mWorkingCell->GetScene() );
    }
}

bool
UOdysseyPainterEditorVectorBaseTool::OnKeyDownGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FKeyEvent& InKeyEvent )
{
    return false;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnKeyDownGlobal( const FKeyEvent& InKeyEvent )
{
    if( mWorkingCell )
    {
        bool handled = OnKeyDownGlobalVector ( mWorkingCell->GetScene(), InKeyEvent );

        // we always return false because other tools might need the signal
        //return handled;
    }

    return false;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
                                                    , const FKey& iKey )
{
//    if( iKey == EKeys::Delete )
//    {
//        ActionDelete();
//        // force redraw
//        iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

//        return true;
//    }

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
    if( mWorkingCell )
    {
        bool handled = OnKeyDownVector( mWorkingCell->GetScene(), iKey );

        return handled;
    }

    return false;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnKeyUpGlobalVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FKeyEvent& InKeyEvent )
{
    return false;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnKeyUpGlobal( const FKeyEvent& InKeyEvent )
{
    if( mWorkingCell )
    {
        bool handled = OnKeyUpGlobalVector( mWorkingCell->GetScene(), InKeyEvent );

        // we always return false because other tools might need the signal
        //return handled;
    }

    return false;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnKeyUpVector( FOdysseyVectorGroupPaint* iScene
                                                  , const FKey& iKey )
{
    return false;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnKeyUp( const FKey& iKey )
{
    if( mWorkingCell )
    {
        bool handled = OnKeyUpVector( mWorkingCell->GetScene(), iKey );

        return handled;
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

    UOdysseyPainterEditorTool::OnMouseDown(iPointInTexture, iKey);

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
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    // workaround for buggy stylus drivers
    //if( FilterMouseEvent( eMouseEventName::MouseDown ) == false )
        //return false;

    mDragging = false;

    if( mAutoCreateMedia )
    {
        mediaProvider.GetOrCreateMedias<FOdysseyMediaVector>();
    }

    // Load() will be called automatically if media is created, initializing mworkingScene

    if( mWorkingCell )
    {
        bool handled = OnMouseDownVector( mWorkingCell->GetScene(), iPointInTexture, iKey );

        return handled;
    }

    return false;
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
    //if( FilterMouseEvent( eMouseEventName::MouseHover ) == false )
    //    return;

    // we need the focus on the viewport for keyboard
    //FSlateApplication::Get().SetKeyboardFocus( mViewportWidget );

    if( mWorkingCell )
    {
        OnMouseHoverVector( mWorkingCell->GetScene(), iPointInTexture );
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
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;

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
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;

    // workaround for buggy stylus drivers
    //if( FilterMouseEvent( eMouseEventName::MouseDrag ) == false )
    //    return;

    mDragging = true;

    if( mWorkingCell )
    {
        OnMouseDragVector( mWorkingCell->GetScene(), iPointInTexture );
    }
}

bool
UOdysseyPainterEditorVectorBaseTool::OnMouseClick( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

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

    if( mWorkingCell )
    {
        bool handled = OnMouseClickVector( mWorkingCell->GetScene(), iPointInTexture, iKey );

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

    return false;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnMouseClickVector( FOdysseyVectorGroupPaint* iScene
                                                       , const FOdysseyPoint& iPointInTexture
                                                       , const FKey& iKey )
{
    return false;
}

bool
UOdysseyPainterEditorVectorBaseTool::OnMouseUp( const FOdysseyPoint& iPointInTexture
                                              , const FKey& iKey )
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

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
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    // workaround for buggy stylus drivers
    //if( FilterMouseEvent( eMouseEventName::MouseUp ) == false )
    //    return false;

    mDragging = false;

    if( mWorkingCell )
    {
        bool handled = OnMouseUpVector( mWorkingCell->GetScene(), iPointInTexture, iKey );

        return handled;
    }

    return false;
}

void
UOdysseyPainterEditorVectorBaseTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FName& iPropertyName )
{
}

void
UOdysseyPainterEditorVectorBaseTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    // redraw
    if ( mWorkingCell )
    {
        PropertyChangedVector( mWorkingCell->GetScene(), PropertyChangedEvent.GetPropertyName() );
    }
}

void
UOdysseyPainterEditorVectorBaseTool::Commit()
{

}

void UOdysseyPainterEditorVectorBaseTool::BindShortcuts(TSharedPtr<FUICommandList> iCommandList)
{
    Super::BindShortcuts(iCommandList);

    iCommandList->MapAction(
        FGenericCommands::Get().SelectAll,
        FExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::SelectAll )
    );

    iCommandList->MapAction(
        FGenericCommands::Get().Delete,
        FExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::ActionDelete )
    );

    iCommandList->MapAction(
        FGenericCommands::Get().Cut,
        FExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::Cut )
    );

    iCommandList->MapAction(
        FGenericCommands::Get().Copy,
        FExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::Copy )
    );

    iCommandList->MapAction(
        FGenericCommands::Get().Paste,
        FExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::Paste )
    );

    // shortcuts available in object mode menu

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().VectorResetView,
        FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorVectorBaseTool::ActionResetView)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().VectorMakePaintGroup,
        FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorVectorBaseTool::ActionMakePaintGroup)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().VectorGroup,
        FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorVectorBaseTool::ActionGroup)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().VectorUngroup
      , FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorVectorBaseTool::ActionUngroup)
      , FCanExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::CanUngroup )
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().VectorBringForward,
        FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorVectorBaseTool::ActionBringForward)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().VectorSendBackward,
        FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorVectorBaseTool::ActionSendBackward)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().VectorFlipHorizontal,
        FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorVectorBaseTool::ActionFlipHorizontal)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().VectorFlipVertical,
        FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorVectorBaseTool::ActionFlipVertical)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().VectorClearColoring
      , FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorVectorBaseTool::ActionClearColoring)
      , FCanExecuteAction::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::CanBePainted)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().VectorApplyTransformations,
        FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorVectorBaseTool::ActionApplyTransformations)
    );

    // shortcuts available in vertex mode menu

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().VectorSubdivideSegments,
        FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorVectorBaseTool::ActionSubdivideSegments)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().VectorAlignPointSelection,
        FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorVectorBaseTool::ActionAlignPointSelection)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().VectorUnalignPointSelection,
        FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorVectorBaseTool::ActionUnalignPointSelection)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().VectorLockPointSelection,
        FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorVectorBaseTool::ActionLockPointSelection)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().VectorUnlockPointSelection,
        FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorVectorBaseTool::ActionUnlockPointSelection)
    );
}

// Actions in object mode

void
UOdysseyPainterEditorVectorBaseTool::ActionResetView()
{
    if( mWorkingCell )
    {
        FOdysseyPainterEditor::ResetView( mEditor, mWorkingCell->GetScene() );
    }
}

void
UOdysseyPainterEditorVectorBaseTool::ActionMakePaintGroup()
{
    if( mWorkingCell )
    {
        FOdysseyPainterEditor::MakePaintGroup( mEditor, mWorkingCell->GetScene() );
    }
}

void
UOdysseyPainterEditorVectorBaseTool::ActionGroup()
{
    if( mWorkingCell )
    {
        FOdysseyPainterEditor::Group( mEditor, mWorkingCell->GetScene() );
    }
}

void
UOdysseyPainterEditorVectorBaseTool::ActionUngroup()
{
    if( mWorkingCell )
    {
        FOdysseyPainterEditor::Ungroup( mEditor, mWorkingCell->GetScene() );
    }
}

void
UOdysseyPainterEditorVectorBaseTool::ActionBringForward()
{
    if( mWorkingCell )
    {
        FOdysseyPainterEditor::BringForward( mEditor, mWorkingCell->GetScene() );
    }
}

void
UOdysseyPainterEditorVectorBaseTool::ActionSendBackward()
{
    if( mWorkingCell )
    {
        FOdysseyPainterEditor::SendBackward( mEditor, mWorkingCell->GetScene() );
    }
}

void
UOdysseyPainterEditorVectorBaseTool::ActionFlipHorizontal()
{
    if( mWorkingCell )
    {
        FOdysseyPainterEditor::FlipHorizontal( mEditor, mWorkingCell->GetScene() );
    }
}

void
UOdysseyPainterEditorVectorBaseTool::ActionFlipVertical()
{
    if( mWorkingCell )
    {
        FOdysseyPainterEditor::FlipVertical( mEditor, mWorkingCell->GetScene() );
    }
}

void
UOdysseyPainterEditorVectorBaseTool::ActionClearColoring()
{
    if( mWorkingCell )
    {
        FOdysseyPainterEditor::ClearColoring( mEditor, mWorkingCell->GetScene() );
    }
}

void
UOdysseyPainterEditorVectorBaseTool::ActionApplyTransformations()
{
    if( mWorkingCell )
    {
        FOdysseyPainterEditor::ApplyTransformations( mEditor, mWorkingCell->GetScene() );
    }
}

void
UOdysseyPainterEditorVectorBaseTool::Copy()
{
    if( mWorkingCell )
    {
        if( GetEditor()->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_OBJECT )
        {
            FOdysseyPainterEditor::CopyObjects( mWorkingCell->GetScene() );
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::Paste()
{
    if( mWorkingCell )
    {
        if( GetEditor()->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_OBJECT )
        {
            FOdysseyPainterEditor::PasteObjects( GetEditor(), mWorkingCell->GetScene() );
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::SelectAll()
{
    if( mWorkingCell )
    {
        if( GetEditor()->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_OBJECT )
        {
            FOdysseyPainterEditor::SelectAllObjects( GetEditor(), mWorkingCell->GetScene() );
        }

        if( GetEditor()->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX )
        {
            FOdysseyPainterEditor::SelectAllPoints( GetEditor(), mWorkingCell->GetScene() );
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::IncreaseContourWidth()
{
    if( mWorkingCell )
    {
        FOdysseyPainterEditor::AlterContourWidth( mWorkingCell->GetScene(), 1.1f, false );
    }
}

void
UOdysseyPainterEditorVectorBaseTool::DecreaseContourWidth()
{
    if( mWorkingCell )
    {
        FOdysseyPainterEditor::AlterContourWidth( mWorkingCell->GetScene(), 0.9f, false );
    }
}

void
UOdysseyPainterEditorVectorBaseTool::Cut()
{
    UOdysseyPainterEditorVectorBaseTool::Copy();
    UOdysseyPainterEditorVectorBaseTool::ActionDelete();
}

void
UOdysseyPainterEditorVectorBaseTool::ActionDelete()
{
    if( mWorkingCell )
    {
        if( GetEditor()->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_OBJECT )
        {
            FOdysseyPainterEditor::DeleteObjects( GetEditor(), mWorkingCell->GetScene() );
        }

        if( GetEditor()->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX )
        {
            FOdysseyPainterEditor::DeletePointSelection( GetEditor(), mWorkingCell->GetScene() );
        }
    }
}

// Actions in vertex mode

void
UOdysseyPainterEditorVectorBaseTool::ActionSubdivideSegments()
{
    if( mWorkingCell )
    {
        if( GetEditor()->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX )
        {
            FOdysseyPainterEditor::Subdivide( mEditor, mWorkingCell->GetScene() );
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::ActionAlignPointSelection()
{
    if( mWorkingCell )
    {
        if( GetEditor()->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX )
        {
            FOdysseyPainterEditor::AlignPointSelection( mEditor, mWorkingCell->GetScene() );
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::ActionUnalignPointSelection()
{
    if( mWorkingCell )
    {
        if( GetEditor()->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX )
        {
            FOdysseyPainterEditor::UnalignPointSelection( mEditor, mWorkingCell->GetScene() );
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::ActionLockPointSelection()
{
    if( mWorkingCell )
    {
        FOdysseyPainterEditor::LockPointSelection( mEditor, mWorkingCell->GetScene() );
    }
}

void
UOdysseyPainterEditorVectorBaseTool::ActionUnlockPointSelection()
{
    if( mWorkingCell )
    {
        if( GetEditor()->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX )
        {
            FOdysseyPainterEditor::UnlockPointSelection( mEditor, mWorkingCell->GetScene() );
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
    mEditor->SetVectorHUDFlags( ( mEditor->GetVectorHUDFlags() & (~FOdysseyVectorHUD::HUD_MODE_ALL) ) | iViewMode );

    mEditor->SanitizeCurrentTool();

    if( mWorkingCell )
    {
        // select a object with an inbetweener tag if none are selected
        if( iViewMode & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
        {
            FOdysseyVectorTag* selectedTag = mWorkingCell->GetLayer()->GetSelectedTagByClassType( FOdysseyVectorTagInbetweener::StaticClass() );

            if ( selectedTag == nullptr )
            {
                FOdysseyVectorTag* lastTag = nullptr;

                for( FOdysseyVectorTag* tag : mWorkingCell->GetLayer()->GetSharedTagList() )
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

        // force invalidation
        mWorkingCell->Invalidate( FOdysseyVectorObjectInvalidationFlags().Set(FOdysseyVectorObjectInvalidationFlags::DEFAULT) );
        mWorkingCell->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        mWorkingCell->GetLayer()->RequestRedraw( mWorkingCell, 0 );
    }
}

void
UOdysseyPainterEditorVectorBaseTool::ExtendToolbar( UToolMenu* iToolMenu )
{
    Super::ExtendToolbar(iToolMenu);

    FToolMenuSection& section = iToolMenu->AddSection(NAME_None);

    uint64 HUDFlags = mEditor->GetVectorHUDFlags();

    if( HUDFlags & FOdysseyVectorHUD::HUD_MODE_OBJECT_ALLOWED )
    {
        section.AddEntry(
            FToolMenuEntry::InitToolBarButton(
                "VectorObjectMode",
                FUIAction(
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
                ),
                FText(),
                LOCTEXT("vector-edition-mode.object-mode.tooltip", "Object Mode"),
                FSlateIcon("OdysseyStyle", "PainterEditor.TopBar.VectorModeObject32"),
                EUserInterfaceActionType::RadioButton
            )
        );
    }

    if( HUDFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX_ALLOWED)
    {
        section.AddEntry(
            FToolMenuEntry::InitToolBarButton(
                "VectorVertexMode",
                FUIAction(
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
                ),
                FText(),
                LOCTEXT("vector-edition-mode.vertex-mode.tooltip", "Vertex Mode"),
                FSlateIcon("OdysseyStyle", "PainterEditor.TopBar.VectorModeVertex32"),
                EUserInterfaceActionType::RadioButton
            )
        );
    }

    if( HUDFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN_ALLOWED)
    {
        section.AddEntry(
            FToolMenuEntry::InitToolBarButton(
                "VectorInbetweenMode",
                FUIAction(
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
                ),
                FText(),
                LOCTEXT("vector-edition-mode.inbetween-mode.tooltip", "Inbetween Mode"),
                FSlateIcon("OdysseyStyle", "PainterEditor.TopBar.VectorModeInbetween32"),
                EUserInterfaceActionType::RadioButton
            )
        );
    }
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
    FMenuBuilder menu( true, mCommandList );

    menu.BeginSection("Context Menu");

    ExtendContextMenu( menu );

    menu.EndSection();

    return menu.MakeWidget();
}

void
UOdysseyPainterEditorVectorBaseTool::ExtendContextMenu( FMenuBuilder& menu )
{
    if( mWorkingCell )
    {
        if( GetEditor()->GetVectorHUDFlags() &  FOdysseyVectorHUD::HUD_MODE_OBJECT )
        {
            ExtendContextMenuObject( mWorkingCell->GetScene(), menu, 0 );
        }

        if( GetEditor()->GetVectorHUDFlags() &  FOdysseyVectorHUD::HUD_MODE_VERTEX )
        {
            ExtendContextMenuVertex( mWorkingCell->GetScene(), menu, 0 );
        }

        if( GetEditor()->GetVectorHUDFlags() &  FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
        {
            ExtendContextMenuInbetween( mWorkingCell->GetScene(), menu, 0 );
        }
    }
}

void
UOdysseyPainterEditorVectorBaseTool::ExtendContextMenuObject( FOdysseyVectorGroupPaint* iScene
                                                            , FMenuBuilder& menu
                                                            , uint64 iObjectMenuFlags )
{
    menu.AddMenuEntry( FOdysseyPainterEditorCommands::Get().VectorResetView
                     , NAME_None
                     , LOCTEXT("vector-tool.object-context-menu.reset-view.name", "Reset View")
                     , LOCTEXT("vector-tool.object-context-menu.reset-view.tooltip", "Reset View")
                     , FSlateIcon() );

    menu.AddMenuEntry( FOdysseyPainterEditorCommands::Get().VectorMakePaintGroup
                     , NAME_None
                     , LOCTEXT("vector-tool.object-context-menu.group-paint.name", "Make Paint Group")
                     , LOCTEXT("vector-tool.object-context-menu.group-paint.tooltip", "Make Paint Group")
                     , FSlateIcon() );

    menu.AddMenuEntry( FOdysseyPainterEditorCommands::Get().VectorGroup
                     , NAME_None
                     , LOCTEXT("vector-tool.object-context-menu.group.name", "Group")
                     , LOCTEXT("vector-tool.object-context-menu.group.tooltip", "Group")
                     , FSlateIcon() );

    menu.AddMenuEntry( FOdysseyPainterEditorCommands::Get().VectorUngroup
                     , NAME_None
                     , LOCTEXT("vector-tool.object-context-menu.ungroup.name", "Ungroup")
                     , LOCTEXT("vector-tool.object-context-menu.ungroup.tooltip", "Ungroup")
                     , FSlateIcon() );

    menu.AddMenuEntry( FOdysseyPainterEditorCommands::Get().VectorBringForward
                     , NAME_None
                     , LOCTEXT("vector-tool.object-context-menu.bring-forward.name", "Bring forward")
                     , LOCTEXT("vector-tool.object-context-menu.bring-forward.tooltip", "Bring forward")
                     , FSlateIcon() );

    menu.AddMenuEntry( FOdysseyPainterEditorCommands::Get().VectorSendBackward
                     , NAME_None
                     , LOCTEXT("vector-tool.object-context-menu.send-backward.name", "Send backward")
                     , LOCTEXT("vector-tool.object-context-menu.send-backward.tooltip", "Send backward")
                     , FSlateIcon() );

    menu.AddMenuSeparator();
    menu.AddMenuEntry( FGenericCommands::Get().Delete
                     , NAME_None
                     , LOCTEXT("vector-tool.object-context-menu.delete-selection.name","Delete Selection")
                     , LOCTEXT("vector-tool.object-context-menu.delete-selection.tooltip","Delete Selection")
                     , FSlateIcon() );
    menu.AddMenuSeparator();

    menu.AddMenuEntry( FOdysseyPainterEditorCommands::Get().VectorFlipHorizontal
                     , NAME_None
                     , LOCTEXT("vector-tool.object-context-menu.flip-horizontal.name","Flip Horizontal")
                     , LOCTEXT("vector-tool.object-context-menu.flip-horizontal.tooltip","Flip Horizontal")
                     , FSlateIcon() );

    menu.AddMenuEntry( FOdysseyPainterEditorCommands::Get().VectorFlipVertical
                     , NAME_None
                     , LOCTEXT("vector-tool.object-context-menu.flip-vertical.name","Flip Vertical")
                     , LOCTEXT("vector-tool.object-context-menu.flip-vertical.tooltip","Flip Vertical")
                     , FSlateIcon() );

    menu.AddMenuEntry( FOdysseyPainterEditorCommands::Get().VectorClearColoring
                     , NAME_None
                     , LOCTEXT("vector-tool.object-context-menu.clear-coloring.name", "Clear Coloring")
                     , LOCTEXT("vector-tool.object-context-menu.clear-coloring.tooltip", "Clear Coloring")
                     , FSlateIcon() );

    menu.AddMenuEntry( FOdysseyPainterEditorCommands::Get().VectorApplyTransformations
                     , NAME_None
                     , LOCTEXT("vector-tool.object-context-menu.apply-transformations.name", "Apply Transformations")
                     , LOCTEXT("vector-tool.object-context-menu.apply-transformations.tooltip", "Apply Transformations")
                     , FSlateIcon() );
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
    menu.AddMenuEntry( FOdysseyPainterEditorCommands::Get().VectorSubdivideSegments
                     , NAME_None
                     , LOCTEXT("vector-tool.vertex-context-menu.subdivide-segments.name", "Subdivide segments")
                     , LOCTEXT("vector-tool.vertex-context-menu.subdivide-segments.tooltip", "Subdivide segments")
                     , FSlateIcon() );

    menu.AddMenuSeparator();
    menu.AddMenuEntry( FGenericCommands::Get().Delete
                     , NAME_None
                     , LOCTEXT("vector-tool.vertex-context-menu.delete-selection.name", "Delete Selection")
                     , LOCTEXT("vector-tool.vertex-context-menu.delete-selection.tooltip", "Delete Selection")
                     , FSlateIcon() );
    menu.AddMenuSeparator();

    menu.AddMenuEntry( FOdysseyPainterEditorCommands::Get().VectorAlignPointSelection
                     , NAME_None
                     , LOCTEXT("vector-tool.vertex-context-menu.align-point-selection.name", "Align Point Selection")
                     , LOCTEXT("vector-tool.vertex-context-menu.align-point-selection.tooltip", "Align Point Selection")
                     , FSlateIcon() );

    menu.AddMenuEntry( FOdysseyPainterEditorCommands::Get().VectorUnalignPointSelection
                     , NAME_None
                     , LOCTEXT("vector-tool.vertex-context-menu.unalign-point-selection.name", "Unalign Point Selection")
                     , LOCTEXT("vector-tool.vertex-context-menu.unalign-point-selection.tooltip", "Unalign Point Selection")
                     , FSlateIcon() );

    menu.AddMenuEntry( FOdysseyPainterEditorCommands::Get().VectorLockPointSelection
                     , NAME_None
                     , LOCTEXT("vector-tool.vertex-context-menu.lock-point-selection.name", "Lock Point Selection")
                     , LOCTEXT("vector-tool.vertex-context-menu.lock-point-selection.tooltip", "Lock Point Selection")
                     , FSlateIcon() );

    menu.AddMenuEntry( FOdysseyPainterEditorCommands::Get().VectorUnlockPointSelection
                     , NAME_None
                     , LOCTEXT("vector-tool.vertex-context-menu.unlock-point-selection.name", "Unlock Point Selection")
                     , LOCTEXT("vector-tool.vertex-context-menu.unlock-point-selection.tooltip", "Unlock Point Selection")
                     , FSlateIcon() );
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
                  LOCTEXT("vector-tool.inbetween-context-menu.groupadd-inbetweener-tag.name", "Group and Add Inbetweener Tag")
                , LOCTEXT("vector-tool.inbetween-context-menu.groupadd-inbetweener-tag.tooltip", "Group and Add Inbetweener Tag")
                , FSlateIcon()
                , FUIAction( FExecuteAction::CreateStatic( &FOdysseyPainterEditor::GroupAndAddInbetweenerTag, GetEditor(), iScene )
                           , FCanExecuteAction::CreateStatic( &UOdysseyPainterEditorVectorBaseTool::CanAddTag, iScene ) ) );
    }

    menu.AddMenuEntry(
              LOCTEXT("vector-tool.inbetween-context-menu.add-inbetweener-tag.name", "Add Inbetweener Tag")
            , LOCTEXT("vector-tool.inbetween-context-menu.add-inbetweener-tag.tooltip", "Add Inbetweener Tag")
            , FSlateIcon()
            , FUIAction( FExecuteAction::CreateStatic( &FOdysseyPainterEditor::AddInbetweenerTag, GetEditor(), iScene )
                       , FCanExecuteAction::CreateStatic( &UOdysseyPainterEditorVectorBaseTool::CanAddTag, iScene )));

    menu.AddMenuEntry(
              LOCTEXT("vector-tool.inbetween-context-menu.remove-inbetweener-tag.name", "Remove Inbetweener Tag")
            , LOCTEXT("vector-tool.inbetween-context-menu.remove-inbetweener-tag.tooltip", "Remove Inbetweener Tag")
            , FSlateIcon()
            , FUIAction( FExecuteAction::CreateStatic( &FOdysseyPainterEditor::RemoveInbetweenerTag, GetEditor(), iScene )
                       , FCanExecuteAction::CreateStatic( &UOdysseyPainterEditorVectorBaseTool::CanAlterTag, iScene )));

    menu.AddMenuEntry(
            LOCTEXT("vector-tool.inbetween-context-menu.commit-inbetweener-tag.name", "Commit Inbetweener Tag")
          , LOCTEXT("vector-tool.inbetween-context-menu.commit-inbetweener-tag.tooltip", "Commit Inbetweener Tag")
          , FSlateIcon()
          , FUIAction( FExecuteAction::CreateStatic( &FOdysseyPainterEditor::CommitSelectedInbetweenerTag, GetEditor(), iScene->GetLayer() )
                     , FCanExecuteAction::CreateStatic( &UOdysseyPainterEditorVectorBaseTool::CanAlterTag, iScene )));

    menu.AddSubMenu(
        LOCTEXT("vector-tool.inbetween-context-menu.reset-grid.name", "Reset Grid")
      , LOCTEXT("vector-tool.inbetween-context-menu.reset-grid.tooltip", "Reset Grid")
      , FNewMenuDelegate::CreateUObject( this, &UOdysseyPainterEditorVectorBaseTool::ResetGridMenu, iScene ) );

    menu.AddMenuEntry(
        LOCTEXT("vector-tool.inbetween-context-menu.copy-grid.name", "Copy Grid")
      , LOCTEXT("vector-tool.inbetween-context-menu.copy-grid.tooltip", "Copy Grid")
      , FSlateIcon()
      , FUIAction( FExecuteAction::CreateStatic( &FOdysseyPainterEditor::CopyInbetweenerGrid, iScene )
                 , FCanExecuteAction::CreateStatic( &UOdysseyPainterEditorVectorBaseTool::CanAlterTag, iScene )));

    menu.AddMenuEntry(
        LOCTEXT("vector-tool.inbetween-context-menu.paste-grid.name", "Paste Grid")
      , LOCTEXT("vector-tool.inbetween-context-menu.paste-grid.tooltip", "Paste Grid")
      , FSlateIcon()
      , FUIAction( FExecuteAction::CreateStatic( &FOdysseyPainterEditor::PasteInbetweenerGrid, GetEditor(), iScene )
                 , FCanExecuteAction::CreateStatic( &UOdysseyPainterEditorVectorBaseTool::CanAlterTag, iScene )));

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
UOdysseyPainterEditorVectorBaseTool::CanUngroup()
{
    bool ret = false;

    if( mWorkingCell )
    {
        for( FOdysseyVectorObject* selectedObject : mWorkingCell->GetSelectedObjectList() )
        {
            if( ( selectedObject->HasBaseClass( FOdysseyVectorGroup::StaticClass() ) == false )
             || ( selectedObject == mWorkingCell->GetScene() ) )
            {
                return false;
            }

            ret = true;
        }
    }

    return ret;
}

bool
UOdysseyPainterEditorVectorBaseTool::CanBePainted()
{
    if( mWorkingCell )
    {
        for( FOdysseyVectorObject* selectedObject : mWorkingCell->GetSelectedObjectList() )
        {
            if( selectedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
            {
                return true;
            }
        }
    }

    return false;
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
                   , FCanExecuteAction::CreateStatic( &UOdysseyPainterEditorVectorBaseTool::CanAlterTag, iScene )));

    menu.AddMenuEntry(
          LOCTEXT("vector-tool.inbetween-context-menu.reset-grid-deformation.name", "Deformation")
        , LOCTEXT("vector-tool.inbetween-context-menu.reset-grid-deformation.tooltip", "Deformation")
        , FSlateIcon()
        , FUIAction( FExecuteAction::CreateStatic( &FOdysseyPainterEditor::ResetInbetweenerGrid, GetEditor(), iScene, false, true )
                   , FCanExecuteAction::CreateStatic( &UOdysseyPainterEditorVectorBaseTool::CanAlterTag, iScene )));

    menu.AddMenuEntry(
          LOCTEXT("vector-tool.inbetween-context-menu.reset-grid-both.name", "Both")
        , LOCTEXT("vector-tool.inbetween-context-menu.reset-grid-both.tooltip", "Both")
        , FSlateIcon()
        , FUIAction( FExecuteAction::CreateStatic( &FOdysseyPainterEditor::ResetInbetweenerGrid, GetEditor(), iScene, true, true )
                   , FCanExecuteAction::CreateStatic( &UOdysseyPainterEditorVectorBaseTool::CanAlterTag, iScene )));
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
                   , FCanExecuteAction::CreateStatic( &UOdysseyPainterEditorVectorBaseTool::CanAlterTag, iScene )));

    menu.AddMenuEntry(
          LOCTEXT("vector-tool.inbetween-context-menu.reset-spacing-all-breakdowns.name", "All Breakdonws")
        , LOCTEXT("vector-tool.inbetween-context-menu.reset-spacing-all-breakdowns.tooltip", "All Breakdonws")
        , FSlateIcon()
        , FUIAction( FExecuteAction::CreateStatic( &FOdysseyPainterEditor::ResetSpacingChart, GetEditor(), iScene, false, false )
                   , FCanExecuteAction::CreateStatic( &UOdysseyPainterEditorVectorBaseTool::CanAlterTag, iScene )));
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
                   , FCanExecuteAction::CreateStatic( &UOdysseyPainterEditorVectorBaseTool::CanAlterTag, iScene )));

    menu.AddMenuEntry(
          LOCTEXT("vector-tool.inbetween-context-menu.copy-spacing-all-breakdowns.name", "All Breakdonws")
        , LOCTEXT("vector-tool.inbetween-context-menu.copy-spacing-all-breakdowns.tooltip", "All Breakdonws")
        , FSlateIcon()
        , FUIAction( FExecuteAction::CreateStatic( &FOdysseyPainterEditor::CopySpacingChart, GetEditor(), iScene, false )
                   , FCanExecuteAction::CreateStatic( &UOdysseyPainterEditorVectorBaseTool::CanAlterTag, iScene )));
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
                   , FCanExecuteAction::CreateStatic( &UOdysseyPainterEditorVectorBaseTool::CanAlterTag, iScene )));

    menu.AddMenuEntry(
          LOCTEXT("vector-tool.inbetween-context-menu.paste-spacing-all-breakdowns.name", "All Breakdonws")
        , LOCTEXT("vector-tool.inbetween-context-menu.paste-spacing-all-breakdowns.tooltip", "All Breakdonws")
        , FSlateIcon()
        , FUIAction( FExecuteAction::CreateStatic( &FOdysseyPainterEditor::PasteSpacingChart, GetEditor(), iScene, false )
                   , FCanExecuteAction::CreateStatic( &UOdysseyPainterEditorVectorBaseTool::CanAlterTag, iScene )));
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

TOptional<FMouseCursor> UOdysseyPainterEditorVectorBaseTool::GetMouseCursorOverride() const
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return FMouseCursor(EMouseCursor::SlashedCircle);

    // working group can be null if were are not on a cell
    FOdysseyVectorGroup* workingGroup = GetWorkingGroup();

    if (workingGroup && !workingGroup->IsVisible(true))
        return FMouseCursor(EMouseCursor::SlashedCircle);

    return TOptional<FMouseCursor>();
}

#undef LOCTEXT_NAMESPACE

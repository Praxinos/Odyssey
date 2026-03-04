// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane
#include "ArianeEditorViewportEdMode.h"
#include "ArianeEditorViewportToolkit.h"
#include "ArianeEditorTool.h"
// Odyssey
#include "IOdysseyStylusInputModule.h"
// Unreal
#include "EdMode.h"
#include "EditorModeManager.h"
#include "LevelEditorViewport.h"
#include "SEditorViewport.h"
#include "Slate/SceneViewport.h"

/* Gary

#include "EditorModeManager.h"
#include "LevelEditor.h"
#include "Interfaces/IMainFrameModule.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "Toolkits/ToolkitManager.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Framework/Application/SlateApplication.h"
#include "EditorReimportHandler.h"
#include "MeshPaintHelpers.h"
#include "MeshPaintSettings.h"
#include "ToolMenus.h"
#include "Selection.h"

#include "EditorWorldExtension.h"
#include "ViewportWorldInteraction.h"
#include "Misc/MessageDialog.h"


#include "ArianeViewportDrawingEditorExtension.h"

#include "Adapters/IArianeViewportDrawingEditorAdapter.h"

#include "UObject/UObjectGlobals.h"
#include "Editor/EditorPerProjectUserSettings.h"
#include "MediaPlate.h"
#include "SEditorViewport.h"
*/

#define LOCTEXT_NAMESPACE "ArianeEditorViewport"

const FEditorModeID FArianeEditorViewportEdMode::EM_ArianeEditorViewportEdModeId = TEXT("EM_ArianeEditorViewportEdMode");

FArianeEditorViewportEdMode::~FArianeEditorViewportEdMode()
{
    if (GEditor)
    {
        GEditor->OnEditorClose().RemoveAll(this);
    }

    UOdysseyStylusInputSubsystem* inputSubsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();
    inputSubsystem->RemoveMessageHandler(*this);
}

FArianeEditorViewportEdMode::FArianeEditorViewportEdMode()
    : FEdMode()
    , bStylusInUse( false )
{
    GEditor->OnEditorClose().AddRaw(this, &FArianeEditorViewportEdMode::OnEditorClose);
}

void FArianeEditorViewportEdMode::Initialize()
{
    UOdysseyStylusInputSubsystem* inputSubsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();

    inputSubsystem->AddMessageHandler(*this);
}

void FArianeEditorViewportEdMode::AddReferencedObjects(FReferenceCollector& Collector)
{
    // Call parent implementation
    FEdMode::AddReferencedObjects(Collector);
    //if (mViewportDrawingEditorPainter)
        //mViewportDrawingEditorPainter->AddReferencedObjects(Collector);
}

void FArianeEditorViewportEdMode::Render(const FSceneView* View,FViewport* Viewport,FPrimitiveDrawInterface* PDI)
{
/* Gary
    if (!mViewportDrawingEditorExtension)
        return;

    UTexture* texture = mViewportDrawingEditorExtension->Texture();
    if (!texture)
        return;

    IArianeViewportDrawingEditorAdapter* adapter = mViewportDrawingEditorExtension->GetArianeViewportDrawingEditorAdapter();
    if (!adapter)
        return;

    adapter->RenderInteractorWidget(View, Viewport, PDI);
*/
}

void
FArianeEditorViewportEdMode::DrawHUD(FEditorViewportClient* ViewportClient,FViewport* Viewport,const FSceneView* View,FCanvas* Canvas)
{
/* Gary
    if (!mViewportDrawingEditorExtension || !mViewportDrawingEditorExtension->IsPlaneComponent())
        return;

    TSharedPtr<SWidget> viewportWidget = ViewportClient->GetEditorViewportWidget();
    TSharedPtr<SWindow> window = FSlateApplication::Get().FindWidgetWindow(viewportWidget.ToSharedRef());

    if( !window )
        return;

    float scaleFactor = FSlateApplication::Get().GetApplicationScale() * window->GetNativeWindow()->GetDPIScaleFactor();

    FOdysseyHUDElement::FDrawHUDParams params;
    if (!mViewportDrawingEditorExtension->GetDrawHUDParams(View, Canvas, scaleFactor, params))
        return;

    GetEditor()->HUDSystem()->Draw(params);
*/
}


bool FArianeEditorViewportEdMode::Select(AActor* InActor, bool bInSelected)
{
/* Gary
    if (!bInSelected)
        mViewportDrawingEditorExtension->SetActor(nullptr);
    else
        mViewportDrawingEditorExtension->SetActor(InActor);
*/

    return FEdMode::Select( InActor, bInSelected );
}

void
FArianeEditorViewportEdMode::OnStylusStateChanged( const TWeakPtr<SWidget> iWidget
                                                 , const TArray<FStylusState>& iStates
                                                 , int32 iIndex )
{
/*
    mStylusLastEventTime = std::chrono::steady_clock::now();

    if ( !IsReadyToDraw() || !mLastKnownViewport )
        return;

    TSharedPtr<SWidget> inWidget = iWidget.Pin();
    if( !inWidget)
        return;

    TSharedPtr< SViewport > viewport = GCurrentLevelEditingViewportClient->GetEditorViewportWidget()->GetSceneViewport()->GetViewportWidget().Pin();
    if( inWidget != viewport )
        return;

    //---

    mStylusStates = iStates;
    mLastStylusEventIndex = 0;

    ReadStylusInput();
*/

    for ( const FStylusState& StylusState : iStates )
    {
        TSharedPtr< SViewport > viewportWidget = GCurrentLevelEditingViewportClient->GetEditorViewportWidget()->GetSceneViewport()->GetViewportWidget().Pin();
        float scaleDPI = viewportWidget->GetCachedGeometry().GetAccumulatedLayoutTransform().GetScale();
        FVector2D positionInViewport = viewportWidget->GetCachedGeometry().AbsoluteToLocal( StylusState.GetPosition() ) * scaleDPI;
        static bool PreviouslyDown = false;

        if( StylusState.IsStylusDown() )
        {
            if( PreviouslyDown == false )
            {
                InputKey_Private( GCurrentLevelEditingViewportClient
                                , GCurrentLevelEditingViewportClient->GetEditorViewportWidget()->GetSceneViewport().Get()
                                , EKeys::LeftMouseButton
                                , FArianePointerState( positionInViewport.X, positionInViewport.Y, StylusState )
                                , EInputEvent::IE_Pressed );

                bStylusInUse = true;
            }
            else
            {
                CapturedMouseMove_Private( GCurrentLevelEditingViewportClient
                                         , GCurrentLevelEditingViewportClient->GetEditorViewportWidget()->GetSceneViewport().Get()
                                         , FArianePointerState( positionInViewport.X, positionInViewport.Y, StylusState ) );

                bStylusInUse = true;
            }

            PreviouslyDown = true;
        }
        else
        {
            if( PreviouslyDown == true )
            {
                InputKey_Private( GCurrentLevelEditingViewportClient
                                , GCurrentLevelEditingViewportClient->GetEditorViewportWidget()->GetSceneViewport().Get()
                                , EKeys::LeftMouseButton
                                , FArianePointerState( positionInViewport.X, positionInViewport.Y, StylusState )
                                , EInputEvent::IE_Released );

                bStylusInUse = false;
                PreviouslyDown = false;

                break;
            }
            // Mouse Hover does not need to be handled, we use the "normal" MouseMove callback.
        }
/*
        //Don't manage MouseDown when using the Right Mouse Button to allow hovered mouse clicks
        if (!mStylusIsDown && state.IsStylusDown() && mStylusButton != EKeys::RightMouseButton)
        {
            //MouseDown
            MouseDown(ray, mStylusButton);

            mStylusIsDown = true;
            mLastStylusEventIndex = i;
        }
        else if (mStylusIsDown && !state.IsStylusDown() && mStylusButton != EKeys::RightMouseButton)
        {
            //MouseUp
            MouseUp(ray, mStylusButton);

            mStylusIsDown = false;
            mLastStylusEventIndex = i;
        }
        //Force Right Mouse Button Drag
        else if (mStylusIsDown || mStylusButton == EKeys::RightMouseButton)
        {
            //MouseMove
            MouseDrag(ray);

            mLastStylusEventIndex = i;
        }
*/
    }

}

bool FArianeEditorViewportEdMode::MouseMove_Private( FEditorViewportClient* iViewportClient
                                                   , FViewport* iViewport
                                                   , const FArianePointerState& State )
{
    TSharedPtr<FArianeEditorViewportToolkit> viewportToolkit = GetArianeEditorViewportToolkit();

    if( viewportToolkit->GetEditor().GetCurrentTool() )
    {
        UArianeEditorTool* currenTool = viewportToolkit->GetEditor().GetCurrentTool();

        //currenTool->OnMouseDrag( FVector2D( iMouseX, iMouseY ) );
    }

/* Gary
    IArianeViewportDrawingEditorAdapter* adapter = mViewportDrawingEditorExtension->GetArianeViewportDrawingEditorAdapter();
    if (!adapter)
        return false;

    return adapter->MouseMove(iViewportClient, iViewport, iMouseX, iMouseY);
*/

    return false;
}

bool FArianeEditorViewportEdMode::MouseMove( FEditorViewportClient* iViewportClient
                                           , FViewport* iViewport
                                           , int32 iMouseX
                                           , int32 iMouseY )
{
    return ( bStylusInUse ) ? true : MouseMove_Private( iViewportClient
                                                      , iViewport
                                                      , FArianePointerState( iMouseX, iMouseY ) );
}

bool
FArianeEditorViewportEdMode::InputKey_Private( FEditorViewportClient* iViewportClient
                                             , FViewport* iViewport
                                             , FKey iKey
                                             , const FArianePointerState& State
                                             , EInputEvent iEvent )
{
    TSharedPtr<FArianeEditorViewportToolkit> viewportToolkit = GetArianeEditorViewportToolkit();

    if( viewportToolkit->GetEditor().GetCurrentTool() )
    {
        UArianeEditorTool* currenTool = viewportToolkit->GetEditor().GetCurrentTool();

        switch( iEvent )
        {
            case IE_Pressed :
                return currenTool->OnMouseDown( iViewportClient
                                              , iKey
                                              , State
                                              , false );
            break;

            case IE_Repeat :
                return currenTool->OnMouseDown( iViewportClient
                                                , iKey
                                                , State
                                                , true );
            break;

            case IE_Released :
                return currenTool->OnMouseUp( iViewportClient
                                            , iKey
                                            , State );
            break;

            default :
            break;
        }
    }

/*
    IE_Pressed              =0,
    IE_Released             =1,
    IE_Repeat               =2,
    IE_DoubleClick          =3,
    IE_Axis                 =4,
    IE_MAX                  =5,
*/

/* Gary
    if (!IsEditingEnabled())
        return false;

    IArianeViewportDrawingEditorAdapter* adapter = mViewportDrawingEditorExtension->GetArianeViewportDrawingEditorAdapter();
    if (!adapter)
        return false;

    return adapter->InputKey(iViewportClient, iViewport, iKey, iEvent);
*/

    return false;
}

bool
FArianeEditorViewportEdMode::InputKey( FEditorViewportClient* iViewportClient
                                     , FViewport* iViewport
                                     , FKey iKey
                                     , EInputEvent iEvent )
{
    return ( bStylusInUse ) ? true : InputKey_Private( iViewportClient
                                                     , iViewport
                                                     , iKey
                                                     , FArianePointerState( iViewport->GetMouseX(), iViewport->GetMouseY() )
                                                     , iEvent );
}

bool FArianeEditorViewportEdMode::CapturedMouseMove_Private( FEditorViewportClient* iViewportClient
                                                           , FViewport* iViewport
                                                           , const FArianePointerState& State )
{
    TSharedPtr<FArianeEditorViewportToolkit> viewportToolkit = GetArianeEditorViewportToolkit();

    if( viewportToolkit->GetEditor().GetCurrentTool() )
    {
        UArianeEditorTool* currenTool = viewportToolkit->GetEditor().GetCurrentTool();

        return currenTool->OnMouseDrag( iViewportClient
                                      , State );
    }
/* Gary
#if PLATFORM_MAC
    // Fix an issue where the captured and non captured cursor are in conflict, resulting in two cursors that teleport to each other
    // No need to put it back on true, it already does so automatically at each event, from the FReply in viewports CapturedMouseMove()
    FSlateApplication::Get().GetPlatformApplication()->SetHighPrecisionMouseMode(false, nullptr);
#endif

    IArianeViewportDrawingEditorAdapter* adapter = mViewportDrawingEditorExtension->GetArianeViewportDrawingEditorAdapter();
    if (!adapter)
        return false;

    return adapter->CapturedMouseMove(iViewportClient, iViewport, iMouseX, iMouseY);
*/

    return false;
}

bool FArianeEditorViewportEdMode::CapturedMouseMove( FEditorViewportClient* iViewportClient
                                                   , FViewport* iViewport
                                                   , int32 iMouseX
                                                   , int32 iMouseY )
{
    return ( bStylusInUse ) ? true : CapturedMouseMove_Private( iViewportClient
                                                              , iViewport
                                                              , FArianePointerState( iMouseX, iMouseY ) );
}

bool
FArianeEditorViewportEdMode::HandleClick_Private( FEditorViewportClient* InViewportClient
                                                , HHitProxy* HitProxy
                                                , const FViewportClick& Click
                                                , const FArianePointerState& State )
{
    TSharedPtr<FArianeEditorViewportToolkit> viewportToolkit = GetArianeEditorViewportToolkit();

    if( viewportToolkit->GetEditor().GetCurrentTool() )
    {
        UArianeEditorTool* currenTool = viewportToolkit->GetEditor().GetCurrentTool();
        const FKey& Key = Click.GetKey();

        return currenTool->OnMouseClick( InViewportClient
                                       , Key
                                       , State );
    }

/* Gary
    IArianeViewportDrawingEditorAdapter* adapter = mViewportDrawingEditorExtension->GetArianeViewportDrawingEditorAdapter();
    if (!adapter)
        return false;

    return adapter->HandleClick(InViewportClient, HitProxy, Click);
*/
    return false;
}

bool
FArianeEditorViewportEdMode::HandleClick( FEditorViewportClient* InViewportClient
                                        , HHitProxy* HitProxy
                                        , const FViewportClick& Click )
{
    return ( bStylusInUse ) ? true : HandleClick_Private( InViewportClient
                                                        , HitProxy
                                                        , Click
                                                        , FArianePointerState( Click.GetCursorPos().X
                                                                             , Click.GetCursorPos().Y ) );
}

bool
FArianeEditorViewportEdMode::GetCursor( EMouseCursor::Type& OutCursor ) const
{
/*
    IArianeViewportDrawingEditorAdapter* adapter = mViewportDrawingEditorExtension->GetArianeViewportDrawingEditorAdapter();
    if (!adapter)
        return false;

    return adapter->GetCursor(OutCursor);
*/
    return false;
}

bool FArianeEditorViewportEdMode::IsEditingEnabled() const
{
    return GetWorld() ? GetWorld()->GetFeatureLevel() >= ERHIFeatureLevel::SM5 : false;
}

void FArianeEditorViewportEdMode::OnEditorClose()
{
/* Gary
    // Reset viewport color mode for all active viewports
    for (FEditorViewportClient* ViewportClient : GEditor->GetAllViewportClients())
    {
        if (!ViewportClient || ViewportClient->GetModeTools() != GetModeManager())
        {
            continue;
        }

        MeshPaintHelpers::SetViewportColorMode(EMeshPaintColorViewMode::Normal, ViewportClient);
    }
*/
}

TSharedPtr<FArianeEditorViewportToolkit>
FArianeEditorViewportEdMode::GetArianeEditorViewportToolkit() const
{
    return StaticCastSharedPtr<FArianeEditorViewportToolkit>(Toolkit);
}

void
FArianeEditorViewportEdMode::Enter()
{
    TSharedPtr<FArianeEditorViewportToolkit> arianeToolkit = MakeShared<FArianeEditorViewportToolkit>(this);

    FEdMode::Enter();

    Toolkit = StaticCastSharedPtr<FModeToolkit>(arianeToolkit);

    Toolkit->Init( Owner->GetToolkitHost() );

/* Gary
    //checkf(mViewportDrawingEditorPainter != nullptr, TEXT("ViewportDrawingEditorPainter was not created"));

    if (UsesToolkits() && !Toolkit.IsValid())
    {
        mViewportDrawingEditorToolkit = MakeShared<FArianeEditorViewportToolkit>(this);
        Toolkit = mViewportDrawingEditorToolkit;
        mViewportDrawingEditorToolkit->Initialize(this, Owner->GetToolkitHost());

        mViewportDrawingEditorExtension = mViewportDrawingEditorToolkit->GetViewportDrawingExtension();

        TSharedPtr< ILevelEditor > levelEditor = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor").GetFirstLevelEditor();
        levelEditor->AppendCommands( Toolkit->GetToolkitCommands() );
    }

    // Change the engine to draw selected objects without a color boost, but unselected objects will
    // be darkened slightly.  This just makes it easier to paint on selected objects without the
    // highlight effect distorting the appearance.
    GEngine->OverrideSelectedMaterialColor( FLinearColor::Black );

    UEditorWorldExtensionCollection* ExtensionCollection = GEditor->GetEditorWorldExtensionsManager()->GetEditorWorldExtensions(GetWorld());
    if (ExtensionCollection != nullptr)
    {
        // Register to find out about VR input events
        UViewportWorldInteraction* ViewportWorldInteraction = Cast<UViewportWorldInteraction>(ExtensionCollection->FindExtension(UViewportWorldInteraction::StaticClass()));
        if (ViewportWorldInteraction != nullptr)
        {
            ViewportWorldInteraction->OnViewportInteractionInputAction().RemoveAll(this);

            // Hide the VR transform gizmo while we're in mesh paint mode.  It sort of gets in the way of painting.
            ViewportWorldInteraction->SetTransformGizmoVisible(false);
        }
    }

    //Set Default Selected Actor to the one selected in the viewport
    USelection* actorSelection = GEditor->ArianeEditor();
    int numObjects = actorSelection->Num();
    for (int i = 0; i < numObjects; i++)
    {
        UObject* selectedObject = actorSelection->GetSelectedObject(i);
        AActor* selectedActor = Cast<AActor>(selectedObject);
        if (selectedActor)
        {
            mViewportDrawingEditorExtension->SetActor(selectedActor);
            break;
        }
    }

    if( !UPhysicsSettings::Get()->bSupportUVFromHitResults )
    {
        FMessageDialog::Open(EAppMsgType::Ok,LOCTEXT("editor-mode.no-collision-uv-support-window.message","'Support UV From Hit Results' doesn't seem to be enabled. Enable it from project settings in order to use this paint editor properly."),LOCTEXT("editor-mode.no-collision-uv-support-window.title","CollisionUVNoSupport"));
    }
*/
}

void FArianeEditorViewportEdMode::Exit()
{
/* Gary
    // Restore selection color
    GEngine->RestoreSelectedMaterialColor();

    if (Toolkit.IsValid())
    {
        UToolMenus::Get()->UnregisterOwner( Toolkit.Get() );
        FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
        Toolkit.Reset();
    }

    if (mViewportDrawingEditorToolkit)
    {
        mViewportDrawingEditorToolkit->GetEditor()->OnClose();
        mViewportDrawingEditorToolkit = nullptr;
    }
    mViewportDrawingEditorExtension = nullptr;

    //mViewportDrawingEditorPainter->Finalize();
    //delete mViewportDrawingEditorPainter;
    //mViewportDrawingEditorPainter = nullptr;
*/

    // Call parent implementation
    FEdMode::Exit();
}

/* Gary
FOdysseyPainterEditor*
FArianeEditorViewportEdMode::GetEditor() const
{
    if (!mViewportDrawingEditorToolkit)
        return nullptr;

    return mViewportDrawingEditorToolkit->GetEditor().Get();
}
*/

#undef LOCTEXT_NAMESPACE

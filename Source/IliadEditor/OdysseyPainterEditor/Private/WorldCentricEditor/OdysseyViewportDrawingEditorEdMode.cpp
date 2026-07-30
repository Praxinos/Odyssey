// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyViewportDrawingEditorEdMode.h"

#include "OdysseyPainterEditor.h"
#include "OdysseyViewportDrawingEditorExtension.h"

#include "EdMode.h"
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
#include "UObject/UObjectGlobals.h"
#include "Editor/EditorPerProjectUserSettings.h"
#include "MediaPlate.h"
#include "SEditorViewport.h"
#include "EditorWorldExtension.h"
#include "ViewportWorldInteraction.h"
#include "Misc/MessageDialog.h"

#include "OdysseyViewportDrawingEditorToolkit.h"
#include "Adapters/IOdysseyViewportDrawingEditorAdapter.h"
#include "OdysseyTelemetry.h"


#define LOCTEXT_NAMESPACE "ViewportDrawingEditor"

const FEditorModeID FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId = TEXT("EM_OdysseyViewportDrawingEditorEdMode");

FOdysseyViewportDrawingEditorEdMode::~FOdysseyViewportDrawingEditorEdMode()
{
    if (GEditor)
    {
        GEditor->OnEditorClose().RemoveAll(this);
    }
}

FOdysseyViewportDrawingEditorEdMode::FOdysseyViewportDrawingEditorEdMode()
    : FEdMode()
{
    GEditor->OnEditorClose().AddRaw(this, &FOdysseyViewportDrawingEditorEdMode::OnEditorClose);
}

void FOdysseyViewportDrawingEditorEdMode::Initialize()
{
}

void FOdysseyViewportDrawingEditorEdMode::Render(const FSceneView* View,FViewport* Viewport,FPrimitiveDrawInterface* PDI)
{
    if (!mViewportDrawingEditorExtension)
        return;

    UTexture* texture = mViewportDrawingEditorExtension->Texture();
    if (!texture)
        return;

    IOdysseyViewportDrawingEditorAdapter* adapter = mViewportDrawingEditorExtension->GetOdysseyViewportDrawingEditorAdapter();
    if (!adapter)
        return;

    adapter->RenderInteractorWidget(View, Viewport, PDI);
}

void
FOdysseyViewportDrawingEditorEdMode::DrawHUD(FEditorViewportClient* ViewportClient,FViewport* Viewport,const FSceneView* View,FCanvas* Canvas)
{
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
}


bool FOdysseyViewportDrawingEditorEdMode::Select(AActor* InActor, bool bInSelected)
{
    if (!bInSelected)
        mViewportDrawingEditorExtension->SetActor(nullptr);
    else
        mViewportDrawingEditorExtension->SetActor(InActor);

    return FEdMode::Select(InActor, bInSelected);
}

bool FOdysseyViewportDrawingEditorEdMode::MouseEnter(FEditorViewportClient* iViewportClient, FViewport* iViewport, int32 iMouseX, int32 iMouseY)
{
    IOdysseyViewportDrawingEditorAdapter* adapter = mViewportDrawingEditorExtension->GetOdysseyViewportDrawingEditorAdapter();
    if (!adapter)
        return false;

    return adapter->MouseEnter(iViewportClient, iViewport, iMouseX, iMouseY);
}

bool FOdysseyViewportDrawingEditorEdMode::MouseLeave(FEditorViewportClient* iViewportClient, FViewport* iViewport)
{
    IOdysseyViewportDrawingEditorAdapter* adapter = mViewportDrawingEditorExtension->GetOdysseyViewportDrawingEditorAdapter();
    if (!adapter)
        return false;

    return adapter->MouseLeave(iViewportClient, iViewport);
}

bool FOdysseyViewportDrawingEditorEdMode::MouseMove(FEditorViewportClient* iViewportClient, FViewport* iViewport, int32 iMouseX, int32 iMouseY)
{
    IOdysseyViewportDrawingEditorAdapter* adapter = mViewportDrawingEditorExtension->GetOdysseyViewportDrawingEditorAdapter();
    if (!adapter)
        return false;

    return adapter->MouseMove(iViewportClient, iViewport, iMouseX, iMouseY);
}

bool FOdysseyViewportDrawingEditorEdMode::InputKey(FEditorViewportClient* iViewportClient, FViewport* iViewport, FKey iKey, EInputEvent iEvent)
{
    if (!IsEditingEnabled())
        return false;

    IOdysseyViewportDrawingEditorAdapter* adapter = mViewportDrawingEditorExtension->GetOdysseyViewportDrawingEditorAdapter();
    if (!adapter)
        return false;

    return adapter->InputKey(iViewportClient, iViewport, iKey, iEvent);
}

bool FOdysseyViewportDrawingEditorEdMode::CapturedMouseMove(FEditorViewportClient* iViewportClient, FViewport* iViewport, int32 iMouseX, int32 iMouseY)
{
#if PLATFORM_MAC
    // Fix an issue where the captured and non captured cursor are in conflict, resulting in two cursors that teleport to each other
    // No need to put it back on true, it already does so automatically at each event, from the FReply in viewports CapturedMouseMove()
    FSlateApplication::Get().GetPlatformApplication()->SetHighPrecisionMouseMode(false, nullptr);
#endif

    IOdysseyViewportDrawingEditorAdapter* adapter = mViewportDrawingEditorExtension->GetOdysseyViewportDrawingEditorAdapter();
    if (!adapter)
        return false;

    return adapter->CapturedMouseMove(iViewportClient, iViewport, iMouseX, iMouseY);
}

bool
FOdysseyViewportDrawingEditorEdMode::HandleClick(FEditorViewportClient* iViewportClient, HHitProxy* iHitProxy, const FViewportClick& iClick)
{
    IOdysseyViewportDrawingEditorAdapter* adapter = mViewportDrawingEditorExtension->GetOdysseyViewportDrawingEditorAdapter();
    if (!adapter)
        return false;

    return adapter->HandleClick(iViewportClient, iHitProxy, iClick);
}

bool
FOdysseyViewportDrawingEditorEdMode::GetCursor(EMouseCursor::Type& OutCursor) const
{
    IOdysseyViewportDrawingEditorAdapter* adapter = mViewportDrawingEditorExtension->GetOdysseyViewportDrawingEditorAdapter();
    if (!adapter)
        return false;

    return adapter->GetCursor(OutCursor);
}

bool FOdysseyViewportDrawingEditorEdMode::IsEditingEnabled() const
{
    return GetWorld() ? GetWorld()->GetFeatureLevel() >= ERHIFeatureLevel::SM5 : false;
}

void FOdysseyViewportDrawingEditorEdMode::OnEditorClose()
{
    // Reset viewport color mode for all active viewports
    for (FEditorViewportClient* ViewportClient : GEditor->GetAllViewportClients())
    {
        if (!ViewportClient || ViewportClient->GetModeTools() != GetModeManager())
        {
            continue;
        }

        MeshPaintHelpers::SetViewportColorMode(EMeshPaintColorViewMode::Normal, ViewportClient);
    }
}

void FOdysseyViewportDrawingEditorEdMode::Enter()
{
    FEdMode::Enter();

    SessionStartTime = FDateTime::UtcNow();

    if (UsesToolkits() && !Toolkit.IsValid())
    {
        mViewportDrawingEditorToolkit = MakeShared<FOdysseyViewportDrawingEditorToolkit>(this);
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
    USelection* actorSelection = GEditor->GetSelectedActors();
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
}

void FOdysseyViewportDrawingEditorEdMode::Exit()
{
    // Restore selection color
    GEngine->RestoreSelectedMaterialColor();

    if (Toolkit.IsValid())
    {
        UToolMenus::Get()->UnregisterOwner( Toolkit.Get() );
        FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
        Toolkit.Reset();
    }

    mViewportDrawingEditorExtension = nullptr;
    mViewportDrawingEditorToolkit = nullptr;

    {
        // Use this in a module startup, but maybe to painful to store/compute duration (?)
        //GLevelEditorModeTools().OnEditorModeIDChanged().AddLambda( ... );

        using FAssetEditionFields = FAssetEdition_TelemetryFields;

        TArray<FAnalyticsEventAttribute> Attributes;
        Attributes.Emplace( FAssetEditionFields::EditorName_KeyName_AsString, TEXT( "EdMode:Odyssey" ) ); // GetModeInfo().Name.ToString() == "Odyssey"
        Attributes.Emplace( FAssetEditionFields::SessionDuration_KeyName_AsDouble, ( FDateTime::UtcNow() - SessionStartTime ).GetTotalSeconds() );

        FOdysseyTelemetry::Get().RecordEvent( FAssetEditionFields::KeyName, Attributes );
    }

    // Call parent implementation
    FEdMode::Exit();
}

FOdysseyPainterEditor*
FOdysseyViewportDrawingEditorEdMode::GetEditor() const
{
    if (!mViewportDrawingEditorToolkit)
        return nullptr;

    return mViewportDrawingEditorToolkit->GetEditor().Get();
}

TSharedPtr<FOdysseyViewportDrawingEditorToolkit>
FOdysseyViewportDrawingEditorEdMode::GetViewportDrawingEditorToolkit() const
{
    return mViewportDrawingEditorToolkit;
}

#undef LOCTEXT_NAMESPACE

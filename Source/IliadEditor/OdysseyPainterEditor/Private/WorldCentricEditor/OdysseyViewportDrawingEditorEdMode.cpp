// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyViewportDrawingEditorEdMode.h"

#include "PainterEditor/OdysseyPainterEditor.h"
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

#include "EditorWorldExtension.h"
#include "ViewportWorldInteraction.h"
#include "Misc/MessageDialog.h"

#include "OdysseyViewportDrawingEditorToolkit.h"
//#include "OdysseyViewportDrawingEditorPainter.h"
#include "OdysseyTextureEditorExtension.h"
#include "Adapters/IOdysseyViewportDrawingEditorAdapter.h"

#include "UObject/UObjectGlobals.h"
#include "Editor/EditorPerProjectUserSettings.h"
#include "MediaPlate.h"
#include "SEditorViewport.h"

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
    GEditor->OnEditorClose().AddRaw(this, &FOdysseyViewportDrawingEditorEdMode::OnResetViewMode);
}

void FOdysseyViewportDrawingEditorEdMode::Initialize()
{
}

void FOdysseyViewportDrawingEditorEdMode::AddReferencedObjects(FReferenceCollector& Collector)
{
    // Call parent implementation
    FEdMode::AddReferencedObjects(Collector);
    //if (mViewportDrawingEditorPainter)
        //mViewportDrawingEditorPainter->AddReferencedObjects(Collector);
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

    FOdysseyHUD::FDrawHUDParams params;
    if (!mViewportDrawingEditorExtension->GetDrawHUDParams(View, Canvas, scaleFactor, params))
        return;

    GetEditor()->HUDSystem()->DrawHUD(params);
}


bool FOdysseyViewportDrawingEditorEdMode::Select(AActor* InActor, bool bInSelected)
{
    if (!bInSelected)
        mViewportDrawingEditorExtension->SetActor(nullptr);
    else
        mViewportDrawingEditorExtension->SetActor(InActor);

    return FEdMode::Select(InActor, bInSelected);
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
    IOdysseyViewportDrawingEditorAdapter* adapter = mViewportDrawingEditorExtension->GetOdysseyViewportDrawingEditorAdapter();
    if (!adapter)
        return false;

    return adapter->CapturedMouseMove(iViewportClient, iViewport, iMouseX, iMouseY);
}

bool
FOdysseyViewportDrawingEditorEdMode::HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click)
{
    IOdysseyViewportDrawingEditorAdapter* adapter = mViewportDrawingEditorExtension->GetOdysseyViewportDrawingEditorAdapter();
    if (!adapter)
        return false;

    return adapter->HandleClick(InViewportClient, HitProxy, Click);
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

void FOdysseyViewportDrawingEditorEdMode::OnResetViewMode()
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

    //checkf(mViewportDrawingEditorPainter != nullptr, TEXT("ViewportDrawingEditorPainter was not created"));

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

    if (mViewportDrawingEditorToolkit)
    {
        mViewportDrawingEditorToolkit->GetEditor()->OnClose();
        mViewportDrawingEditorToolkit = nullptr;
    }
    mViewportDrawingEditorExtension = nullptr;

    //mViewportDrawingEditorPainter->Finalize();
    //delete mViewportDrawingEditorPainter;
    //mViewportDrawingEditorPainter = nullptr;

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

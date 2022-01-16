// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyViewportDrawingEditorEdMode.h"
#include "EdMode.h"
#include "EditorModeManager.h"
#include "LevelEditor.h"
#include "Interfaces/IMainFrameModule.h" 
#include "PhysicsEngine/PhysicsSettings.h"
#include "Widgets/Docking/SDockableTab.h" 
#include "Toolkits/ToolkitManager.h"
#include "AssetRegistryModule.h"
#include "EditorReimportHandler.h"
#include "MeshPaintHelpers.h"
#include "MeshPaintSettings.h"
#include "ToolMenus.h"

#include "EditorWorldExtension.h"
#include "ViewportWorldInteraction.h"

#include "OdysseyViewportDrawingEditor.h"
#include "OdysseyViewportDrawingEditorToolkit.h"
#include "OdysseyViewportDrawingEditorPainter.h"

#include "UObject/UObjectGlobals.h"
#include "Editor/EditorPerProjectUserSettings.h"

#define LOCTEXT_NAMESPACE "FOdysseyViewportDrawingEditorEdMode"

const FEditorModeID FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId = TEXT("EM_OdysseyViewportDrawingEditorEdMode");

FOdysseyViewportDrawingEditorEdMode::FOdysseyViewportDrawingEditorEdMode()
    : FEdMode()
{
    GEditor->OnEditorClose().AddRaw(this, &FOdysseyViewportDrawingEditorEdMode::OnResetViewMode);
}

FOdysseyViewportDrawingEditorEdMode::~FOdysseyViewportDrawingEditorEdMode()
{
    if (GEditor)
    {
        GEditor->OnEditorClose().RemoveAll(this);
    }
}

void FOdysseyViewportDrawingEditorEdMode::Initialize()
{
    mEditor = MakeShareable(new FOdysseyViewportDrawingEditor());
	mToolkit = MakeShareable(new FOdysseyViewportDrawingEditorToolkit(mEditor, this));
    mEditor->Initialize(nullptr);
    mToolkit->Initialize();

    mViewportDrawingEditorPainter = new FOdysseyViewportDrawingEditorPainter(mEditor);
}


TSharedPtr<class FModeToolkit> FOdysseyViewportDrawingEditorEdMode::GetToolkit()
{
    return mToolkit;
}

void FOdysseyViewportDrawingEditorEdMode::AddReferencedObjects(FReferenceCollector& Collector)
{
    // Call parent implementation
    FEdMode::AddReferencedObjects(Collector);
    mViewportDrawingEditorPainter->AddReferencedObjects(Collector);
}

void FOdysseyViewportDrawingEditorEdMode::Render(const FSceneView* View,FViewport* Viewport,FPrimitiveDrawInterface* PDI)
{
    /** Call parent implementation */
    //FEdMode::Render( View, Viewport, PDI );
}

bool FOdysseyViewportDrawingEditorEdMode::Select(AActor* InActor, bool bInSelected)
{
    mViewportDrawingEditorPainter->SelectActor(InActor);

    return false;
}

bool FOdysseyViewportDrawingEditorEdMode::MouseMove(FEditorViewportClient* iViewportClient, FViewport* iViewport, int32 iMouseX, int32 iMouseY)
{
    bool bHandled = false;

    if (mViewportDrawingEditorPainter->GetOdysseyViewportDrawingEditorAdapter())
        bHandled = mViewportDrawingEditorPainter->GetOdysseyViewportDrawingEditorAdapter()->MouseMove(iViewportClient, iViewport, iMouseX, iMouseY);

    return bHandled;
}

bool FOdysseyViewportDrawingEditorEdMode::InputKey(FEditorViewportClient* iViewportClient, FViewport* iViewport, FKey iKey, EInputEvent iEvent)
{
    if (!IsEditingEnabled())
    {
        return false;
    }

    bool bHandled = false;

    if (mViewportDrawingEditorPainter->GetOdysseyViewportDrawingEditorAdapter())
        bHandled = mViewportDrawingEditorPainter->GetOdysseyViewportDrawingEditorAdapter()->InputKey(iViewportClient, iViewport, iKey, iEvent);

    return bHandled;
}

bool FOdysseyViewportDrawingEditorEdMode::CapturedMouseMove(FEditorViewportClient* iViewportClient, FViewport* iViewport, int32 iMouseX, int32 iMouseY)
{
    bool bHandled = false;

    if (mViewportDrawingEditorPainter->GetOdysseyViewportDrawingEditorAdapter())
        bHandled = mViewportDrawingEditorPainter->GetOdysseyViewportDrawingEditorAdapter()->CapturedMouseMove(iViewportClient, iViewport, iMouseX, iMouseY);

    return bHandled;
}

void FOdysseyViewportDrawingEditorEdMode::Tick(FEditorViewportClient* ViewportClient, float DeltaTime)
{
    FEdMode::Tick(ViewportClient, DeltaTime);
    mViewportDrawingEditorPainter->Tick(ViewportClient, DeltaTime);
}

bool FOdysseyViewportDrawingEditorEdMode::IsEditingEnabled() const
{
	return GetWorld() ? GetWorld()->FeatureLevel >= ERHIFeatureLevel::SM5 : false;
}

void FOdysseyViewportDrawingEditorEdMode::CleanupTabsToolbar(const TSharedRef<SWindow>& Window)
{
    FLevelEditorModule& levelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(FName("LevelEditor"));
    TSharedRef<FTabManager> tabManager = levelEditorModule.GetLevelEditorTabManager()->AsShared();

    mEditor->UnregisterTabSpawners( tabManager );
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

    checkf(mViewportDrawingEditorPainter != nullptr, TEXT("ViewportDrawingEditorPainter was not created"));

    if (UsesToolkits() && !Toolkit.IsValid())
    {
        Toolkit = GetToolkit();
        Toolkit->Init(Owner->GetToolkitHost());
        TSharedPtr< ILevelEditor > levelEditor = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor").GetFirstLevelEditor();
        levelEditor->AppendCommands( Toolkit->GetToolkitCommands() );
    }

    if( mToolkit )
        mToolkit->ExtendMenu();

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

    mViewportDrawingEditorPainter->Initialize();


    if( !UPhysicsSettings::Get()->bSupportUVFromHitResults )
    {
        FText Title = LOCTEXT("TitleCollisionUVNoSupport","CollisionUVNoSupport");
        FMessageDialog::Open(EAppMsgType::Ok,LOCTEXT("Enable FindCollisionUV","'Support UV From Hit Results' doesn't seem to be enabled. Enable it from project settings in order to use this paint editor properly."),&Title);
    }

    IMainFrameModule& mainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
    const TSharedPtr<SWindow>& mainFrameParentWindow = mainFrameModule.GetParentWindow();
    mainFrameParentWindow->SetOnWindowClosed(FOnWindowClosed::CreateSP(this, &FOdysseyViewportDrawingEditorEdMode::CleanupTabsToolbar));
}

void FOdysseyViewportDrawingEditorEdMode::Exit()
{
    /** Finish up painting if we still are */
    /*if (mViewportDrawingEditorPainter->GetOdysseyViewportDrawingEditorAdapter()->GetState() == eState::kIdleReady)
    {
        mViewportDrawingEditorPainter->FinishPainting();
    }*/

    // Restore selection color
    GEngine->RestoreSelectedMaterialColor();

    if (Toolkit.IsValid())
    {
        UToolMenus::Get()->UnregisterOwner( Toolkit.Get() );
        FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
        Toolkit.Reset();
    }

    // Unbind delegates
    FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
    assetRegistryModule.Get().OnAssetRemoved().RemoveAll(this);
    FReimportManager::Instance()->OnPostReimport().RemoveAll(this);
    GEditor->GetEditorSubsystem<UImportSubsystem>()->OnAssetPostImport.RemoveAll(this);
    FEditorDelegates::PreSaveWorld.RemoveAll(this);
    FEditorDelegates::PostSaveWorld.RemoveAll(this);
    GEditor->OnObjectsReplaced().RemoveAll(this);
    //USelection::SelectionChangedEvent.Remove(SelectionChangedHandle);

    mViewportDrawingEditorPainter->Finalize();

    // Call parent implementation
    FEdMode::Exit();
}

#undef LOCTEXT_NAMESPACE

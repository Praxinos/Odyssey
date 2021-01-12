// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyViewportDrawingEditorPainter.h"

#include "Engine/Selection.h"

#include "Components/StaticMeshComponent.h"
#include "ComponentReregisterContext.h"
#include "StaticMeshResources.h"
#include "Components/SkeletalMeshComponent.h"


#include "MeshPaintModule.h"
#include "OdysseyViewportDrawingEditorEdMode.h"

#include "MeshPaintHelpers.h"
#include "ScopedTransaction.h"

#include "Misc/FeedbackContext.h"
#include "MeshPaintSettings.h"
#include "Dialogs/Dialogs.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Texture2DPreview.h"
#include "CanvasTypes.h"
#include "CanvasItem.h"
#include "MeshPaintTypes.h"
#include "TexturePaintHelpers.h"

#include "VREditorMode.h"
#include "IVREditorModule.h"
#include "ViewportWorldInteraction.h"
#include "ViewportInteractableInterface.h"
#include "VREditorInteractor.h"
#include "EditorWorldExtension.h"

#include "Framework/Commands/UICommandList.h"
#include "OdysseyViewportDrawingEditorCommands.h"
#include "OdysseyViewportDrawingEditorSettings.h"
#include "MeshPaintAdapterFactory.h"
#include "IMeshPaintGeometryAdapter.h"

#include "PackageTools.h"

#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorPainter"

FOdysseyViewportDrawingEditorPainter::~FOdysseyViewportDrawingEditorPainter()
{
	Finalize();
    delete mTemporaryViewportClient;
}

FOdysseyViewportDrawingEditorPainter::FOdysseyViewportDrawingEditorPainter()
    : mPaintSettings(UOdysseyViewportDrawingEditorSettings::Get()), 
    mTexturePaintingCurrentMeshComponent(nullptr),
	mPaintingTexture2D(nullptr),
    mStrokeBufferTexture2D(nullptr),
    mBrushRenderTargetTexture(nullptr),
	mDoRestoreRenTargets(false),
	mDoRefreshCachedData(true),
	mUICommandList(MakeShareable(new FUICommandList())),
    mFocusedViewport( nullptr ),
    mTemporaryViewportClient(new FOdysseyViewportDrawingEditorViewportClient() ),
    mIsGoingToDraw( false ),
	mIsCapturedByStylus(false),
	mBeginPosition(0, 0)
{
}

FOdysseyViewportDrawingEditorPainter* FOdysseyViewportDrawingEditorPainter::Get()
{
	static FOdysseyViewportDrawingEditorPainter* painter = nullptr;
	if (!painter)
	{
		painter = new FOdysseyViewportDrawingEditorPainter();
		painter->Initialize();
	}
	return painter;
}

void FOdysseyViewportDrawingEditorPainter::Initialize()
{
	//Create Data / View / Controller
	TSharedPtr<FOdysseyViewportDrawingEditorData> data = MakeShareable(new FOdysseyViewportDrawingEditorData());
	mWidget = MakeShareable(new SOdysseyViewportDrawingEditorGUI());
	mController = MakeShareable(new FOdysseyViewportDrawingEditorController(data, mWidget));

	//Register commands
	RegisterTexturePaintCommands();

	// Handle Stylus SubSystem
	UOdysseyStylusInputSubsystem* inputSubsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();
	inputSubsystem->AddMessageHandler(*this);
	inputSubsystem->OnStylusInputChanged().BindRaw(this, &FOdysseyViewportDrawingEditorPainter::OnStylusInputChanged);

    /** Setup necessary data */
    mBrushSettings = DuplicateObject<UPaintBrushSettings>(GetMutableDefault<UPaintBrushSettings>(),GetTransientPackage());
    mBrushSettings->AddToRoot();
    mBrushSettings->SetBrushRadius(50);

	//Handle Object Property Changed Callback to refresh when object's visibility changes for example
    FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(this,&FOdysseyViewportDrawingEditorPainter::UpdatePaintTargets);

	//Initialize Data / View / Controller
    mController->GetData()->Init(mPaintSettings->mTexturePaintSettings.mPaintTexture);
    mWidget->Init(this);
    mController->Init();

    //Select default brush
    mController->GetGUI()->GetBrushSelector()->SelectBrush(LoadObject<UOdysseyBrush>(nullptr,TEXT("/Iliad/Brushes/Drawing_Tools/Penbrush1.Penbrush1")));
}

void
FOdysseyViewportDrawingEditorPainter::OnStylusInputChanged(TSharedPtr<IStylusInputInterfaceInternal> iStylusInput)
{
}

void FOdysseyViewportDrawingEditorPainter::Finalize()
{
    mWidget.Reset(); mWidget = 0;
    mController.Reset(); mController = 0; //Resetting the controller also reset the data inside it. Should work differently

    UnregisterTexturePaintCommands();

	UOdysseyStylusInputSubsystem* inputSubsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();
    inputSubsystem->OnStylusInputChanged().Unbind();
	inputSubsystem->RemoveMessageHandler(*this);

	mBrushSettings->RemoveFromRoot();

    FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll(this);

	Cleanup(); //Why ?
	mComponentToTexturePaintSettingsMap.Empty(); //Why ?
}

void FOdysseyViewportDrawingEditorPainter::RegisterTexturePaintCommands()
{
	mUICommandList->MapAction(
		FOdysseyViewportDrawingEditorCommands::Get().SetOdysseyBrushSettingsView,
		FExecuteAction::CreateRaw(mController->GetGUI().Get(), &SOdysseyViewportDrawingEditorGUI::OnSetOdysseyBrushSettingsView),
		FCanExecuteAction(),
    	FIsActionChecked::CreateLambda([=]
	    {
		    return mController->GetGUI()->GetSelectedView() == EOdysseyViewportSelectedView::kBrushSettings;
        }));

	mUICommandList->MapAction(
		FOdysseyViewportDrawingEditorCommands::Get().SetOdysseyStrokeOptionsView,
		FExecuteAction::CreateRaw(mController->GetGUI().Get(), &SOdysseyViewportDrawingEditorGUI::OnSetOdysseyStrokeOptionsView),
		FCanExecuteAction(),
    	FIsActionChecked::CreateLambda([=]
	    {
		    return mController->GetGUI()->GetSelectedView() == EOdysseyViewportSelectedView::kStrokeOptions;
        }));

	mUICommandList->MapAction(
		FOdysseyViewportDrawingEditorCommands::Get().SetOdysseyLayerStackView,
		FExecuteAction::CreateRaw(mController->GetGUI().Get(), &SOdysseyViewportDrawingEditorGUI::OnSetOdysseyLayerStackView),
		FCanExecuteAction(),
    	FIsActionChecked::CreateLambda([=]
	    {
		    return mController->GetGUI()->GetSelectedView() == EOdysseyViewportSelectedView::kLayerStack;
        }));

    mUICommandList->MapAction(
        FOdysseyViewportDrawingEditorCommands::Get().SetOdysseyToolsView,
        FExecuteAction::CreateRaw(mController->GetGUI().Get(),&SOdysseyViewportDrawingEditorGUI::OnSetOdysseyToolsView),
        FCanExecuteAction(),
        FIsActionChecked::CreateLambda([=]
        {
            return mController->GetGUI()->GetSelectedView() == EOdysseyViewportSelectedView::kTools;
        }));
}

void FOdysseyViewportDrawingEditorPainter::Render(const FSceneView* iView, FViewport* iViewport, FPrimitiveDrawInterface* iPDI)
{
	/** Render viewport interactors */
	//RenderInteractors(iView, iViewport, iPDI, false);
}

bool FOdysseyViewportDrawingEditorPainter::Paint(FViewport* iViewport, const FVector& iCameraOrigin, const FVector& iRayOrigin, const FVector& iRayDirection)
{
	// Determine paint action according to whether or not shift is held down
	const EMeshPaintAction paintAction = (iViewport->KeyState(EKeys::LeftShift) || iViewport->KeyState(EKeys::RightShift)) ? EMeshPaintAction::Erase : EMeshPaintAction::Paint;
	
	const float paintStrength = iViewport->IsPenActive() ? iViewport->GetTabletPressure() : 1.f;
	// Handle internal painting functionality
	TPair<FVector, FVector> Ray(iRayOrigin, iRayDirection);
	return PaintInternal(iCameraOrigin, MakeArrayView(&Ray, 1), paintAction, paintStrength);
}

bool FOdysseyViewportDrawingEditorPainter::Paint(FViewport* iViewport, const FVector& iCameraOrigin, const TArrayView<TPair<FVector, FVector>>& iRays)
{
	// Determine paint action according to whether or not shift is held down
	const EMeshPaintAction paintAction = (iViewport->KeyState(EKeys::LeftShift) || iViewport->KeyState(EKeys::RightShift)) ? EMeshPaintAction::Erase : EMeshPaintAction::Paint;

	const float paintStrength = iViewport->IsPenActive() ? iViewport->GetTabletPressure() : 1.f;
	// Handle internal painting functionality
	return PaintInternal(iCameraOrigin, iRays, paintAction, paintStrength);
}

UPaintBrushSettings* FOdysseyViewportDrawingEditorPainter::GetBrushSettings()
{
	return mBrushSettings;
}

UMeshPaintSettings* FOdysseyViewportDrawingEditorPainter::GetPainterSettings()
{
	return mPaintSettings;
}

TSharedPtr<class SWidget> FOdysseyViewportDrawingEditorPainter::GetWidget()
{
    return mWidget->GetMainWidget();
}

TSharedPtr<FUICommandList> FOdysseyViewportDrawingEditorPainter::GetUICommandList()
{
	return mUICommandList;
}

TSharedPtr<FOdysseyViewportDrawingEditorController> FOdysseyViewportDrawingEditorPainter::GetController() const
{
    return mController;
}


bool FOdysseyViewportDrawingEditorPainter::ShouldFilterTextureAsset(const FAssetData& iAssetData) const
{	
	return !(mPaintableTextures.ContainsByPredicate([=](const FPaintableTexture& iTexture) { return iTexture.Texture->GetFullName() == iAssetData.GetFullName(); }));
}

void FOdysseyViewportDrawingEditorPainter::PaintTextureChanged(const FAssetData& iAssetData)
{
	UTexture2D* texture = Cast<UTexture2D>(iAssetData.GetAsset());

	if (texture)
	{
		//check
		UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
		if (texture != mController->GetData()->Texture() && AssetEditorSubsystem->FindEditorForAsset(texture, true) != nullptr)
		{
			FText Title = LOCTEXT("TitleSelectedTextureAlreadyOpenedTitle", "Selected Texture Already Opened");
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("SelectedTextureAlreadyOpened", "The selected texture is already opened in an other editor. Please close the editor before selecting this texture."), &Title);
			return;
		}

        mPaintSettings->mTexturePaintSettings.mPaintTexture = texture;
        texture->TemporarilyDisableStreaming();

        mController->OnEditedTextureChanged(mPaintSettings->mTexturePaintSettings.mPaintTexture);
		// Loop through our list of textures and see which one the user wants to select
		for (int32 targetIndex = 0; targetIndex < mTexturePaintTargetList.Num(); targetIndex++)
		{
			FTextureTargetListInfo& textureTarget = mTexturePaintTargetList[targetIndex];
			if (textureTarget.TextureData == texture)
			{
				textureTarget.bIsSelected = true;
			}
			else
			{
				textureTarget.bIsSelected = false;
			}
		}
	}
}

void FOdysseyViewportDrawingEditorPainter::UnregisterTexturePaintCommands()
{
	const FOdysseyViewportDrawingEditorCommands& commands = FOdysseyViewportDrawingEditorCommands::Get();
	for (const TSharedPtr<const FUICommandInfo> Action : commands.Commands)
	{
		mUICommandList->UnmapAction(Action);
	}
}

const FHitResult FOdysseyViewportDrawingEditorPainter::GetHitResult(const FVector& iOrigin, const FVector& iDirection)
{
	TArray<UMeshComponent*> hoveredComponents;
	hoveredComponents.Empty(mPaintableComponents.Num());

	// Fire out a ray to see if there is a *selected* component under the mouse cursor that can be painted.
	// NOTE: We can't use a GWorld line check for this as that would ignore components that have collision disabled
	FHitResult bestTraceResult;
	{
		const FVector traceStart(iOrigin);
		const FVector traceEnd(iOrigin + iDirection * HALF_WORLD_MAX);

		for (UMeshComponent* meshComponent : mPaintableComponents)
		{
			if (meshComponent)
			{
				TSharedPtr<IMeshPaintGeometryAdapter> meshAdapter = mComponentToAdapterMap.FindChecked(meshComponent);

				// Ray trace
				FHitResult traceHitResult(1.0f);

				if (meshAdapter->LineTraceComponent(traceHitResult, traceStart, traceEnd, FCollisionQueryParams(SCENE_QUERY_STAT(Paint), true)))
				{
					// Find the closest impact
					if ((bestTraceResult.GetComponent() == nullptr) || (traceHitResult.Time < bestTraceResult.Time))
					{
						bestTraceResult = traceHitResult;
					}
				}
			}
		}
	}

	return bestTraceResult;
}

void FOdysseyViewportDrawingEditorPainter::ActorSelected(AActor* iActor)
{
    if( mPaintSettings->mTexturePaintSettings.mPaintTexture )
        CommitAllPaintedTextures();

	TInlineComponentArray<UMeshComponent*> meshComponents;
	iActor->GetComponents<UMeshComponent>(meshComponents);
	
	//Update settings
	for (UMeshComponent* meshComponent : meshComponents)
	{
		FInstanceTexturePaintSettings& Settings = AddOrRetrieveInstanceTexturePaintSettings(meshComponent);
		mPaintSettings->mTexturePaintSettings.mPaintTexture = Settings.mSelectedTexture;
	}


	// ----------------- Update Paintable Textures
	
	// CacheSelectionData();
	// CacheTexturePaintData();

	Refresh();
}

void FOdysseyViewportDrawingEditorPainter::ActorDeselected(AActor* iActor)
{
	TInlineComponentArray<UMeshComponent*> meshComponents;
	iActor->GetComponents<UMeshComponent>(meshComponents);
		
	for (UMeshComponent* meshComponent : meshComponents)
	{
        //todo: SAVE EACH TEXTURE HERE
		if (IMeshPaintGeometryAdapter* adapter = mComponentToAdapterMap.FindRef(meshComponent).Get())
		{	
			MeshPaintHelpers::ClearMeshTextureOverrides(*adapter, meshComponent);
			FInstanceTexturePaintSettings& settings = AddOrRetrieveInstanceTexturePaintSettings(meshComponent);
			settings.mSelectedTexture = mPaintSettings->mTexturePaintSettings.mPaintTexture;
		}
	}

	Refresh();
}


void FOdysseyViewportDrawingEditorPainter::AddReferencedObjects(FReferenceCollector& iCollector)
{
	iCollector.AddReferencedObject(mTexturePaintingCurrentMeshComponent);
	iCollector.AddReferencedObject(mPaintingTexture2D);
    iCollector.AddReferencedObject(mBrushRenderTargetTexture);
	iCollector.AddReferencedObjects(mPaintableComponents);
	for (TMap< UTexture2D*, FPaintTexture2DData >::TIterator It(mPaintTargetData); It; ++It)
	{
		iCollector.AddReferencedObject(It.Key());
		It.Value().AddReferencedObjects(iCollector);
	}

	FMeshPaintAdapterFactory::AddReferencedObjectsGlobals(iCollector);

	for (TMap< UMeshComponent*, TSharedPtr<IMeshPaintGeometryAdapter>>::TIterator It(mComponentToAdapterMap); It; ++It)
	{
		iCollector.AddReferencedObject(It.Key());
		It.Value()->AddReferencedObjects(iCollector);
	}
}

void FOdysseyViewportDrawingEditorPainter::FinishPainting()
{
    mBeginPosition = FVector2D(0,0);
    mFocusedViewport = nullptr;
    mController->GetData()->PaintEngine()->EndStroke();
    mIsGoingToDraw = false;
    
    //mPaintSettings->mTexturePaintSettings.mPaintTexture->MarkPackageDirty();

    /*
    CopyUTexturePixelDataIntoBlock(mController->GetData()->PaintEngine()->StrokeBlock(),mStrokeBufferTexture2D);
    TArray<::ul3::FRect> rects;
    ::ul3::FRect rect = ::ul3::FRect(0,0,1023,1023);
    rects.Add(rect);
    mController->GetData()->PaintEngine()->BlendStrokeBlockInPreviewBlock(rects);
    mController->GetData()->PaintEngine()->EndStroke();

    InvalidateTextureFromData(mController->GetData()->PaintEngine()->PreviewBlock(),mStrokeBufferTexture2D);

    for(TMap< UTexture2D*,FPaintTexture2DData >::TIterator It(mPaintTargetData); It; ++It)
    {
        FPaintTexture2DData* textureData = &It.Value();
        if(textureData)
        {
            TexturePaintHelpers::CopyTextureToRenderTargetTexture(mStrokeBufferTexture2D,textureData->PaintRenderTargetTexture,GEditor->GetEditorWorldContext().World()->FeatureLevel);
        }
    }*/

	IMeshPainter::FinishPainting();	
	FinishPaintingTextureBased();
}

bool FOdysseyViewportDrawingEditorPainter::PaintInternal(const FVector& iCameraOrigin, const TArrayView<TPair<FVector, FVector>>& iRays, EMeshPaintAction iPaintAction, float iPaintStrength)
{
    if( !mPaintSettings->mTexturePaintSettings.mPaintTexture )
        return false;

	TArray<FHitResult> paintRayResults;
	paintRayResults.AddDefaulted(iRays.Num());
	
	TMap<UMeshComponent*, TArray<int32>> hoveredComponents;

	const bool bIsPainting = (iPaintAction == EMeshPaintAction::Paint);
	// const float strengthScale = iPaintStrength;

	bool bPaintApplied = false;

	// Fire out a ray to see if there is a *selected* component under the mouse cursor that can be painted.
	// NOTE: We can't use a GWorld line check for this as that would ignore components that have collision disabled

	for (int32 i=0; i < iRays.Num(); ++i)
	{
		const FVector& rayOrigin = iRays[i].Key;
		const FVector& rayDirection = iRays[i].Value;
		FHitResult& bestTraceResult = paintRayResults[i];

		const FVector traceStart(rayOrigin);
		const FVector traceEnd(rayOrigin + rayDirection * HALF_WORLD_MAX);

		for (UMeshComponent* meshComponent : mPaintableComponents)
		{
			if (!meshComponent)
			{
				continue;
			}

			TSharedPtr<IMeshPaintGeometryAdapter>* meshAdapterPtr = mComponentToAdapterMap.Find(meshComponent);
			if (!meshAdapterPtr)
			{
				continue;
			}
			TSharedPtr<IMeshPaintGeometryAdapter> meshAdapter = *meshAdapterPtr;

			// Ray trace
			FHitResult traceHitResult(1.0f);

			if (meshAdapter->LineTraceComponent(traceHitResult, traceStart, traceEnd, FCollisionQueryParams(SCENE_QUERY_STAT(Paint), true)))
			{
				// Find the closest impact
				if ((bestTraceResult.GetComponent() == nullptr) || (traceHitResult.Time < bestTraceResult.Time))
				{
					bestTraceResult = traceHitResult;
				}
			}
		}

		bool bUsed = false;

		if (bestTraceResult.GetComponent() != nullptr)
		{
			UMeshComponent* componentToPaint = CastChecked<UMeshComponent>(bestTraceResult.GetComponent());
			hoveredComponents.FindOrAdd(componentToPaint).Add(i);
			bUsed = true;
            
            FVector2D coord;

            if( mIsGoingToDraw )
            {
                if( UGameplayStatics::FindCollisionUV( bestTraceResult, 0, coord ) )
                {
				    auto end_time = std::chrono::steady_clock::now();
				    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - mStylusLastEventTime).count();

				    if (!mIsCapturedByStylus && delta >= 500)
				    {
					    //force mouse values
					    mLastEvent = FOdysseyStrokePoint::DefaultPoint();
				    }

				    //WARNING: Be aware that we change the meaning of mLastEvent x and y here, as they were the stylus event x and y, now they are the x and y position in the texture
				    // this new meaning is then propagated to mPreviousEvent in OnStylusStateChanged()
				    mLastEvent.x = coord.X * mPaintSettings->mTexturePaintSettings.mPaintTexture->GetSurfaceWidth();
				    mLastEvent.y = coord.Y * mPaintSettings->mTexturePaintSettings.mPaintTexture->GetSurfaceHeight();
                    mLastEvent.keysDown = mKeysPressed;

				    if (IsPainting())
				    {
					    mController->GetData()->PaintEngine()->PushStroke(mLastEvent);
				    }
				    else
				    {
					    mController->GetData()->PaintEngine()->BeginStroke(mLastEvent, mPreviousEvent);
				    }
                }
            }
            else
            {
                mIsGoingToDraw = true;
            }

		}

	}

	if (hoveredComponents.Num() > 0)
	{
		if (bArePainting == false)
		{ 
			bArePainting = true;
			TimeSinceStartedPainting = 0.0f;
		}
		
		// Iterate over the selected meshes under the cursor and paint them!
		for (auto& entry : hoveredComponents)
		{
			UMeshComponent* hoveredComponent = entry.Key;
			TArray<int32>& paintRayResultIds = entry.Value;

			IMeshPaintGeometryAdapter* meshAdapter = mComponentToAdapterMap.FindRef(hoveredComponent).Get();
			if (!ensure(meshAdapter))
			{
				continue;
			}

			if ( meshAdapter->SupportsTexturePaint() )
			{
				TArray<const UTexture*> textures;
				const UTexture2D* targetTexture2D = mPaintSettings->mTexturePaintSettings.mPaintTexture;
				if (targetTexture2D)
				{
					textures.Add(targetTexture2D);

					FPaintTexture2DData* textureData = GetPaintTargetData(targetTexture2D);
					if (textureData)
					{
						textures.Add(textureData->PaintRenderTargetTexture);
					}

					TArray<FTexturePaintMeshSectionInfo> materialSections;
					TexturePaintHelpers::RetrieveMeshSectionsForTextures(hoveredComponent, 0, textures, materialSections);

					TArray<FTexturePaintTriangleInfo> trianglePaintInfoArray;
					for (int32 paintRayResultId : paintRayResultIds)
					{
						const FVector& bestTraceResultLocation = paintRayResults[paintRayResultId].Location;
						bPaintApplied |= MeshPaintHelpers::ApplyPerTrianglePaintAction(meshAdapter, iCameraOrigin, bestTraceResultLocation, mBrushSettings, FPerTrianglePaintAction::CreateRaw(this, &FOdysseyViewportDrawingEditorPainter::GatherTextureTriangles, &trianglePaintInfoArray, &materialSections, 0));
                        break;
					}

                    // Painting textures
                    if((mTexturePaintingCurrentMeshComponent != nullptr) && (mTexturePaintingCurrentMeshComponent != hoveredComponent))
                    {
                        // Mesh has changed, so finish up with our previous texture
                        FinishPaintingTextureBased();
                    }

                    if(mTexturePaintingCurrentMeshComponent == nullptr)
                    {
                        StartPaintingTextureBased(hoveredComponent,*meshAdapter);
                    }
                    
                    if(mTexturePaintingCurrentMeshComponent != nullptr)
                    {
                        for(int32 paintRayResultId : paintRayResultIds)
                        {
                            PaintTextureBased(paintRayResults[paintRayResultId],trianglePaintInfoArray,*meshAdapter);
                            //UE_LOG(LogTemp, Display, TEXT("id: %d"), paintRayResultId );
                            break;
                        }
                    }
				}
			}
		}
	}

	return bPaintApplied;
}

void FOdysseyViewportDrawingEditorPainter::GatherTextureTriangles(IMeshPaintGeometryAdapter* iAdapter, int32 iTriangleIndex, const int32 iVertexIndices[3], TArray<FTexturePaintTriangleInfo>* iTriangleInfo, TArray<FTexturePaintMeshSectionInfo>* iSectionInfos, int32 iUVChannelIndex)
{
	/** Retrieve triangles eligible for texture painting */
	bool bAdd = iSectionInfos->Num() == 0;
	for (const FTexturePaintMeshSectionInfo& sectionInfo : *iSectionInfos)
	{
		if (iTriangleIndex >= sectionInfo.FirstIndex && iTriangleIndex < sectionInfo.LastIndex)
		{
			bAdd = true;
			break;			
		}
	}

	if (bAdd)
	{
		FTexturePaintTriangleInfo info;
		iAdapter->GetVertexPosition(iVertexIndices[0], info.TriVertices[0]);
		iAdapter->GetVertexPosition(iVertexIndices[1], info.TriVertices[1]);
		iAdapter->GetVertexPosition(iVertexIndices[2], info.TriVertices[2]);
		info.TriVertices[0] = iAdapter->GetComponentToWorldMatrix().TransformPosition(info.TriVertices[0]);
		info.TriVertices[1] = iAdapter->GetComponentToWorldMatrix().TransformPosition(info.TriVertices[1]);
		info.TriVertices[2] = iAdapter->GetComponentToWorldMatrix().TransformPosition(info.TriVertices[2]);				
		iAdapter->GetTextureCoordinate(iVertexIndices[0], iUVChannelIndex, info.TriUVs[0]);
		iAdapter->GetTextureCoordinate(iVertexIndices[1], iUVChannelIndex, info.TriUVs[1]);
		iAdapter->GetTextureCoordinate(iVertexIndices[2], iUVChannelIndex, info.TriUVs[2]);
		iTriangleInfo->Add(info);
	}
}

void FOdysseyViewportDrawingEditorPainter::Reset()
{		
	//If we're painting vertex colors then propagate the painting done on LOD0 to all lower LODs. 
	//Then stop forcing the LOD level of the mesh to LOD0.
	ApplyForcedLODIndex(-1);

	// If the user has pending changes and the editor is not exiting, we want to do the commit for all the modified textures.
	if ((GetNumberOfPendingPaintChanges() > 0) && !IsEngineExitRequested())
	{
		CommitAllPaintedTextures();
	}
	else
	{
		ClearAllTextureOverrides();
	}

	mPaintTargetData.Empty();

	// Remove any existing texture targets
	mTexturePaintTargetList.Empty();
	
	mPaintableComponents.Empty();

	// Cleanup all cached 
	Cleanup();
}

TSharedPtr<IMeshPaintGeometryAdapter> FOdysseyViewportDrawingEditorPainter::GetMeshAdapterForComponent(const UMeshComponent* iComponent)
{
	return mComponentToAdapterMap.FindChecked(iComponent);
}

bool FOdysseyViewportDrawingEditorPainter::ContainsDuplicateMeshes(TArray<UMeshComponent*>& iComponents) const
{
	TArray<UObject*> objects;	
	
	bool bDuplicates = false;
	// Check for components painting to the same static/skeletal mesh
	for (UMeshComponent* component : iComponents)
	{
		UObject* object = nullptr;
		if (UStaticMeshComponent* staticMeshComponent = Cast<UStaticMeshComponent>(component))
		{
			object = staticMeshComponent->GetStaticMesh();
		}
		else if (USkeletalMeshComponent* skeletalMeshComponent = Cast<USkeletalMeshComponent>(component))
		{
			object = skeletalMeshComponent->SkeletalMesh;
		}

		if (object)
		{
			if (objects.Contains(object))
			{
				bDuplicates = true;
				break;
			}
			else
			{
				objects.Add(object);
			}
		}
	}

	return bDuplicates;
}

int32 FOdysseyViewportDrawingEditorPainter::GetMaxLODIndexToPaint() const
{
	//The maximum LOD we can paint is decide by the lowest number of LOD in the selection
	int32 numLODMin = TNumericLimits<int32>::Max();

	TArray<const UMeshComponent*> selectedComponents = GetSelectedComponents<const UMeshComponent>();

	for (const UMeshComponent* meshComponent : selectedComponents )
	{
		int32 numMeshLODs = 0;
		if (MeshPaintHelpers::TryGetNumberOfLODs(meshComponent, numMeshLODs))
		{
			ensure(numMeshLODs > 0);
			numLODMin = FMath::Min(numLODMin, numMeshLODs - 1);
		}
	}

	if (numLODMin == TNumericLimits<int32>::Max())
	{
		numLODMin = 1;
	}
	
	return numLODMin;
}

int32 FOdysseyViewportDrawingEditorPainter::GetMaxUVIndexToPaint() const
{
	if (mPaintableComponents.Num() == 1 && mPaintableComponents[0])
	{
		return MeshPaintHelpers::GetNumberOfUVs(mPaintableComponents[0], 0) - 1;
	}
	
	return 0;
}

void FOdysseyViewportDrawingEditorPainter::StartPaintingTextureBased(UMeshComponent* iMeshComponent,const IMeshPaintGeometryAdapter& iGeometryInfo)
{
    check(iMeshComponent != nullptr);
    check(mTexturePaintingCurrentMeshComponent == nullptr);
    check(mPaintingTexture2D == nullptr);

    const auto featureLevel = iMeshComponent->GetWorld()->FeatureLevel;

    UTexture2D* texture2D = mPaintSettings->mTexturePaintSettings.mPaintTexture;
    if(texture2D == nullptr)
    {
        return;
    }

    bool bStartedPainting = false;
    FPaintTexture2DData* textureData = GetPaintTargetData(texture2D);

    // Check all the materials on the mesh to see if the user texture is there
    int32 materialIndex = 0;
    UMaterialInterface* materialToCheck = iMeshComponent->GetMaterial(materialIndex);

    while(materialToCheck != nullptr)
    {
        bool bIsTextureUsed = TexturePaintHelpers::DoesMeshComponentUseTexture(iMeshComponent,texture2D);

        if(!bIsTextureUsed && (textureData != nullptr) && (textureData->PaintRenderTargetTexture != nullptr))
        {
            bIsTextureUsed = TexturePaintHelpers::DoesMeshComponentUseTexture(iMeshComponent,textureData->PaintRenderTargetTexture);
        }

        if(bIsTextureUsed && !bStartedPainting)
        {
            bool bIsSourceTextureStreamedIn = texture2D->IsFullyStreamedIn();

            if(!bIsSourceTextureStreamedIn)
            {
                // We found that this texture is used in one of the meshes materials but not fully loaded, we will
                //   attempt to fully stream in the texture before we try to do anything with it.
                texture2D->SetForceMipLevelsToBeResident(30.0f);
                texture2D->WaitForStreaming();

                // We do a quick sanity check to make sure it is streamed fully streamed in now.
                bIsSourceTextureStreamedIn = texture2D->IsFullyStreamedIn();
            }

            if(bIsSourceTextureStreamedIn)
            {
                const int32 textureWidth = texture2D->Source.GetSizeX();
                const int32 textureHeight = texture2D->Source.GetSizeY();

                if(textureData == nullptr)
                {
                    textureData = AddPaintTargetData(texture2D);
                }
                check(textureData != nullptr);

                // Create our render target texture
                if(textureData->PaintRenderTargetTexture == nullptr ||
                    textureData->PaintRenderTargetTexture->GetSurfaceWidth() != textureWidth ||
                    textureData->PaintRenderTargetTexture->GetSurfaceHeight() != textureHeight)
                {
                    textureData->PaintRenderTargetTexture = nullptr;
                    textureData->PaintRenderTargetTexture = NewObject<UTextureRenderTarget2D>(GetTransientPackage(),NAME_None,RF_Transient);
                    textureData->PaintRenderTargetTexture->bNeedsTwoCopies = true;
                    const bool bForceLinearGamma = true;
                    textureData->PaintRenderTargetTexture->InitCustomFormat(textureWidth,textureHeight,PF_A16B16G16R16,bForceLinearGamma);
                    textureData->PaintRenderTargetTexture->UpdateResourceImmediate();

                    //Duplicate the texture we are painting and store it in the transient package. This texture is a backup of the data incase we want to revert before commiting.
                    textureData->PaintingTexture2DDuplicate = (UTexture2D*)StaticDuplicateObject(texture2D,GetTransientPackage(),*FString::Printf(TEXT("%s_TEMP"),*texture2D->GetName()));
                }
                textureData->PaintRenderTargetTexture->AddressX = texture2D->AddressX;
                textureData->PaintRenderTargetTexture->AddressY = texture2D->AddressY;

                const int32 brushTargetTextureWidth = textureWidth;
                const int32 brushTargetTextureHeight = textureHeight;

                // Create the rendertarget used to store our paint delta
                if(mBrushRenderTargetTexture == nullptr ||
                    mBrushRenderTargetTexture->GetSurfaceWidth() != brushTargetTextureWidth ||
                    mBrushRenderTargetTexture->GetSurfaceHeight() != brushTargetTextureHeight)
                {
                    mBrushRenderTargetTexture = nullptr;
                    mBrushRenderTargetTexture = NewObject<UTextureRenderTarget2D>(GetTransientPackage(),NAME_None,RF_Transient);
                    const bool bForceLinearGamma = true;
                    mBrushRenderTargetTexture->ClearColor = FLinearColor::Transparent;
                    mBrushRenderTargetTexture->bNeedsTwoCopies = true;
                    mBrushRenderTargetTexture->InitCustomFormat(brushTargetTextureWidth,brushTargetTextureHeight,PF_A16B16G16R16,bForceLinearGamma);
                    mBrushRenderTargetTexture->UpdateResourceImmediate();
                    mBrushRenderTargetTexture->AddressX = textureData->PaintRenderTargetTexture->AddressX;
                    mBrushRenderTargetTexture->AddressY = textureData->PaintRenderTargetTexture->AddressY;
                }

                bStartedPainting = true;
            }
        }

        // @todo MeshPaint: Here we override the textures on the mesh with the render target.  The problem is that other meshes in the scene that use
        //    this texture do not get the override. Do we want to extend this to all other selected meshes or maybe even to all meshes in the scene?
        if(bIsTextureUsed && bStartedPainting && !textureData->PaintingMaterials.Contains(materialToCheck))
        {
            textureData->PaintingMaterials.AddUnique(materialToCheck);

            iGeometryInfo.ApplyOrRemoveTextureOverride(texture2D,textureData->PaintRenderTargetTexture);
        }

        materialIndex++;
        materialToCheck = iMeshComponent->GetMaterial(materialIndex);
    }

    if(bStartedPainting)
    {
        mTexturePaintingCurrentMeshComponent = iMeshComponent;

        check(texture2D != nullptr);
        mPaintingTexture2D = texture2D;

        // OK, now we need to make sure our render target is filled in with data
        // TexturePaintHelpers::SetupInitialRenderTargetData(textureData->PaintingTexture2D,textureData->PaintRenderTargetTexture);

		if (textureData->PaintingTexture2D->Source.IsValid())
		{
			TexturePaintHelpers::CopyTextureToRenderTargetTexture(textureData->PaintingTexture2D, textureData->PaintRenderTargetTexture, GEditor->GetEditorWorldContext().World()->FeatureLevel);
		}
		else
		{
			check(textureData->PaintingTexture2D->IsFullyStreamedIn());
			TexturePaintHelpers::CopyTextureToRenderTargetTexture(textureData->PaintingTexture2D, textureData->PaintRenderTargetTexture, GEditor->GetEditorWorldContext().World()->FeatureLevel);
		}
    }
}

void FOdysseyViewportDrawingEditorPainter::PaintTextureBased(const FHitResult& iHitResult,TArray<FTexturePaintTriangleInfo>& iInfluencedTriangles,const IMeshPaintGeometryAdapter& iGeometryInfo)
{
    // We bail early if there are no influenced triangles
    if(iInfluencedTriangles.Num() <= 0)
    {
        return;
    }

    if( mBeginPosition == FVector2D(0,0))
        mBeginPosition = FVector2D(mLastEvent.x / mPaintingTexture2D->Source.GetSizeX(), mLastEvent.y / mPaintingTexture2D->Source.GetSizeY());

    const auto featureLevel = GEditor->GetEditorWorldContext().World()->FeatureLevel;

    FPaintTexture2DData* textureData = GetPaintTargetData(mPaintingTexture2D);
    check(textureData != nullptr && textureData->PaintRenderTargetTexture != nullptr);

    textureData->bIsPaintingTexture2DModified = true;
        
    TexturePaintHelpers::CopyTextureToRenderTargetTexture(mPaintingTexture2D,textureData->PaintRenderTargetTexture,featureLevel);
    
    FlushRenderingCommands();
}

void FOdysseyViewportDrawingEditorPainter::FinishPaintingTextureBased()
{
    if(mTexturePaintingCurrentMeshComponent != nullptr)
    {
        check(mPaintingTexture2D != nullptr);

        mPaintingTexture2D = nullptr;
        mTexturePaintingCurrentMeshComponent = nullptr;
    }
}

void FOdysseyViewportDrawingEditorPainter::StartPaintingMeshBased(UMeshComponent* iMeshComponent,const IMeshPaintGeometryAdapter& iGeometryInfo)
{
    check(iMeshComponent != nullptr);
    check(mTexturePaintingCurrentMeshComponent == nullptr);
    check(mPaintingTexture2D == nullptr);

    const auto featureLevel = iMeshComponent->GetWorld()->FeatureLevel;

    UTexture2D* texture2D = mPaintSettings->mTexturePaintSettings.mPaintTexture;
    if(texture2D == nullptr)
    {
        return;
    }

    bool bStartedPainting = false;
    FPaintTexture2DData* textureData = GetPaintTargetData(texture2D);

    // Check all the materials on the mesh to see if the user texture is there
    int32 materialIndex = 0;
    UMaterialInterface* materialToCheck = iMeshComponent->GetMaterial(materialIndex);

    while(materialToCheck != nullptr)
    {
        bool bIsTextureUsed = TexturePaintHelpers::DoesMeshComponentUseTexture(iMeshComponent,texture2D);

        if(!bIsTextureUsed && (textureData != nullptr) && (textureData->PaintRenderTargetTexture != nullptr))
        {
            bIsTextureUsed = TexturePaintHelpers::DoesMeshComponentUseTexture(iMeshComponent,textureData->PaintRenderTargetTexture);
        }

        if(bIsTextureUsed && !bStartedPainting)
        {
            bool bIsSourceTextureStreamedIn = texture2D->IsFullyStreamedIn();

            if(!bIsSourceTextureStreamedIn)
            {
                // We found that this texture is used in one of the meshes materials but not fully loaded, we will
                //   attempt to fully stream in the texture before we try to do anything with it.
                texture2D->SetForceMipLevelsToBeResident(30.0f);
                texture2D->WaitForStreaming();

                // We do a quick sanity check to make sure it is streamed fully streamed in now.
                bIsSourceTextureStreamedIn = texture2D->IsFullyStreamedIn();
            }

            if(bIsSourceTextureStreamedIn)
            {
                const int32 textureWidth = texture2D->Source.GetSizeX();
                const int32 textureHeight = texture2D->Source.GetSizeY();

                if(textureData == nullptr)
                {
                    textureData = AddPaintTargetData(texture2D);
                }
                check(textureData != nullptr);

                // Create our render target texture
                if(textureData->PaintRenderTargetTexture == nullptr ||
                    textureData->PaintRenderTargetTexture->GetSurfaceWidth() != textureWidth ||
                    textureData->PaintRenderTargetTexture->GetSurfaceHeight() != textureHeight)
                {
                    textureData->PaintRenderTargetTexture = nullptr;
                    textureData->PaintRenderTargetTexture = NewObject<UTextureRenderTarget2D>(GetTransientPackage(),NAME_None,RF_Transient);
                    textureData->PaintRenderTargetTexture->bNeedsTwoCopies = true;
                    const bool bForceLinearGamma = true;
                    textureData->PaintRenderTargetTexture->InitCustomFormat(textureWidth,textureHeight,PF_A16B16G16R16,bForceLinearGamma);
                    textureData->PaintRenderTargetTexture->UpdateResourceImmediate();

                    //Duplicate the texture we are painting and store it in the transient package. This texture is a backup of the data incase we want to revert before commiting.
                    textureData->PaintingTexture2DDuplicate = (UTexture2D*)StaticDuplicateObject(texture2D,GetTransientPackage(),*FString::Printf(TEXT("%s_TEMP"),*texture2D->GetName()));
                }
                textureData->PaintRenderTargetTexture->AddressX = texture2D->AddressX;
                textureData->PaintRenderTargetTexture->AddressY = texture2D->AddressY;

                const int32 brushTargetTextureWidth = textureWidth;
                const int32 brushTargetTextureHeight = textureHeight;

                // Create the rendertarget used to store our paint delta
                if(mBrushRenderTargetTexture == nullptr ||
                    mBrushRenderTargetTexture->GetSurfaceWidth() != brushTargetTextureWidth ||
                    mBrushRenderTargetTexture->GetSurfaceHeight() != brushTargetTextureHeight)
                {
                    mBrushRenderTargetTexture = nullptr;
                    mBrushRenderTargetTexture = NewObject<UTextureRenderTarget2D>(GetTransientPackage(),NAME_None,RF_Transient);
                    const bool bForceLinearGamma = true;
                    mBrushRenderTargetTexture->ClearColor = FLinearColor::Transparent;
                    mBrushRenderTargetTexture->bNeedsTwoCopies = true;
                    mBrushRenderTargetTexture->InitCustomFormat(brushTargetTextureWidth,brushTargetTextureHeight,PF_A16B16G16R16,bForceLinearGamma);
                    mBrushRenderTargetTexture->UpdateResourceImmediate();
                    mBrushRenderTargetTexture->AddressX = textureData->PaintRenderTargetTexture->AddressX;
                    mBrushRenderTargetTexture->AddressY = textureData->PaintRenderTargetTexture->AddressY;
                }

                bStartedPainting = true;
            }
        }

        // @todo MeshPaint: Here we override the textures on the mesh with the render target.  The problem is that other meshes in the scene that use
        //    this texture do not get the override. Do we want to extend this to all other selected meshes or maybe even to all meshes in the scene?
        if(bIsTextureUsed && bStartedPainting && !textureData->PaintingMaterials.Contains(materialToCheck))
        {
            textureData->PaintingMaterials.AddUnique(materialToCheck);

            iGeometryInfo.ApplyOrRemoveTextureOverride(texture2D,textureData->PaintRenderTargetTexture);
        }

        materialIndex++;
        materialToCheck = iMeshComponent->GetMaterial(materialIndex);
    }

    if(bStartedPainting)
    {
        mTexturePaintingCurrentMeshComponent = iMeshComponent;

        check(texture2D != nullptr);
        mPaintingTexture2D = texture2D;

        mStrokeBufferTexture2D = NewObject<UTexture2D>(GetTransientPackage(),FName(),RF_Transient);
        InitTextureWithBlockData(mController->GetData()->PaintEngine()->PreviewBlock(),mStrokeBufferTexture2D,TSF_BGRA8);
        mStrokeBufferTexture2D->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
        mStrokeBufferTexture2D->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
        mStrokeBufferTexture2D->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;
        mStrokeBufferTexture2D->UpdateResource();
        mStrokeBufferTexture2D->PostEditChange();

        mStrokeBufferTexture3D = NewObject<UTexture2D>(GetTransientPackage(),FName(),RF_Transient);
        InitTextureWithBlockData(mController->GetData()->PaintEngine()->PreviewBlock(),mStrokeBufferTexture3D,TSF_BGRA8);
        mStrokeBufferTexture3D->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
        mStrokeBufferTexture3D->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
        mStrokeBufferTexture3D->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;
        mStrokeBufferTexture3D->UpdateResource();
        mStrokeBufferTexture3D->PostEditChange();

        // OK, now we need to make sure our render target is filled in with data
        //TexturePaintHelpers::SetupInitialRenderTargetData(textureData->PaintingTexture2D,textureData->PaintRenderTargetTexture);


		if (textureData->PaintingTexture2D->Source.IsValid())
		{
			TexturePaintHelpers::CopyTextureToRenderTargetTexture(textureData->PaintingTexture2D, textureData->PaintRenderTargetTexture, GEditor->GetEditorWorldContext().World()->FeatureLevel);
		}
		else
		{
			check(textureData->PaintingTexture2D->IsFullyStreamedIn());
			TexturePaintHelpers::CopyTextureToRenderTargetTexture(textureData->PaintingTexture2D, textureData->PaintRenderTargetTexture, GEditor->GetEditorWorldContext().World()->FeatureLevel);
		}
    }
}

void FOdysseyViewportDrawingEditorPainter::PaintMeshBased(const FHitResult& iHitResult,TArray<FTexturePaintTriangleInfo>& iInfluencedTriangles,const IMeshPaintGeometryAdapter& iGeometryInfo)
{
    // We bail early if there are no influenced triangles
    if(iInfluencedTriangles.Num() <= 0)
    {
        return;
    }

    if( mBeginPosition == FVector2D(0,0))
        mBeginPosition = FVector2D(mLastEvent.x / mPaintingTexture2D->Source.GetSizeX(), mLastEvent.y / mPaintingTexture2D->Source.GetSizeY());

    //UE_LOG(LogTemp, Display, TEXT("triangles: %d"), iInfluencedTriangles.Num() );

    const auto featureLevel = GEditor->GetEditorWorldContext().World()->FeatureLevel;

    FPaintTexture2DData* textureData = GetPaintTargetData(mPaintingTexture2D);
    check(textureData != nullptr && textureData->PaintRenderTargetTexture != nullptr);

    // Copy the current image to the brush rendertarget texture.
    {
        check(mBrushRenderTargetTexture != nullptr);
        TexturePaintHelpers::CopyTextureToRenderTargetTexture(mPaintingTexture2D,mBrushRenderTargetTexture,featureLevel);
    }

    const bool bEnableSeamPainting = false;

    FVector BrushXAxis,BrushYAxis;
    iHitResult.Normal.FindBestAxisVectors(BrushXAxis,BrushYAxis);
    const FMatrix worldToBrushMatrix = FMatrix(BrushXAxis, BrushYAxis, iHitResult.Normal, iHitResult.Location).InverseFast();

    // Grab the actual render target resource from the textures.  Note that we're absolutely NOT ALLOWED to
    // dereference these pointers.  We're just passing them along to other functions that will use them on the render
    // thread.  The only thing we're allowed to do is check to see if they are nullptr or not.
    FTextureRenderTargetResource* brushRenderTargetResource = mBrushRenderTargetTexture->GameThread_GetRenderTargetResource();
    //mStrokeBufferTexture3D->Resource->TextureRHI = brushRenderTargetResource->GetRenderTargetTexture();

    check(brushRenderTargetResource != nullptr);

    // Create a canvas for the brush render target.
    FCanvas brushPaintCanvas(brushRenderTargetResource,nullptr,0,0,0,featureLevel);

    InvalidateTextureFromData(mController->GetData()->PaintEngine()->StrokeBlock(), mStrokeBufferTexture2D);

    // Parameters for brush paint
    TRefCountPtr< FOdysseyMeshPaintBatchedElementParameters > meshPaintBatchedElementParameters(new FOdysseyMeshPaintBatchedElementParameters());
    {
        meshPaintBatchedElementParameters->ShaderParams.Stroke2D = mStrokeBufferTexture2D;
        meshPaintBatchedElementParameters->ShaderParams.WorldToBrushMatrix = worldToBrushMatrix;
        meshPaintBatchedElementParameters->ShaderParams.TextureHitPoint = mBeginPosition;
    }

    FBatchedElements* brushPaintBatchedElements = brushPaintCanvas.GetBatchedElements(FCanvas::ET_Triangle,meshPaintBatchedElementParameters,nullptr,SE_BLEND_Opaque);
    brushPaintBatchedElements->AddReserveVertices(iInfluencedTriangles.Num() * 3);
    brushPaintBatchedElements->AddReserveTriangles(iInfluencedTriangles.Num(),nullptr,SE_BLEND_Opaque);

    FHitProxyId brushPaintHitProxyId = brushPaintCanvas.GetHitProxyId();

    // Process the influenced triangles - storing off a large list is much slower than processing in a single loop
    for(int32 curIndex = 0; curIndex < iInfluencedTriangles.Num(); ++curIndex)
    {
        FTexturePaintTriangleInfo& curTriangle = iInfluencedTriangles[curIndex];

        FVector2D uvMin(99999.9f,99999.9f);
        FVector2D uvMax(-99999.9f,-99999.9f);

        // Transform the triangle and update the UV bounds
        for(int32 triVertexNum = 0; triVertexNum < 3; ++triVertexNum)
        {
            // Update bounds
            float u = curTriangle.TriUVs[triVertexNum].X;
            float v = curTriangle.TriUVs[triVertexNum].Y;

            if(u < uvMin.X)
            {
                uvMin.X = u;
            }
            if(u > uvMax.X)
            {
                uvMax.X = u;
            }
            if(v < uvMin.Y)
            {
                uvMin.Y = v;
            }
            if(v > uvMax.Y)
            {
                uvMax.Y = v;
            }
        }

        // If the triangle lies entirely outside of the 0.0-1.0 range, we'll transpose it back
        FVector2D uvOffset(0.0f,0.0f);
        if(uvMax.X > 1.0f)
        {
            uvOffset.X = -FMath::FloorToFloat(uvMin.X);
        } else if(uvMin.X < 0.0f)
        {
            uvOffset.X = 1.0f + FMath::FloorToFloat(-uvMax.X);
        }

        if(uvMax.Y > 1.0f)
        {
            uvOffset.Y = -FMath::FloorToFloat(uvMin.Y);
        } else if(uvMin.Y < 0.0f)
        {
            uvOffset.Y = 1.0f + FMath::FloorToFloat(-uvMax.Y);
        }

        // Note that we "wrap" the texture coordinates here to handle the case where the user
        // is painting on a tiling texture, or with the UVs out of bounds.  Ideally all of the
        // UVs would be in the 0.0 - 1.0 range but sometimes content isn't setup that way.
        // @todo MeshPaint: Handle triangles that cross the 0.0-1.0 UV boundary?
        for(int32 triVertexNum = 0; triVertexNum < 3; ++triVertexNum)
        {
            curTriangle.TriUVs[triVertexNum].X += uvOffset.X;
            curTriangle.TriUVs[triVertexNum].Y += uvOffset.Y;

            // @todo: Need any half-texel offset adjustments here? Some info about offsets and MSAA here: http://drilian.com/2008/11/25/understanding-half-pixel-and-half-texel-offsets/
            // @todo: MeshPaint: Screen-space texture coords: http://diaryofagraphicsprogrammer.blogspot.com/2008/09/calculating-screen-space-texture.html
            curTriangle.TrianglePoints[triVertexNum].X = curTriangle.TriUVs[triVertexNum].X * textureData->PaintRenderTargetTexture->GetSurfaceWidth();
            curTriangle.TrianglePoints[triVertexNum].Y = curTriangle.TriUVs[triVertexNum].Y * textureData->PaintRenderTargetTexture->GetSurfaceHeight();
        }

        // Vertex positions
        FVector4 vert0(curTriangle.TrianglePoints[0].X,curTriangle.TrianglePoints[0].Y,0,1);
        FVector4 vert1(curTriangle.TrianglePoints[1].X,curTriangle.TrianglePoints[1].Y,0,1);
        FVector4 vert2(curTriangle.TrianglePoints[2].X,curTriangle.TrianglePoints[2].Y,0,1);

        //UE_LOG(LogTemp,Display,TEXT("------------------------------------"));
        /*UE_LOG(LogTemp,Display,TEXT("vert0x: %lf, vert0y: %lf,vert0z: %lf, vert0w: %lf"),vert0.X,vert0.Y,vert0.Z,vert0.W);
        UE_LOG(LogTemp,Display, TEXT("vert1x: %lf, vert1y: %lf,vert1z: %lf, vert1w: %lf"), vert1.X, vert1.Y, vert1.Z, vert1.W);
        UE_LOG(LogTemp,Display,TEXT("vert2x: %lf, vert2y: %lf,vert2z: %lf, vert2w: %lf"),vert2.X,vert2.Y,vert2.Z,vert2.W);*/


        // Vertex color
        FLinearColor col0(curTriangle.TriVertices[0].X,curTriangle.TriVertices[0].Y,curTriangle.TriVertices[0].Z);
        FLinearColor col1(curTriangle.TriVertices[1].X,curTriangle.TriVertices[1].Y,curTriangle.TriVertices[1].Z);
        FLinearColor col2(curTriangle.TriVertices[2].X,curTriangle.TriVertices[2].Y,curTriangle.TriVertices[2].Z);
        /*UE_LOG(LogTemp,Display,TEXT("col0 R: %lf, col0 G: %lf,col0 B: %lf, col0 A: %lf"),col0.R,col0.G,col0.B,col0.A);
        UE_LOG(LogTemp,Display,TEXT("col1 R: %lf, col1 G: %lf,col1 B: %lf, col1 A: %lf"),col1.R,col1.G,col1.B,col1.A);
        UE_LOG(LogTemp,Display,TEXT("col2 R: %lf, col2 G: %lf,col2 B: %lf, col2 A: %lf"),col2.R,col2.G,col2.B,col2.A);*/


        // Brush Paint triangle
        {
            int32 v0 = brushPaintBatchedElements->AddVertex(vert0,curTriangle.TriUVs[0],col0,brushPaintHitProxyId);
            int32 v1 = brushPaintBatchedElements->AddVertex(vert1,curTriangle.TriUVs[1],col1,brushPaintHitProxyId);
            int32 v2 = brushPaintBatchedElements->AddVertex(vert2,curTriangle.TriUVs[2],col2,brushPaintHitProxyId);
            /*UE_LOG(LogTemp,Display,TEXT("curTriangle.TriUVs[0]x: %lf, curTriangle.TriUVs[0]y: %lf"),curTriangle.TriUVs[0].X,curTriangle.TriUVs[0].Y);
            UE_LOG(LogTemp,Display,TEXT("curTriangle.TriUVs[1]x: %lf, curTriangle.TriUVs[1]y: %lf"),curTriangle.TriUVs[1].X,curTriangle.TriUVs[1].Y);
            UE_LOG(LogTemp,Display,TEXT("curTriangle.TriUVs[2]x: %lf, curTriangle.TriUVs[2]y: %lf"),curTriangle.TriUVs[2].X,curTriangle.TriUVs[2].Y);*/

            brushPaintBatchedElements->AddTriangle(v0,v1,v2,meshPaintBatchedElementParameters,SE_BLEND_Opaque);
        }
    }

    // Tell the rendering thread to draw any remaining batched elements
    {
        brushPaintCanvas.Flush_GameThread(true);

        textureData->bIsPaintingTexture2DModified = true;
    }

    {
        ENQUEUE_RENDER_COMMAND(UpdateMeshPaintRTCommand1)(
            [brushRenderTargetResource](FRHICommandListImmediate& RHICmdList)
        {
            // Copy (resolve) the rendered image from the frame buffer to its render target texture
            RHICmdList.CopyToResolveTarget(
                brushRenderTargetResource->GetRenderTargetTexture(),	// Source texture
                brushRenderTargetResource->TextureRHI,
                FResolveParams());										// Resolve parameters

        });
    }

    //mBrushRenderTargetTexture->bGPUSharedFlag = true;

    //brushRenderTargetResource->ReadPixelsPtr((FColor*)(mController->GetData()->PaintEngine()->StrokeBlock3D()->GetArray().GetData()));
    //mBrushRenderTargetTexture->UpdateTexture2D(mStrokeBufferTexture3D, ETextureSourceFormat::TSF_RGBA16 );
    //CopyURenderTargetPixelDataIntoBlock(mController->GetData()->PaintEngine()->StrokeBlock3D()->GetArray().GetData(), mBrushRenderTargetTexture);
    //CopyUTexturePixelDataIntoBlock(mController->GetData()->PaintEngine()->StrokeBlock3D(), mStrokeBufferTexture2D);
    /*TArray<::ul3::FRect> rects;
    ::ul3::FRect rect = ::ul3::FRect(0,0,1023,1023);
    rects.Add(rect);
    mController->GetData()->PaintEngine()->BlendStrokeBlockInPreviewBlock(rects);
    mController->OnPaintEngineStrokeChanged(rects);*/

    //mController->GetData()->PaintEngine()->UpdateTmpTileMapFromTriangles( iInfluencedTriangles );

    //CopyUTexturePixelDataIntoBlock(mController->GetData()->PaintEngine()->StrokeBlock3D(), mPaintingTexture2D);
    //mController->GetData()->PaintEngine()->Tick();
    
    TexturePaintHelpers::CopyTextureToRenderTargetTexture(mPaintingTexture2D,textureData->PaintRenderTargetTexture,featureLevel);
    
    FlushRenderingCommands();
}

void FOdysseyViewportDrawingEditorPainter::FinishPaintingMeshBased()
{
    if(mTexturePaintingCurrentMeshComponent != nullptr)
    {
        check(mPaintingTexture2D != nullptr);

        FPaintTexture2DData* textureData = GetPaintTargetData(mPaintingTexture2D);
        check(textureData);

        // Commit to the texture source art but don't do any compression, compression is saved for the CommitAllPaintedTextures function.
        if(textureData->bIsPaintingTexture2DModified == true)
        {
            const int32 texWidth = textureData->PaintRenderTargetTexture->SizeX;
            const int32 texHeight = textureData->PaintRenderTargetTexture->SizeY;
            TArray< FColor > texturePixels;
            texturePixels.AddUninitialized(texWidth * texHeight);

            FlushRenderingCommands();
            // NOTE: You are normally not allowed to dereference this pointer on the game thread! Normally you can only pass the pointer around and
            //  check for NULLness.  We do it in this context, however, and it is only ok because this does not happen every frame and we make sure to flush the
            //  rendering thread.
            FTextureRenderTargetResource* renderTargetResource = textureData->PaintRenderTargetTexture->GameThread_GetRenderTargetResource();
            check(renderTargetResource != nullptr);
            renderTargetResource->ReadPixels(texturePixels);

            {
                FScopedTransaction transaction(LOCTEXT("MeshPaintMode_TexturePaint_Transaction","Texture Paint"));

                // For undo
                textureData->PaintingTexture2D->SetFlags(RF_Transactional);
                textureData->PaintingTexture2D->Modify();

                // Store source art
                FColor* colors = (FColor*)textureData->PaintingTexture2D->Source.LockMip(0);
                check(textureData->PaintingTexture2D->Source.CalcMipSize(0) == texturePixels.Num() * sizeof(FColor));
                FMemory::Memcpy(colors,texturePixels.GetData(),texturePixels.Num() * sizeof(FColor));
                textureData->PaintingTexture2D->Source.UnlockMip(0);

                // If render target gamma used was 1.0 then disable SRGB for the static texture
                textureData->PaintingTexture2D->SRGB = FMath::Abs(renderTargetResource->GetDisplayGamma() - 1.0f) >= KINDA_SMALL_NUMBER;

                textureData->PaintingTexture2D->bHasBeenPaintedInEditor = true;
            }
        }

        mPaintingTexture2D = nullptr;
        mTexturePaintingCurrentMeshComponent = nullptr;
    }
}


FPaintTexture2DData* FOdysseyViewportDrawingEditorPainter::GetPaintTargetData(const UTexture2D* iTexture)
{
	checkf(iTexture != nullptr, TEXT("Invalid Texture ptr"));
	/** Retrieve target paint data for the given texture */
	FPaintTexture2DData* textureData = mPaintTargetData.Find(iTexture);
	return textureData;
}

FPaintTexture2DData* FOdysseyViewportDrawingEditorPainter::AddPaintTargetData(UTexture2D* iTexture)
{
	checkf(iTexture != nullptr, TEXT("Invalid Texture ptr"));

	/** Only create new target if we haven't gotten one already  */
	FPaintTexture2DData* textureData = GetPaintTargetData(iTexture);
	if (textureData == nullptr)
	{
		// If we didn't find data associated with this texture we create a new entry and return a reference to it.
		//   Note: This reference is only valid until the next change to any key in the map.
		textureData = &mPaintTargetData.Add(iTexture, FPaintTexture2DData(iTexture, false));
	}
	return textureData;
}

UTexture2D* FOdysseyViewportDrawingEditorPainter::GetOriginalTextureFromRenderTarget(UTextureRenderTarget2D* iTexture)
{
	checkf(iTexture != nullptr, TEXT("Invalid Texture ptr"));

	UTexture2D* texture2D = nullptr;

	// We loop through our data set and see if we can find this rendertarget.  If we can, then we add the corresponding UTexture2D to the UI list.
	for (TMap< UTexture2D*, FPaintTexture2DData >::TIterator It(mPaintTargetData); It; ++It)
	{
		FPaintTexture2DData* textureData = &It.Value();

		if (textureData->PaintRenderTargetTexture != nullptr &&
			textureData->PaintRenderTargetTexture == iTexture)
		{
			texture2D = textureData->PaintingTexture2D;

			// We found the matching texture so we can stop searching
			break;
		}
	}

	return texture2D;
}

void FOdysseyViewportDrawingEditorPainter::CommitAllPaintedTextures()
{
	if (mPaintTargetData.Num() > 0)
	{
		FScopedTransaction transaction(LOCTEXT("MeshPaintMode_TexturePaint_Transaction", "Texture Paint"));

		GWarn->BeginSlowTask(LOCTEXT("BeginMeshPaintMode_TexturePaint_CommitTask", "Committing Texture Paint Changes"), true);

		int32 curStep = 1;
		int32 totalSteps = GetNumberOfPendingPaintChanges();

		for (TMap< UTexture2D*, FPaintTexture2DData >::TIterator It(mPaintTargetData); It; ++It)
		{
			FPaintTexture2DData* textureData = &It.Value();

			// Commit the texture
			if (textureData->bIsPaintingTexture2DModified == true)
			{
				GWarn->StatusUpdate(curStep++, totalSteps, FText::Format(LOCTEXT("MeshPaintMode_TexturePaint_CommitStatus", "Committing Texture Paint Changes: {0}"), FText::FromName(textureData->PaintingTexture2D->GetFName())));

				const int32 texWidth = textureData->PaintRenderTargetTexture->SizeX;
				const int32 texHeight = textureData->PaintRenderTargetTexture->SizeY;
				TArray< FColor > texturePixels;
				texturePixels.AddUninitialized(texWidth * texHeight);

				// Copy the contents of the remote texture to system memory
				// NOTE: OutRawImageData must be a preallocated buffer!

				FlushRenderingCommands();
				// NOTE: You are normally not allowed to dereference this pointer on the game thread! Normally you can only pass the pointer around and
				//  check for NULLness.  We do it in this context, however, and it is only ok because this does not happen every frame and we make sure to flush the
				//  rendering thread.
				FTextureRenderTargetResource* renderTargetResource = textureData->PaintRenderTargetTexture->GameThread_GetRenderTargetResource();
				check(renderTargetResource != nullptr);
				renderTargetResource->ReadPixels(texturePixels); //RGBA

				{
					// For undo
					textureData->PaintingTexture2D->SetFlags(RF_Transactional);
					textureData->PaintingTexture2D->Modify();

					// Store source art
					FColor* colors = (FColor*)textureData->PaintingTexture2D->Source.LockMip(0);

					::ul3::FBlock* srcblock = new ::ul3::FBlock((::ul3::tByte*)texturePixels.GetData(), textureData->PaintingTexture2D->Source.GetSizeX(), textureData->PaintingTexture2D->Source.GetSizeY(), ULIS3_FORMAT_BGRA8);
					::ul3::FBlock* dstblock = new ::ul3::FBlock((::ul3::tByte*)colors, textureData->PaintingTexture2D->Source.GetSizeX(), textureData->PaintingTexture2D->Source.GetSizeY(), ULISFormatForUE4TextureSourceFormat(textureData->PaintingTexture2D->Source.GetFormat()));
					

					delete srcblock; srcblock = nullptr;
					delete dstblock; dstblock = nullptr;
					// check(textureData->PaintingTexture2D->Source.CalcMipSize(0) == texturePixels.Num() * sizeof(FColor));
					//FMemory::Memcpy(colors, texturePixels.GetData(), texturePixels.Num() * sizeof(FColor));
					textureData->PaintingTexture2D->Source.UnlockMip(0);

					// If render target gamma used was 1.0 then disable SRGB for the static texture
					// @todo MeshPaint: We are not allowed to dereference the RenderTargetResource pointer, figure out why we need this when the GetDisplayGamma() function is hard coded to return 2.2.
					textureData->PaintingTexture2D->SRGB = FMath::Abs(renderTargetResource->GetDisplayGamma() - 1.0f) >= KINDA_SMALL_NUMBER;

					textureData->PaintingTexture2D->bHasBeenPaintedInEditor = true;

					// Update the texture (generate mips, compress if needed)
					textureData->PaintingTexture2D->PostEditChange();

					textureData->bIsPaintingTexture2DModified = false;

					// Reduplicate the duplicate so that if we cancel our future changes, it will restore to how the texture looked at this point.
					textureData->PaintingTexture2DDuplicate = (UTexture2D*)StaticDuplicateObject(textureData->PaintingTexture2D, GetTransientPackage(), *FString::Printf(TEXT("%s_TEMP"), *textureData->PaintingTexture2D->GetName()));

				}
			}
		}

		ClearAllTextureOverrides();

		GWarn->EndSlowTask();
	}
}

void FOdysseyViewportDrawingEditorPainter::ClearAllTextureOverrides()
{
	/** Remove all texture overrides which are currently stored and active */
	for (TMap< UTexture2D*, FPaintTexture2DData >::TIterator it(mPaintTargetData); it; ++it)
	{
		FPaintTexture2DData* textureData = &it.Value();

		for (int32 materialIndex = 0; materialIndex < textureData->PaintingMaterials.Num(); materialIndex++)
		{
			UMaterialInterface* paintingMaterialInterface = textureData->PaintingMaterials[materialIndex];
			paintingMaterialInterface->OverrideTexture(textureData->PaintingTexture2D, nullptr, GEditor->GetEditorWorldContext().World()->FeatureLevel);//findme
		}

		textureData->PaintingMaterials.Empty();
	}
}

void FOdysseyViewportDrawingEditorPainter::SetAllTextureOverrides(const IMeshPaintGeometryAdapter& iGeometryInfo, UMeshComponent* iMeshComponent)
{
	if (iMeshComponent != nullptr)
	{
		TArray<UTexture*> usedTextures;
		iMeshComponent->GetUsedTextures(/*out*/ usedTextures, EMaterialQualityLevel::High);

		for (UTexture* texture : usedTextures)
		{
			if (UTexture2D* texture2D = Cast<UTexture2D>(texture))
			{
				if (FPaintTexture2DData* textureData = GetPaintTargetData(texture2D))
				{
					iGeometryInfo.ApplyOrRemoveTextureOverride(texture2D, textureData->PaintRenderTargetTexture);
				}
			}
		}
	}
}

void FOdysseyViewportDrawingEditorPainter::SetSpecificTextureOverrideForMesh(const IMeshPaintGeometryAdapter& iGeometryInfo, UTexture2D* iTexture)
{
	// If there is texture data, that means we have an override ready, so set it. 
	// If there is no data, then remove the override so we can at least see the texture without the changes to the other texture.
	// This is important because overrides are shared between material instances with the same parent. We want to disable a override in place,
	// making the action more comprehensive to the user.
	FPaintTexture2DData* textureData = GetPaintTargetData(iTexture);
	UTextureRenderTarget2D* textureForOverrideOrNull = ((textureData != nullptr) && (textureData->PaintingMaterials.Num() > 0)) ? textureData->PaintRenderTargetTexture : nullptr;

	iGeometryInfo.ApplyOrRemoveTextureOverride(iTexture, textureForOverrideOrNull);
}

void FOdysseyViewportDrawingEditorPainter::RestoreRenderTargets()
{
	mDoRestoreRenTargets = true;
}

int32 FOdysseyViewportDrawingEditorPainter::GetNumberOfPendingPaintChanges() const
{
	int32 result = 0;
	for (TMap< UTexture2D*, FPaintTexture2DData >::TConstIterator it(mPaintTargetData); it; ++it)
	{
		const FPaintTexture2DData* textureData = &it.Value();

		// Commit the texture
		if (textureData->bIsPaintingTexture2DModified == true)
		{
			result++;
		}
	}
	return result;
}

void FOdysseyViewportDrawingEditorPainter::ApplyForcedLODIndex(int32 iForcedLODIndex)
{	
	for (UMeshComponent* selectedComponent : mPaintableComponents)
	{
		if (selectedComponent)
		{
			MeshPaintHelpers::ForceRenderMeshLOD(selectedComponent, iForcedLODIndex);
		}
	}
}

void FOdysseyViewportDrawingEditorPainter::UpdatePaintTargets(UObject* iObject, struct FPropertyChangedEvent& iPropertyChangedEvent)
{
	AActor* actor = Cast<AActor>(iObject);
	if (iPropertyChangedEvent.Property && 
		iPropertyChangedEvent.Property->GetName() == USceneComponent::GetVisiblePropertyName().ToString())
	{
		Refresh();
	}
}


void FOdysseyViewportDrawingEditorPainter::SaveModifiedTextures()
{
	UTexture2D* selectedTexture = mPaintSettings->mTexturePaintSettings.mPaintTexture;

	if (nullptr != selectedTexture)
	{
		TArray<UObject*> texturesToSaveArray;
		texturesToSaveArray.Add(selectedTexture);
		UPackageTools::SavePackagesForObjects(texturesToSaveArray);
	}
}

void FOdysseyViewportDrawingEditorPainter::Refresh()
{
	// Ensure that we call OnRemoved while adapter/components are still valid
	mPaintableComponents.Empty();
	Cleanup();

	mDoRefreshCachedData = true;
}

void FOdysseyViewportDrawingEditorPainter::Cleanup()
{
	for (auto meshAdapterPair : mComponentToAdapterMap)
	{
		meshAdapterPair.Value->OnRemoved();
	}
	mComponentToAdapterMap.Empty();
	FMeshPaintAdapterFactory::CleanupGlobals();
}

void FOdysseyViewportDrawingEditorPainter::Tick(FEditorViewportClient* iViewportClient, float iDeltaTime)
{
	IMeshPainter::Tick(iViewportClient, iDeltaTime);
	
	if (mDoRefreshCachedData)
	{
		mDoRefreshCachedData = false;
		CacheSelectionData();
		CacheTexturePaintData();

		mDoRestoreRenTargets = true;
	}

	// Will set the texture override up for the selected texture, important for the drop down combo-list and selecting between material instances.
	if (mPaintableComponents.Num() == 1 && mPaintableComponents[0] && mPaintSettings->mTexturePaintSettings.mPaintTexture)
	{
		for (UMeshComponent* meshComponent : mPaintableComponents)
		{
			TSharedPtr<IMeshPaintGeometryAdapter>* meshAdapter = mComponentToAdapterMap.Find(meshComponent);
			if (meshAdapter)
			{
				SetSpecificTextureOverrideForMesh(*meshAdapter->Get(), mPaintSettings->mTexturePaintSettings.mPaintTexture);
			}
		}
	}

	// If this is true probably someone force deleted a texture out from under us
	bool bBadAssetFound = false;

	if (mDoRestoreRenTargets)
	{
		if (mPaintingTexture2D == nullptr)
		{
			for (TMap< UTexture2D*, FPaintTexture2DData >::TIterator it(mPaintTargetData); it; ++it)
			{
				
				if (!it.Key())
				{
					bBadAssetFound = true;
					break;
				}

				FPaintTexture2DData* textureData = &it.Value();
				if (textureData->PaintRenderTargetTexture != nullptr)
				{

					bool bIsSourceTextureStreamedIn = textureData->PaintingTexture2D->IsFullyStreamedIn();

					if (!bIsSourceTextureStreamedIn)
					{
						//   Make sure it is fully streamed in before we try to do anything with it.
						textureData->PaintingTexture2D->SetForceMipLevelsToBeResident(30.0f);
						textureData->PaintingTexture2D->WaitForStreaming();
					}

					//Use the duplicate texture here because as we modify the texture and do undo's, it will be different over the original.
					// TexturePaintHelpers::SetupInitialRenderTargetData(textureData->PaintingTexture2D, textureData->PaintRenderTargetTexture);
					if (textureData->PaintingTexture2D->Source.IsValid())
					{
						TexturePaintHelpers::CopyTextureToRenderTargetTexture(textureData->PaintingTexture2D, textureData->PaintRenderTargetTexture, GEditor->GetEditorWorldContext().World()->FeatureLevel);
					}
					else
					{
						check(textureData->PaintingTexture2D->IsFullyStreamedIn());
						TexturePaintHelpers::CopyTextureToRenderTargetTexture(textureData->PaintingTexture2D, textureData->PaintRenderTargetTexture, GEditor->GetEditorWorldContext().World()->FeatureLevel);
					}

				}
			}
		}
		// We attempted a restore of the rendertargets so go ahead and clear the flag
		mDoRestoreRenTargets = false;
	}

	if (bBadAssetFound)
	{
		mPaintTargetData.Empty();
	}

    if( mController->GetData()->Texture() )
    {
        mController->GetData()->PaintEngine()->Tick();
        FPaintTexture2DData* textureData = GetPaintTargetData(mController->GetData()->Texture());
        if( textureData )
            TexturePaintHelpers::CopyTextureToRenderTargetTexture(mController->GetData()->Texture(),textureData->PaintRenderTargetTexture, GEditor->GetEditorWorldContext().World()->FeatureLevel);
    }
}

bool FOdysseyViewportDrawingEditorPainter::InputKey(FEditorViewportClient* InViewportClient,FViewport* InViewport,FKey InKey,EInputEvent InEvent)
{
    if(InEvent == EInputEvent::IE_Pressed)
    {
        mKeysPressed.Add(InKey);
    } 
    else if(InEvent == EInputEvent::IE_Released)
    {
        mKeysPressed.Remove(InKey);
    }

    mFocusedViewport = InViewport;

    if(InEvent == EInputEvent::IE_Pressed && InKey == EKeys::LeftMouseButton)
    {
        mTemporaryViewportClient->SetViewport(mFocusedViewport);
        mTemporaryViewportClient->SetViewportClient(mFocusedViewport->GetClient());
        mFocusedViewport->SetViewportClient(mTemporaryViewportClient);
    }

    if(mIsCapturedByStylus)
        return true;

    bool bHandled = false;

    const bool bIsLeftButtonDown = (InKey == EKeys::LeftMouseButton && InEvent != IE_Released) || InViewport->KeyState(EKeys::LeftMouseButton);
    const bool bIsRightButtonDown = (InKey == EKeys::RightMouseButton && InEvent != IE_Released) || InViewport->KeyState(EKeys::RightMouseButton);
    const bool bIsCtrlDown = ((InKey == EKeys::LeftControl || InKey == EKeys::RightControl || InKey == EKeys::LeftCommand || InKey == EKeys::RightCommand) && InEvent != IE_Released) || InViewport->KeyState(EKeys::LeftControl) || InViewport->KeyState(EKeys::RightControl) || InViewport->KeyState(EKeys::LeftCommand) ||  InViewport->KeyState(EKeys::RightCommand);
    const bool bIsShiftDown = ((InKey == EKeys::LeftShift || InKey == EKeys::RightShift) && InEvent != IE_Released) || InViewport->KeyState(EKeys::LeftShift) || InViewport->KeyState(EKeys::RightShift);
    const bool bIsAltDown = ((InKey == EKeys::LeftAlt || InKey == EKeys::RightAlt) && InEvent != IE_Released) || InViewport->KeyState(EKeys::LeftAlt) || InViewport->KeyState(EKeys::RightAlt);

    // When painting we only care about perspective viewports
    if(!bIsAltDown && InViewportClient->IsPerspective())
    {
        // Does the user want to paint right now?
        const bool bUserWantsPaint = bIsLeftButtonDown;
        bool bPaintApplied = false;

        // Stop current tracking if the user is no longer painting
        if( IsPainting() && !bUserWantsPaint &&
            (InKey == EKeys::LeftMouseButton || InKey == EKeys::RightMouseButton || InKey == EKeys::LeftAlt || InKey == EKeys::RightAlt))
        {
            bHandled = true;
            FinishPainting();
        } 
        else if(!IsPainting() && bUserWantsPaint && !InViewportClient->IsMovingCamera())
        {
            bHandled = true;

            // Compute a world space ray from the screen space mouse coordinates
            FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
                InViewportClient->Viewport,
                InViewportClient->GetScene(),
                InViewportClient->EngineShowFlags)
                .SetRealtimeUpdate(InViewportClient->IsRealtime()));

            FSceneView* View = InViewportClient->CalcSceneView(&ViewFamily);
            const FViewportCursorLocation MouseViewportRay(View,(FEditorViewportClient*)InViewport->GetClient(),InViewport->GetMouseX(),InViewport->GetMouseY());

            // Paint!
            bPaintApplied = Paint(InViewport,View->ViewMatrices.GetViewOrigin(),MouseViewportRay.GetOrigin(),MouseViewportRay.GetDirection());

        } 
        else if(IsPainting() && bUserWantsPaint)
        {
            bHandled = true;
        }

        if(!bPaintApplied && !IsPainting())
        {
            bHandled = false;
        } 
        else
        {
            InViewportClient->bLockFlightCamera = true;
        }

        // Also absorb other mouse buttons, and Ctrl/Alt/Shift events that occur while we're painting as these would cause
        // the editor viewport to start panning/dollying the camera
        {
            const bool bIsOtherMouseButtonEvent = (InKey == EKeys::MiddleMouseButton || InKey == EKeys::RightMouseButton);
            const bool bCtrlButtonEvent = (InKey == EKeys::LeftControl || InKey == EKeys::RightControl || InKey == EKeys::LeftCommand || InKey == EKeys::RightCommand);
            const bool bShiftButtonEvent = (InKey == EKeys::LeftShift || InKey == EKeys::RightShift);
            const bool bAltButtonEvent = (InKey == EKeys::LeftAlt || InKey == EKeys::RightAlt);
            if(IsPainting() && (bIsOtherMouseButtonEvent || bShiftButtonEvent || bAltButtonEvent))
            {
                bHandled = true;
            }

            if(bCtrlButtonEvent && !IsPainting())
            {
                bHandled = false;
            } 
            else if(bIsCtrlDown)
            {
                //default to assuming this is a paint command
                bHandled = true;

                // Allow Ctrl+B to pass through so we can support the finding of a selected static mesh in the content browser.
                if(!(bShiftButtonEvent || bAltButtonEvent || bIsOtherMouseButtonEvent) && ((InKey == EKeys::B) && (InEvent == IE_Pressed)))
                {
                    bHandled = false;
                }

                //We can't change those shortcuts, but for accessibility, we have ctrl Z and ctrl Y working for Undo/redo for Iliad strokes
                if(!IsPainting() && (InKey == EKeys::Z && (InEvent == IE_Pressed)) )
                {
                    mController->OnUndoIliad();
                }

                if(!IsPainting() && (InKey == EKeys::Y && (InEvent == IE_Pressed)) )
                {
                    mController->OnRedoIliad();
                }
            }
        }
    }

    return bHandled;
}


FInstanceTexturePaintSettings& FOdysseyViewportDrawingEditorPainter::AddOrRetrieveInstanceTexturePaintSettings(UMeshComponent* iComponent)
{
	return mComponentToTexturePaintSettingsMap.FindOrAdd(iComponent);
}

void FOdysseyViewportDrawingEditorPainter::CacheSelectionData()
{
	ensure(mComponentToAdapterMap.Num() == 0 && mPaintableComponents.Num() == 0);
	const TArray<UMeshComponent*> selectedMeshComponents = GetSelectedComponents<UMeshComponent>();

	TUniquePtr< FComponentReregisterContext > ComponentReregisterContext;
	for (UMeshComponent* meshComponent : selectedMeshComponents)
	{
		TSharedPtr<IMeshPaintGeometryAdapter> meshAdapter = FMeshPaintAdapterFactory::CreateAdapterForMesh(meshComponent, 0);
		if (meshComponent->IsVisible() && meshAdapter.IsValid() && meshAdapter->IsValid())
		{
			mPaintableComponents.Add(meshComponent);
			mComponentToAdapterMap.Add(meshComponent, meshAdapter);
			meshAdapter->OnAdded();
			MeshPaintHelpers::ForceRenderMeshLOD(meshComponent, 0);
			ComponentReregisterContext.Reset(new FComponentReregisterContext(meshComponent));
		}
	}
}

void FOdysseyViewportDrawingEditorPainter::CacheTexturePaintData()
{
	TArray<UMeshComponent*> selectedMeshComponents = GetSelectedComponents<UMeshComponent>();

	mPaintableTextures.Empty();
	if (mPaintableComponents.Num() == 1 && mPaintableComponents[0])
	{
		const UMeshComponent* component = mPaintableComponents[0];
		TSharedPtr<IMeshPaintGeometryAdapter> adapter = mComponentToAdapterMap.FindChecked(component);
		TexturePaintHelpers::RetrieveTexturesForComponent(component, adapter.Get(), mPaintableTextures);
	}

	// Ensure that the selection remains valid or is invalidated
	if (!mPaintableTextures.Contains(mPaintSettings->mTexturePaintSettings.mPaintTexture))
	{
		mPaintSettings->mTexturePaintSettings.mPaintTexture = nullptr;
		if (mPaintableTextures.Num() > 0)
		{
			for (int i = 0; i < mPaintableTextures.Num(); i++)
			{
				UTexture2D* texture = Cast<UTexture2D>(mPaintableTextures[0].Texture);
				UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
				if (texture != mController->GetData()->Texture() && AssetEditorSubsystem->FindEditorForAsset(texture, true) != nullptr)
				{
					if (i == 0)
					{
						FText Title = LOCTEXT("TitleSelectedtextureAlreadyOpened", "Selected Texture Already Opened");
						FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("SelectedtextureAlreadyOpened", "The selected texture is already opened in an other editor. Please close the editor before selecting this texture."), &Title);
					}
					continue;
				}

				mPaintSettings->mTexturePaintSettings.mPaintTexture = texture;
				break;
			}
		}
	}
	else if (mController->GetData()->Texture() != mPaintSettings->mTexturePaintSettings.mPaintTexture)
	{
		UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
		if (mPaintSettings->mTexturePaintSettings.mPaintTexture != mController->GetData()->Texture() && AssetEditorSubsystem->FindEditorForAsset(mPaintSettings->mTexturePaintSettings.mPaintTexture, true) != nullptr)
		{
			mPaintSettings->mTexturePaintSettings.mPaintTexture = nullptr;
			FText Title = LOCTEXT("TitleSelectedtextureAlreadyOpened", "Selected Texture Already Opened");
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("SelectedtextureAlreadyOpened", "The selected texture is already opened in an other editor. Please close the editor before selecting this texture."), &Title);

			if (mPaintableTextures.Num() > 0)
			{
				for (int i = 0; i < mPaintableTextures.Num(); i++)
				{
					UTexture2D* texture = Cast<UTexture2D>(mPaintableTextures[0].Texture);
					if (texture != mController->GetData()->Texture() && AssetEditorSubsystem->FindEditorForAsset(texture, true) != nullptr)
						continue;

					mPaintSettings->mTexturePaintSettings.mPaintTexture = texture;
					break;
				}
			}
		}
	}
	mController->OnEditedTextureChanged(mPaintSettings->mTexturePaintSettings.mPaintTexture);
}

void FOdysseyViewportDrawingEditorPainter::ResetPaintingState()
{
	bArePainting = false;
	TimeSinceStartedPainting = 0.0f;
	mPaintableComponents.Empty();
}

template<typename ComponentClass>
TArray<ComponentClass*> FOdysseyViewportDrawingEditorPainter::GetSelectedComponents() const
{
	TArray<ComponentClass*> components;

	USelection* componentSelection = GEditor->GetSelectedComponents();
	for (int32 selectionIndex = 0; selectionIndex < componentSelection->Num(); ++selectionIndex)
	{
		ComponentClass* selectedComponent = Cast<ComponentClass>(componentSelection->GetSelectedObject(selectionIndex));
		if (selectedComponent)
		{
			components.AddUnique(selectedComponent);
		}
	}

	if (components.Num() == 0)
	{
		USelection* actorSelection = GEditor->GetSelectedActors();
		for (int32 selectionIndex = 0; selectionIndex < actorSelection->Num(); ++selectionIndex)
		{
			AActor* selectedActor = Cast<AActor>(actorSelection->GetSelectedObject(selectionIndex));
			if (selectedActor)
			{
				TInlineComponentArray<ComponentClass*> actorComponents;
				selectedActor->GetComponents(actorComponents);
				for (ComponentClass* Component : actorComponents)
				{
					components.AddUnique(Component);
				}
			}
		}
	}

	return components;
}

void
FOdysseyViewportDrawingEditorPainter::OnStylusStateChanged( const TWeakPtr<SWidget> iWidget, const FStylusState& iState, int32 iIndex )
{    
    //If we don't have a surface, then we don't interact with anything
    if (!mPaintingTexture2D)
        return;

    if(!iWidget.IsValid() )
        return;

    mPreviousEvent = mLastEvent;

    //---
    mLastEvent = FOdysseyStrokePoint(   0
                                      , 0
                                      , iState.GetZ()
                                      , iState.GetPressure()
                                      , iState.GetTimer()
                                      , iState.GetAltitude()
                                      , iState.GetAzimuth()
                                      , iState.GetTwist()
                                      , 0 //iState.GetPitch()
                                      , 0 // iState.GetRoll()
                                      , 0 ); // iState.GetYaw() );
  //---

    static bool stylusWasDown = false;
    static bool is_dragging = false;

    bool isDownEvent = !stylusWasDown && iState.IsStylusDown();
    bool isUpEvent = stylusWasDown && !iState.IsStylusDown();
    bool isMoveEvent = stylusWasDown == iState.IsStylusDown();

    stylusWasDown = iState.IsStylusDown();
    
    if( isDownEvent )
    {
        mIsCapturedByStylus = true;
        mFocusedViewport->CaptureMouse(false);
    }
    else if( isUpEvent )
    {
        mFocusedViewport->CaptureMouse(true);
        FinishPainting();
        mIsCapturedByStylus = false;
    }
    else if( iState.IsStylusDown() && mFocusedViewport )
    {
        FEditorViewportClient* viewportClient = (FEditorViewportClient*)mFocusedViewport->GetClient();
        // Compute a world space ray from the screen space mouse coordinates
        FSceneViewFamilyContext viewFamily(FSceneViewFamily::ConstructionValues(
            mFocusedViewport,
            viewportClient->GetScene(),
            viewportClient->EngineShowFlags)
            .SetRealtimeUpdate(viewportClient->IsRealtime()));

        FSceneView* view = viewportClient->CalcSceneView(&viewFamily);

        float scaleDPI = iWidget.Pin().Get()->GetCachedGeometry().GetAccumulatedLayoutTransform().GetScale();
        FVector2D positionInViewport = iWidget.Pin().Get()->GetCachedGeometry().AbsoluteToLocal(iState.GetPosition()) * scaleDPI;

        const FViewportCursorLocation mouseViewportRay(view,viewportClient, positionInViewport.X , positionInViewport.Y);

        Paint(mFocusedViewport,view->ViewMatrices.GetViewOrigin(),mouseViewportRay.GetOrigin(),mouseViewportRay.GetDirection());
        //CapturedMouseMoveWithStrokePoint( stroke_point );*/
    }

    
    mStylusLastEventTime = std::chrono::steady_clock::now();
}

template TArray<UStaticMeshComponent*> FOdysseyViewportDrawingEditorPainter::GetSelectedComponents<UStaticMeshComponent>() const;
template TArray<USkeletalMeshComponent*> FOdysseyViewportDrawingEditorPainter::GetSelectedComponents<USkeletalMeshComponent>() const;
template TArray<UMeshComponent*> FOdysseyViewportDrawingEditorPainter::GetSelectedComponents<UMeshComponent>() const;

#undef LOCTEXT_NAMESPACE

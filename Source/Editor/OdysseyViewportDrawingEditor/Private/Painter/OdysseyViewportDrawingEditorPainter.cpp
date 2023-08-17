// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyViewportDrawingEditorPainter.h"
#include "OdysseyViewportDrawingEditorEdMode.h"
#include "OdysseyViewportDrawingEditorCommands.h"
#include "OdysseyViewportDrawingEditorSettings.h"
#include "OdysseyViewportDrawingEditorTextureBasedAdapter.h"
#include "OdysseyViewportDrawingEditorScreenBasedAdapter.h"

#include "ViewportDrawingEditor/OdysseyViewportDrawingEditorExtension.h"

#include "MeshPaintModule.h"
#include "MeshPaintAdapterFactory.h"
#include "IMeshPaintGeometryAdapter.h"
#include "MeshPaintHelpers.h"
#include "TexturePaintHelpers.h"

#include "Components/StaticMeshComponent.h"
#include "StaticMeshResources.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Selection.h"

#include "Kismet/GameplayStatics.h"
#include "LevelEditorSequencerIntegration.h"

#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorPainter"

FOdysseyViewportDrawingEditorPainter::~FOdysseyViewportDrawingEditorPainter()
{
    FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll(this);
	Finalize();
}

FOdysseyViewportDrawingEditorPainter::FOdysseyViewportDrawingEditorPainter(FOdysseyViewportDrawingEditorExtension* iExtension)
    : mExtension(iExtension)
    , mPaintSettings(UOdysseyViewportDrawingEditorSettings::Get())
{
}

void FOdysseyViewportDrawingEditorPainter::Initialize()
{
    USelection* actorSelection = GEditor->GetSelectedActors();
    for (int32 selectionIndex = 0; selectionIndex < actorSelection->Num(); ++selectionIndex)
    {
        AActor* selectedActor = Cast<AActor>(actorSelection->GetSelectedObject(selectionIndex));
        if (selectedActor)
        {
            mExtension->SetActor(selectedActor);
        }
    }

    FLevelEditorSequencerIntegration::Get().GetOnSequencersChanged().AddRaw( this, &FOdysseyViewportDrawingEditorPainter::OnSequencersChanged );
    mExtension->AdapterChangedDelegate().AddRaw( this, &FOdysseyViewportDrawingEditorPainter::CreatePaintingAdapter );
    CreatePaintingAdapter();
    
    OnSequencersChanged(); //Init the potential sequencers already present
}

void FOdysseyViewportDrawingEditorPainter::Finalize()
{
    mExtension->SetActor(nullptr); //Unselect the actor to cleanup the editor
    FLevelEditorSequencerIntegration::Get().GetOnSequencersChanged().RemoveAll(this);
    mExtension->AdapterChangedDelegate().RemoveAll(this);
    ClearAllDelegatesSequencers();

    if (mPaintingAdapter)
    {
        mPaintingAdapter.Reset();
    }
}

void FOdysseyViewportDrawingEditorPainter::CreatePaintingAdapter()
{
    if (mPaintingAdapter)
        mPaintingAdapter.Reset();

    switch (mExtension->PaintingAdapterMethod())
    {
        case EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyTextureBased :
            mPaintingAdapter = MakeShared<FOdysseyViewportDrawingEditorTextureBasedAdapter>(mExtension);
        break;
        case EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyScreenBased:
            mPaintingAdapter = MakeShared<FOdysseyViewportDrawingEditorScreenBasedAdapter>(mExtension);
        break;
        case EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyMeshBasedPlanar:
            mPaintingAdapter = MakeShared<FOdysseyViewportDrawingEditorMeshBasedAdapter>(mExtension);
        break;
        default :
            mPaintingAdapter = MakeShared<FOdysseyViewportDrawingEditorTextureBasedAdapter>(mExtension);
        break;
    }
}

void FOdysseyViewportDrawingEditorPainter::SelectActor(AActor* iActor)
{
    mExtension->SetActor(iActor);
}

IOdysseyViewportDrawingEditorAdapter* FOdysseyViewportDrawingEditorPainter::GetOdysseyViewportDrawingEditorAdapter()
{
    if( mPaintingAdapter )
        return mPaintingAdapter.Get();
    return nullptr;
}

void FOdysseyViewportDrawingEditorPainter::AddReferencedObjects(FReferenceCollector& iCollector)
{
	FMeshPaintAdapterFactory::AddReferencedObjectsGlobals(iCollector);

    TMap<UMeshComponent*, TSharedPtr<IMeshPaintGeometryAdapter>> map = mExtension->ComponentToAdapterMap();
	for (TMap< UMeshComponent*, TSharedPtr<IMeshPaintGeometryAdapter>>::TIterator It(map); It; ++It)
	{
        //Prevent GC on the components we save painting settings for
		iCollector.AddReferencedObject(It.Key());

        //ES: Very weird I don't know why this is done
		It.Value()->AddReferencedObjects(iCollector);
	}
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

TSharedPtr<IMeshPaintGeometryAdapter> FOdysseyViewportDrawingEditorPainter::GetMeshAdapterForComponent(const UMeshComponent* iComponent)
{
	return mExtension->ComponentToAdapterMap().FindChecked(iComponent);
}

// Sequencer related ------------------------------------------

void FOdysseyViewportDrawingEditorPainter::OnSequencersChanged()
{
    //Delete delegates of previous sequencers
    ClearAllDelegatesSequencers();

    //Getting the new sequencers
    mSequencers = FLevelEditorSequencerIntegration::Get().GetSequencers();

    for (int i = 0; i < mSequencers.Num(); i++)
    {
        if( TSharedPtr<ISequencer> itSequencer =  mSequencers[i].Pin() )
        {
            //When we're playing or scrubbing, we delete the delegates of the sequencer with DisableDelegatesSequencer
            itSequencer->OnBeginScrubbingEvent().AddRaw(this, &FOdysseyViewportDrawingEditorPainter::DisableDelegatesSequencer);
            itSequencer->OnPlayEvent().AddRaw(this, &FOdysseyViewportDrawingEditorPainter::DisableDelegatesSequencer);

            //When we're done, we put them back with EnableDelegatesSequencer
            itSequencer->OnEndScrubbingEvent().AddRaw(this, &FOdysseyViewportDrawingEditorPainter::EnableDelegatesSequencer);
            itSequencer->OnStopEvent().AddRaw(this, &FOdysseyViewportDrawingEditorPainter::EnableDelegatesSequencer);
            
            //By default, they are enabled
            itSequencer->OnGlobalTimeChanged().AddRaw(this, &FOdysseyViewportDrawingEditorPainter::OnSyncPaintingWithSequencer);
            itSequencer->OnMovieSceneDataChanged().AddRaw( this, &FOdysseyViewportDrawingEditorPainter::OnSyncPaintingWithSequencerMovieSceneChanged );
        }
    }
}

void FOdysseyViewportDrawingEditorPainter::OnSyncPaintingWithSequencer()
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mExtension->GetEditor()->GetSource();
    if (!source || source->Id() != FOdysseyTextureEditorSource::StaticId())
        return;

    TSharedPtr<FOdysseyTextureEditorSource> textureSource = StaticCastSharedPtr<FOdysseyTextureEditorSource>(source);

    //Security, in case we're flipping in sequencer while drawing at the same time
    if( textureSource->GetTexture() )
        mPaintingAdapter->FinishPainting();

    if (mExtension->Component())
    {
        TArray<UMaterialInterface*> selectableMaterials;
        mExtension->Component()->GetUsedMaterials(selectableMaterials);
        if (selectableMaterials.Num() > 0)
        {
            if (selectableMaterials.Find(mExtension->Material()) == INDEX_NONE)
            {
                mExtension->SetMaterial(selectableMaterials[0]);
            }
        }
    }
}

void FOdysseyViewportDrawingEditorPainter::OnSyncPaintingWithSequencerMovieSceneChanged(EMovieSceneDataChangeType iChangedType)
{
    OnSyncPaintingWithSequencer();
}

void FOdysseyViewportDrawingEditorPainter::DisableDelegatesSequencer()
{
    for (int i = 0; i < mSequencers.Num(); i++)
    {
        if (TSharedPtr<ISequencer> itSequencer = mSequencers[i].Pin())
        {
            itSequencer->OnGlobalTimeChanged().RemoveAll( this );
            itSequencer->OnMovieSceneDataChanged().RemoveAll( this );
        }
    }
}

void FOdysseyViewportDrawingEditorPainter::EnableDelegatesSequencer()
{
    for (int i = 0; i < mSequencers.Num(); i++)
    {
        if (TSharedPtr<ISequencer> itSequencer = mSequencers[i].Pin())
        {
            itSequencer->OnGlobalTimeChanged().AddRaw(this, &FOdysseyViewportDrawingEditorPainter::OnSyncPaintingWithSequencer);
            itSequencer->OnMovieSceneDataChanged().AddRaw(this, &FOdysseyViewportDrawingEditorPainter::OnSyncPaintingWithSequencerMovieSceneChanged);
        }
    }
    OnSyncPaintingWithSequencer();
}

void FOdysseyViewportDrawingEditorPainter::ClearAllDelegatesSequencers()
{
    for (int i = 0; i < mSequencers.Num(); i++)
    {
        if (TSharedPtr<ISequencer> itSequencer = mSequencers[i].Pin())
        {
            itSequencer->OnBeginScrubbingEvent().RemoveAll(this);
            itSequencer->OnPlayEvent().RemoveAll(this);
            itSequencer->OnEndScrubbingEvent().RemoveAll(this);
            itSequencer->OnStopEvent().RemoveAll(this);
            itSequencer->OnGlobalTimeChanged().RemoveAll(this);
            itSequencer->OnMovieSceneDataChanged().RemoveAll(this);
        }
    }
}

// ---


void FOdysseyViewportDrawingEditorPainter::Tick(float iDeltaTime)
{
    if (mPaintingAdapter)
    {
        mPaintingAdapter->Tick(iDeltaTime);
    }
}


#undef LOCTEXT_NAMESPACE

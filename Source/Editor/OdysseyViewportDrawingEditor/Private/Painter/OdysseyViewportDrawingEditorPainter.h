// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "IMeshPainter.h"
#include "OdysseyViewportDrawingEditorGUI.h"
#include "IStylusState.h"
#include "Input/OdysseyPoint.h"
#include "MeshPaintTypes.h"
#include "ISequencer.h"
#include "Engine/StaticMesh.h"
#include "OdysseyViewportDrawingEditorViewportClient.h"
#include "IOdysseyViewportDrawingEditorAdapter.h"

#include "IOdysseyStylusInputModule.h"

class UOdysseyViewportDrawingEditorSettings;
class IMeshPaintGeometryAdapter;
struct FAssetData;
class SWidget;

struct FPaintableTexture;
struct FPaintTexture2DData;
struct FTexturePaintMeshSectionInfo;

/** Painter class used by the level viewport mesh painting mode */
class FOdysseyViewportDrawingEditorPainter
{
public:
	/** destructor */
	virtual ~FOdysseyViewportDrawingEditorPainter();

	/** constructor */
	FOdysseyViewportDrawingEditorPainter(TSharedPtr<FOdysseyViewportDrawingEditor> iEditor);

public:
	void Initialize();
	void Finalize();

private:
    void CreatePaintingAdapter();

public:
	void AddReferencedObjects(FReferenceCollector& iCollector);
	TSharedPtr<IMeshPaintGeometryAdapter> GetMeshAdapterForComponent(const UMeshComponent* iComponent);
    void Tick(float iDeltaTime);
	void SelectActor(AActor* iActor);
	void ActorDeselected(AActor* iActor);
	IOdysseyViewportDrawingEditorAdapter* GetOdysseyViewportDrawingEditorAdapter();

private:
	/** Per triangle action function used for retrieving triangle eligible for texture painting */
	void GatherTextureTriangles(IMeshPaintGeometryAdapter* iAdapter, int32 iTriangleIndex, const int32 iVertexIndices[3], TArray<FTexturePaintTriangleInfo>* iTriangleInfo, TArray<FTexturePaintMeshSectionInfo>* iSectionInfos, int32 iUVChannelIndex);

private:
    /** Sequencer related */
    void OnSequencersChanged();
    void OnSyncPaintingWithSequencer();
    void OnSyncPaintingWithSequencerMovieSceneChanged( EMovieSceneDataChangeType iChangedType );
	void DisableDelegatesSequencer();
	void EnableDelegatesSequencer();
	void ClearAllDelegatesSequencers();

protected:	
	TSharedPtr<FOdysseyViewportDrawingEditor> mEditor;

	/** Painting Extension: describes the method by which we draw in the viewport */
	TSharedPtr<IOdysseyViewportDrawingEditorAdapter> mPaintingAdapter;

	/** Painting settings */
	UOdysseyViewportDrawingEditorSettings* mPaintSettings;

    //Sequencers used thorough the editor. They may change the current actor selected, so we need to keep track of what they are doing
    TArray<TWeakPtr<ISequencer>> mSequencers;
};

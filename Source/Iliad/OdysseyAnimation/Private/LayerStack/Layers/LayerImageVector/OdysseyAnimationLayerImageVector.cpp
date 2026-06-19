// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationLayerImageVector.h"

#include "OdysseyAnimationCellImageVector.h"
#include "OdysseyLayerCellImageStagger.h"

#if WITH_EDITOR
#include "OdysseyPixelFormat.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"
#include "OdysseyStyle.h"
#include "OdysseyAnimationCellsContainerImport.h"
#include "OdysseyLighttable.h"
#include "OdysseyAnimationLayerImageVectorImport.h"
#include "OdysseyAnimation.h"
#include "OdysseyMediaVector.h"
#include "ScopedTransaction.h"
#include "UObject/DevObjectVersion.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorObject.h"
#include "OdysseyPalette.h"

#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorLayer.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerBreakdownAlter.h"
#include "Editor.h"
#include "Misc/TransactionObjectEvent.h"
#endif

#define LOCTEXT_NAMESPACE "Animation"

UOdysseyAnimationLayerImageVector::UOdysseyAnimationLayerImageVector()
#if WITH_EDITOR
    : mVectorLayer( MakeShared<FOdysseyVectorLayer>(this) )
#endif
{}

void
UOdysseyAnimationLayerImageVector::PostInitProperties()
{
    Super::PostInitProperties();

    LayerTypeName = LOCTEXT("layer-image-vector.type", "Vector Layer");
#if WITH_EDITOR
    Icon = FSlateIcon("OdysseyStyle", "OdysseyLayerStack.LayerVector16");
#endif
    DefaultCellClass = UOdysseyAnimationCellImageVector::StaticClass();

    SupportedCellTypes.Add(UOdysseyAnimationCellImageVector::StaticClass());
    SupportedCellTypes.Add(UOdysseyLayerCellImageStagger::StaticClass());
}

#if WITH_EDITOR
UOdysseyAnimationLayerImageVector::~UOdysseyAnimationLayerImageVector()
{
    mVectorLayer->RemoveAllChildren();
}

void
UOdysseyAnimationLayerImageVector::PostLoad()
{
    Super::PostLoad();
    UpdateSharedEnv();
}

void
UOdysseyAnimationLayerImageVector::PostDuplicate(EDuplicateMode::Type iDuplicateMode)
{
    Super::PostDuplicate(iDuplicateMode);
    UpdateSharedEnv();
}

void
UOdysseyAnimationLayerImageVector::UpdateSharedEnv()
{
    mVectorLayer->RemoveAllChildren();

    for (UOdysseyLayerCell* cell : Cells)
    {
        // note: a cell can be nullptr
        if ( cell && !cell->IsA<UOdysseyAnimationCellImageVector>())
            continue;

        UOdysseyAnimationCellImageVector* cellVector = Cast<UOdysseyAnimationCellImageVector>(cell);

        if( cellVector )
        {
            mVectorLayer->AppendChild( cellVector->GetVectorCell() );
        }
    }

    mVectorLayer->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
//    mVectorLayer->RequestRedraw( nullptr, 0 );
}

struct FOdysseyAnimationLayerImageVectorObjectVersion
{
    enum Type
    {
        // Before any version changes were made
        BeforeCustomVersionWasAdded,

        // Reworked how anim blueprint root nodes are recovered
        RefactorCellsToUObject,

        // -----<new versions can be added above this line>-------------------------------------------------
        VersionPlusOne,
        LatestVersion = VersionPlusOne - 1
    };

    // The GUID for this custom version number
    const static FGuid GUID;

private:
    FOdysseyAnimationLayerImageVectorObjectVersion() {}
};

const FGuid FOdysseyAnimationLayerImageVectorObjectVersion::GUID(0x8799329A, 0x214C4880, 0xB9229B09, 0x30C8D2BB);
FDevVersionRegistration GRegisterOdysseyAnimationLayerImageVectorObjectVersion(FOdysseyAnimationLayerImageVectorObjectVersion::GUID, FOdysseyAnimationLayerImageVectorObjectVersion::LatestVersion, TEXT("OdysseyAnimationLayerImageVector"));

void
UOdysseyAnimationLayerImageVector::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);

    Ar.UsingCustomVersion(FOdysseyAnimationLayerImageVectorObjectVersion::GUID);

    if( Ar.IsLoading() && Ar.CustomVer(FOdysseyAnimationLayerImageVectorObjectVersion::GUID) < FOdysseyAnimationLayerImageVectorObjectVersion::RefactorCellsToUObject)
    {
        if (!FOdysseyAnimationLayerImageVectorImport::Read( this, Ar ))
        {
            //Old Style No Chunk Loading
            //DEPRECATED: Keep for compatibility with Odyssey 2.0 and prior
            if (!FOdysseyAnimationCellsContainerImport::Read( this, Ar ))
            {
                //Old Style No Chunk Loading
                //Load or Save the offset
                Ar << CellsOffset;

                //Empty Cells to prepare for loading
                Cells.Empty();

                //Load or Save number of cells
                int32 numCells = 0;
                Ar << numCells;

                for ( int i = 0; i < numCells; i++ )
                {
                    //Load the cell type
                    FName cellType;
                    Ar << cellType;

                    if (cellType == TEXT("FOdysseyAnimationCellImageVector"))
                    {
                        UOdysseyAnimationCellImageVector* cell = NewObject<UOdysseyAnimationCellImageVector>(this, UOdysseyAnimationCellImageVector::StaticClass(), NAME_None, RF_Public | RF_Transactional);
                        Cells.Add(cell);
                        cell->OldSerialize(Ar);
                    }
                    else if (cellType == TEXT("FOdysseyAnimationCellImageStagger"))
                    {
                        UOdysseyLayerCellImageStagger* cell = NewObject<UOdysseyLayerCellImageStagger>(this, UOdysseyLayerCellImageStagger::StaticClass(), NAME_None, RF_Public | RF_Transactional);
                        Cells.Add(cell);
                        cell->OldSerialize(Ar);
                    }
                }
                UpdateCellsIndexInLayer();
            }
        }
    }
}

void
UOdysseyAnimationLayerImageVector::RequestRedrawAllVectorCells()
{
    for (UOdysseyLayerCell* cell : Cells)
    {
        // note: a cell can be nullptr
        if ( cell && cell->IsA<UOdysseyAnimationCellImageVector>() )
        {
            UOdysseyAnimationCellImageVector* cellVector = Cast<UOdysseyAnimationCellImageVector>(cell);

            if( cellVector )
            {
                mVectorLayer->RequestRedraw( cellVector->GetVectorCell(), 0 );
            }
        }
    }
}

void
UOdysseyAnimationLayerImageVector::SetIsWireframe(bool Value)
{
    if( !IsEditable() )
        return;

    Modify();

    bIsWireframe = Value;

    RequestRedrawAllVectorCells();

    RenderingChanged();
}

void
UOdysseyAnimationLayerImageVector::SetIsColored(bool Value)
{
    if( !IsEditable() )
        return;

    Modify();

    bIsColored = Value;

    RequestRedrawAllVectorCells();

    RenderingChanged();
}

bool
UOdysseyAnimationLayerImageVector::IsWireframe() const
{
    return bIsWireframe;
}

bool
UOdysseyAnimationLayerImageVector::IsColored() const
{
    return bIsColored;
}



void
UOdysseyAnimationLayerImageVector::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    Super::PostTransacted(iTransactionEvent);

    if ( iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo )
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();

    if (changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayerImageVector, bIsColored)))
    {
        RenderingChanged();
    }

    if (changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayerImageVector, bIsWireframe)))
    {
        RenderingChanged();
    }
}

TSharedPtr<IOdysseyMedia>
UOdysseyAnimationLayerImageVector::GetCellMediaVector(uint32 iFrameIndex) const
{
    UOdysseyLayerCell* cell = GetCellAtFrame(iFrameIndex);
    if (!cell)
        return nullptr;

    FOdysseyMediaProvider provider = cell->GetMediaProvider();
    if (!provider.HasMedia<FOdysseyMediaVector>())
        return nullptr;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectorList = provider.GetOrCreateMedias<FOdysseyMediaVector>();
    if (mediaVectorList.Num() <= 0)
        return nullptr;

    return mediaVectorList[0];
}

FOdysseyMediaProvider
UOdysseyAnimationLayerImageVector::GetMediaProvider(uint32 iFrameIndex) const
{
    FOdysseyMediaProvider provider;

    bool isActive = IsActivatedRecursively();
    bool isLocked = IsLockedRecursively();
    provider.IsLocked(!isActive || isLocked);

    TSharedPtr<IOdysseyMedia> cellMediaVector = GetCellMediaVector(iFrameIndex);
    if (cellMediaVector)
    {
        provider.Add(cellMediaVector);
    }
    else
    {
        FOdysseyMediaProvider::FCreateMediaDelegate createMediaVectorDelegate = FOdysseyMediaProvider::FCreateMediaDelegate::CreateUObject(const_cast<UOdysseyAnimationLayerImageVector*>(this), &UOdysseyAnimationLayerImageVector::CreateMediaVector, (int)iFrameIndex);
        provider.Add(FOdysseyMediaVector::StaticId(), createMediaVectorDelegate);
    }
    return provider;
}

TSharedPtr<IOdysseyMedia>
UOdysseyAnimationLayerImageVector::CreateMediaVector(int iFrameIndex)
{
    //Create the cell if needed
    AutoCreateCell(iFrameIndex);
    return GetCellMediaVector(iFrameIndex);
}

void
UOdysseyAnimationLayerImageVector::AutoCreateCell(int iFrameIndex)
{
    if( !IsEditable() )
        return;

    FInt32Range range = GetFrameRange();

    //Check if iFrameIndex is Out Of Range
    if ( iFrameIndex < range.GetLowerBoundValue())
    {
        FScopedTransaction transaction(LOCTEXT("layer-image-vector.create-cell-transaction", "Create Cell"));

        Modify();

        //Add a frame at current frame and extend it
        UOdysseyLayerCell* cell = AddCell(UOdysseyAnimationCellImageVector::StaticClass(), 0);
        cell->SetExposure(range.GetLowerBoundValue() - iFrameIndex);
        SetCellsOffset(GetCellsOffset() - cell->GetExposure());
        return;
    }

    if ( iFrameIndex > range.GetUpperBoundValue())
    {
        FScopedTransaction transaction( LOCTEXT( "layer-image-vector.create-cell-transaction", "Create Cell" ) );

        Modify();

        int cellExposure = Cells.Last()->GetExposure() + iFrameIndex - range.GetUpperBoundValue() - 1;
        Cells.Last()->SetExposure(cellExposure);
        UOdysseyLayerCell* cell = AddCell(UOdysseyAnimationCellImageVector::StaticClass());
        return;
    }
}

void
UOdysseyAnimationLayerImageVector::Merge(const TArray<UOdysseyLayer*>& iLayers)
{
    UOdysseyAnimation* animation = GetAnimation();
    if ( !animation )
        return;

    if( !IsEditable() )
        return;

    Modify();

    //Get all frame ranges and combine them
    TArray<FInt32Range> frameRanges = {};
    for (int layerIndex = 0; layerIndex < iLayers.Num(); layerIndex++)
    {
        UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(iLayers[layerIndex]);
        if ( !layer )
            continue;

        frameRanges.Add(layer->GetFrameRange());
    }
    FInt32Range frameRange = FInt32Range::Hull(frameRanges);

    //Deduce offset from frame ranges
    SetCellsOffset(frameRange.GetLowerBoundValue());

    //Get cell ranges from each frame ImageRenderAbility composition
    int startFrame = frameRange.GetLowerBound().IsInclusive() ? frameRange.GetLowerBoundValue() : frameRange.GetLowerBoundValue() + 1;
    int endFrame = frameRange.GetUpperBound().IsInclusive() ? frameRange.GetUpperBoundValue() : frameRange.GetUpperBoundValue() - 1;
    TArray<FGuid> previousIds;
    TArray<FInt32Range> cellRanges;
    for (int frameIndex = startFrame; frameIndex <= endFrame; frameIndex++)
    {
        TArray<FGuid> currentIds;
        for (int layerIndex = 0; layerIndex < iLayers.Num(); layerIndex++)
        {
            UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(iLayers[layerIndex]);
            if ( !layer )
                continue;

            currentIds.Append(layer->GetRenderingComposition(EOdysseyRenderingType::Render, frameIndex));
        }

        //Do we need a new cell
        if (currentIds != previousIds)
        {
            //do we already have a cell at this position
            cellRanges.Add(FInt32Range::Inclusive(frameIndex, frameIndex));
            previousIds = currentIds;
        }
        else
        {
            cellRanges.Last().SetUpperBoundValue(frameIndex);
        }
    }

    RemoveCells(Cells);
    TArray<UOdysseyLayerCell*> cells = AddCells(UOdysseyAnimationCellImageVector::StaticClass(), 0, cellRanges.Num());

    for (int i = 0; i < cellRanges.Num(); i++)
    {
        const FInt32Range& cellRange = cellRanges[i];
        UOdysseyAnimationCellImageVector* cell = Cast<UOdysseyAnimationCellImageVector>(cells[i]);
        cell->SetExposure(cellRange.GetUpperBoundValue() - cellRange.GetLowerBoundValue() + 1);

        int frame = cellRange.GetLowerBoundValue();
        FOdysseyVectorGroupPaint* destinationScene = cell->GetVectorCell()->GetScene();

        for (int layerIndex = 0; layerIndex < iLayers.Num(); layerIndex++)
        {
            UOdysseyAnimationLayerImageVector* vectorLayer = Cast<UOdysseyAnimationLayerImageVector>(iLayers[layerIndex]);
            if ( !vectorLayer )
                continue;

            UOdysseyLayerCell* srcCell = vectorLayer->GetCellAtFrame(frame);
            if (!srcCell)
                continue;

            if (srcCell && srcCell->IsA<UOdysseyLayerCellImageStagger>())
            {
                int staggerFrame = frame;
                while(srcCell && srcCell->IsA<UOdysseyLayerCellImageStagger>())
                {
                    UOdysseyLayerCellImageStagger* cellStagger = Cast<UOdysseyLayerCellImageStagger>(srcCell);
                    int srcCellFrame = staggerFrame - srcCell->GetFrameRange().GetLowerBoundValue();

                    staggerFrame = cellStagger->GetReferenceFrameAtFrame(srcCellFrame);
                    srcCell = vectorLayer->GetCellAtFrame(staggerFrame);
                }
            }

            UOdysseyAnimationCellImageVector* cellVector = Cast<UOdysseyAnimationCellImageVector>(srcCell);
            if (!cellVector)
                continue;

            FOdysseyVectorGroupPaint* scene = cellVector->GetVectorCell()->GetScene();
            for( FOdysseyVectorObject* child : scene->GetChildrenList() )
            {
                FOdysseyVectorObject* copiedChild = child->Copy();
                destinationScene->AppendChild( copiedChild );

                scene->CopyBuckets( destinationScene, false );
            }
        }
    }

    for ( UOdysseyLayerCell* cell : Cells)
    {
        UOdysseyAnimationCellImageVector* cellVector = Cast<UOdysseyAnimationCellImageVector>(cell);
        FOdysseyVectorGroupPaint* scene = cellVector->GetVectorCell()->GetScene();

        scene->UpdateMatrix();

        scene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    }

    mVectorLayer->RequestRedraw( nullptr, 0 );
}

//static
uint32
UOdysseyAnimationLayerImageVector::GetInbetweeningRowHeight()
{
    return 20UL;
}

TSharedPtr<FOdysseyVectorLayer>
UOdysseyAnimationLayerImageVector::GetVectorLayer()
{
    return mVectorLayer;
}

// Implements Interface IOdysseyVectorLayer::GetWidth
uint32
UOdysseyAnimationLayerImageVector::GetWidth()
{
    return ( uint32 ) GetAnimation()->GetWidth();
}

// Implements Interface IOdysseyVectorLayer::GetHeight
uint32
UOdysseyAnimationLayerImageVector::GetHeight()
{
    return ( uint32 ) GetAnimation()->GetHeight();
}

// Implements Interface IOdysseyVectorLayer::GetCellByIndex
FOdysseyVectorCell*
UOdysseyAnimationLayerImageVector::GetCellByIndex( uint32 iIndex )
{
    uint32 cellCount = Cells.Num();

    if( ( iIndex >= 0 ) && ( iIndex < cellCount ) )
    {
        UOdysseyLayerCell* cell = Cells[iIndex];

        if( cell )
        {
            if (!cell->IsA<UOdysseyAnimationCellImageVector>())
                return nullptr;

            return Cast<UOdysseyAnimationCellImageVector>(cell)->GetVectorCell();
        }
    }

    return nullptr;
}

// Implements Interface IOdysseyVectorLayer::Contains
bool
UOdysseyAnimationLayerImageVector::Contains( FOdysseyVectorCell* iCandidateCell )
{
    return GetCells().ContainsByPredicate( [ iCandidateCell ] ( UOdysseyLayerCell* cell )
                                           {
                                               UOdysseyAnimationCellImageVector* vectorCell = Cast<UOdysseyAnimationCellImageVector>(cell);


                                               return ( vectorCell && ( vectorCell->GetVectorCell() == iCandidateCell ) ) ? true : false;
                                           } );
}

// Implements Interface IOdysseyVectorLayer::GetLastCell
FOdysseyVectorCell*
UOdysseyAnimationLayerImageVector::GetLastCell()
{
    if (Cells.IsEmpty())
        return nullptr;

    UOdysseyLayerCell* lastCell = Cells.Last();
    if (!lastCell->IsA<UOdysseyAnimationCellImageVector>())
        return nullptr;

    return Cast<UOdysseyAnimationCellImageVector>(lastCell)->GetVectorCell();
}

// Implements Interface IOdysseyVectorLayer::GetFirstCell
FOdysseyVectorCell*
UOdysseyAnimationLayerImageVector::GetFirstCell()
{
    if (Cells.IsEmpty())
        return nullptr;

    UOdysseyLayerCell* firstCell = Cells[0];
    if (!firstCell->IsA<UOdysseyAnimationCellImageVector>())
        return nullptr;

    return Cast<UOdysseyAnimationCellImageVector>(firstCell)->GetVectorCell();
}

// Implements Interface IOdysseyVectorLayer::GetMaxCellFrom
FOdysseyVectorCell*
UOdysseyAnimationLayerImageVector::GetMaxCellFrom( uint32 iIndex )
{
    UOdysseyAnimationCellImageVector* maxLayerCell = nullptr;

    for( int32 i = iIndex; i < GetCells().Num(); i++ )
    {
        UOdysseyLayerCell* candidateCell = GetCells()[i];

        if( candidateCell->GetClass() == UOdysseyAnimationCellImageVector::StaticClass() )
        {
            maxLayerCell = Cast<UOdysseyAnimationCellImageVector>(candidateCell);
        }
    }

    return maxLayerCell ? maxLayerCell->GetVectorCell() : nullptr;
}

// Implements Interface IOdysseyVectorLayer::GetMinCellFrom
FOdysseyVectorCell*
UOdysseyAnimationLayerImageVector::GetMinCellFrom( uint32 iIndex )
{
    UOdysseyAnimationCellImageVector* maxLayerCell = nullptr;

    for ( int32 i = iIndex; i >= 0; i-- )
    {
        UOdysseyLayerCell* candidateCell = GetCells()[i];

        if( candidateCell->GetClass() == UOdysseyAnimationCellImageVector::StaticClass() )
        {
            maxLayerCell = Cast<UOdysseyAnimationCellImageVector>(candidateCell);
        }
    }

    return maxLayerCell ? maxLayerCell->GetVectorCell() : nullptr;
}

// Implements Interface IOdysseyVectorLayer::GetPaletteSetID
FGuid
UOdysseyAnimationLayerImageVector::GetPaletteSetID( UOdysseyPalette* iPalette )
{
    UOdysseyAnimation* animation = GetAnimation();

    if( animation )
    {
        for( int32 i = 0; i < animation->Palettes.Num(); i++ )
        {
            if( animation->Palettes[i]->mPalette == iPalette )
            {
                return animation->Palettes[i]->mSet;
            }
        }
    }

    return FGuid();
}

// Implements Interface IOdysseyVectorLayer::GetPaletteSets
const TArray<UOdysseyPaletteSet*>
UOdysseyAnimationLayerImageVector::GetPaletteSets() const
{
    TArray<UOdysseyPaletteSet*> paletteSets;
    UOdysseyAnimation* animation = GetAnimation();
    if( !animation )
        return TArray<UOdysseyPaletteSet*>();

    paletteSets = animation->Palettes;

    //Fail safe in case the user force delete a used palette while in the editor
    for( int i = 0; i < paletteSets.Num(); i++ )
    {
        if( !paletteSets[i]->mPalette || !paletteSets[i]->mPalette->IsValidLowLevel() )
        {
            paletteSets.RemoveAt(i);
            i--;
        }
    }

    // Legacy, to remove next version, ensure that the Palettes are going to be saved with the upgraded data
    for(int i = 0; i < paletteSets.Num(); i++)
    {
        if(paletteSets[i]->mPalette->NeedsSavingAfterUpgrade)
        {
            paletteSets[i]->mPalette->MarkPackageDirty();
            paletteSets[i]->mPalette->NeedsSavingAfterUpgrade = false;
        }
    }
    //---

    return paletteSets;
}

#if WITH_EDITOR

TArray<FName>
UOdysseyAnimationLayerImageVector::GetRows() const
{
    TArray<FName> rows = Super::GetRows();
    rows.Add("Inbetweening");

    return rows;
}

int
UOdysseyAnimationLayerImageVector::GetRowHeight(FName iSubRowName) const
{
    if (iSubRowName == "Inbetweening")
    {
        const std::list<FOdysseyVectorTag*>& tagList = mVectorLayer->GetSharedTagList();
        int numTags = 0;
        for (FOdysseyVectorTag* tag : tagList)
        {
            if (tag->GetClass() == FOdysseyVectorTagInbetweener::StaticClass())
                numTags++;
        }
        return GetInbetweeningRowHeight() * numTags;
    }

    return Super::GetRowHeight(iSubRowName);
}

bool
UOdysseyAnimationLayerImageVector::IsRowVisible(FName iSubRowName) const
{
    if (iSubRowName == "Inbetweening")
        return false; //managed by the editor (see SOdysseyAnimationLayerImageVectorTimeline)

    return Super::IsRowVisible(iSubRowName);
}

#endif //WITH_EDITOR

void
UOdysseyAnimationLayerImageVector::MakeBreakdownTargetMap( std::list<FOdysseyVectorTagInbetweener*>& oInbetweenerTagList )
{
    // Make breakdown lookup for adapting the length of the inbetweener tags
    for( FOdysseyVectorTag* sharedTag : mVectorLayer->GetSharedTagList() )
    {
        if( sharedTag->GetClass() == FOdysseyVectorTagInbetweener::StaticClass() )
        {
            FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>( sharedTag );
            // we copy the list because we may alter it.
            std::list<FInbetweenerBreakdown*> breakdownList = inbetweenerTag->GetBreakdownList();

            oInbetweenerTagList.push_back( inbetweenerTag );

            for( FInbetweenerBreakdown* breakdown : breakdownList )
            {
                mBreakdownTargetMap.Add( breakdown, breakdown->GetTargetCell() );
            }
        }
    }
}

void
UOdysseyAnimationLayerImageVector::CheckBreakdownTargetMap()
{
    for ( const auto& pair : mBreakdownTargetMap )
    {
        FInbetweenerBreakdown* breakdown = pair.Key;
        FOdysseyVectorCell* expectedTargetCell = pair.Value;
        FOdysseyVectorTagInbetweener* inbetweenerTag = breakdown->GetInbetweenerTag();
        FOdysseyVectorCell* sourceCell = inbetweenerTag->GetSourceCell();
        FOdysseyVectorCell* targetCell = breakdown->GetTargetCell();
        FOdysseyVectorLayer* layer = inbetweenerTag->GetOwner()->GetCell()->GetLayer();

        if( targetCell && sourceCell )
        {
            int32 targetCellIndex = targetCell->GetIndex();
            int32 expectedTargetCellIndex = layer->Contains(  expectedTargetCell ) ? expectedTargetCell->GetIndex()
                                                                                   : targetCellIndex;

            if( expectedTargetCell != targetCell )
            {
                int32 sourceCellIndex = sourceCell->GetIndex();

                int32 relativeIndex = ( expectedTargetCellIndex - sourceCellIndex ) * (int32) inbetweenerTag->GetInterpolationDirection();

                if( relativeIndex > 0 )
                {
                    breakdown->SetTargetDrawingIndex( relativeIndex );
                }
            }
        }
        else
        {
            if( inbetweenerTag->GetBreakdownCount() > 1 )
            {
                inbetweenerTag->RemoveBreakdown( breakdown, false );
            }
            else
            {
                breakdown->SetTargetDrawingIndex( 1 );
            }
        }
    }

    mVectorLayer->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    mVectorLayer->RequestRedraw( nullptr, 0 );

    mBreakdownTargetMap.Empty();
}

void
UOdysseyAnimationLayerImageVector::PreEditChange( FProperty* PropertyAboutToChange )
{
    Super::PreEditChange( PropertyAboutToChange );

    if( PropertyAboutToChange->GetName() == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayerImageVector, Cells ) )
    {
        //MakeBreakdownTargetMap();
    }
}

void
UOdysseyAnimationLayerImageVector::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    Super::PostEditChangeProperty( PropertyChangedEvent );

    if( PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayerImageVector, Cells ) )
    {
        //CheckBreakdownTargetMap();
    }
}

void
UOdysseyAnimationLayerImageVector::CellsChanged()
{
    std::list<FOdysseyVectorTagInbetweener*> inbetweenerTagList;

    MakeBreakdownTargetMap( inbetweenerTagList );

    if( inbetweenerTagList.size() )
    {
        FOdysseyVectorUndo *undo = new FOdysseyVectorUndoTagInbetweenerBreakdownAlter( mVectorLayer.Get()
                                                                                     , inbetweenerTagList );

        // needed for valid GUndo pointer
        if( GUndo )
        {
            // We use GEditor as the UObject, otherwise if we use "this", at each UNDO, PostEditChangeProperty() will be called
            // which will again call StoreUndo + this will lead to a crash. I don't know however what will be the consequences
            // of a call to GEditor::PostEditChangeProperty()
            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
/*
            TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
*/
        }

        undo->Begin(); // snapshot before operation

        CheckBreakdownTargetMap();

        undo->End(); // snapshot after operation
    }

    // Put this before calling Super::CellsChanged because the HUD might be refreshed by Super::CellsChanged
    // When reloading the current tool and it needs the vector object hierarchy to be correctly set.
    UpdateSharedEnv();


    Super::CellsChanged();
}
#endif

#undef LOCTEXT_NAMESPACE

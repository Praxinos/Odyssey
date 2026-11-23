// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTextureLayerImageVector.h"
#include "OdysseyRenderContext.h"

#include "OdysseyTextureLayerStack.h"
#include "OdysseyMediaVector.h"
#include "OdysseyTextureLayerImageVectorImport.h"
#include "OdysseyTextureLayerImageVectorExport.h"
// from module OdysseyVector
#include "Import/v1/OdysseyVectorImport.h"
#include "Import/v2/OdysseyVectorImport.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorGroupPaint.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "Misc/TransactionObjectEvent.h"
#include "OdysseyTextureLayerStackUserData.h"

#define LOCTEXT_NAMESPACE "Texture"

UOdysseyTextureLayerImageVector::~UOdysseyTextureLayerImageVector()
{
    if (mVectorCell)
    {
        if( mVectorCell->GetParent() )
        {
            mVectorCell->GetParent()->RemoveChild( mVectorCell.Get() );
        }
    }
}

UOdysseyTextureLayerImageVector::UOdysseyTextureLayerImageVector()
    : mVectorLayer( MakeShared<FOdysseyVectorLayer>(this) )
{
    LayerTypeName = LOCTEXT("layer-image-vector.type", "Vector Layer");
    Icon = FSlateIcon("OdysseyStyle", "OdysseyLayerStack.LayerVector16");
}

void
UOdysseyTextureLayerImageVector::Init( uint32 iWidth, uint32 iHeight )
{
    Width  = iWidth;
    Height = iHeight;

    mVectorCell = MakeShared<FOdysseyVectorCell>( this
                                                , new FOdysseyVectorGroupPaint( "Scene" ) );

    mVectorLayer->AppendChild( mVectorCell.Get() );
}

TSharedPtr<FOdysseyVectorCell>
UOdysseyTextureLayerImageVector::GetVectorCell()
{
    return mVectorCell;
}

TSharedPtr<FOdysseyVectorLayer>
UOdysseyTextureLayerImageVector::GetVectorLayer()
{
    return mVectorLayer;
}

FOdysseyMediaProvider
UOdysseyTextureLayerImageVector::GetMediaProvider(uint32 iFrameIndex) const
{
    bool isActive = IsActivatedRecursively();
    bool isLocked = IsLockedRecursively();

    FOdysseyMediaProvider mediaProvider;
    mediaProvider.IsLocked(!isActive || isLocked);

    TSharedPtr<FOdysseyMediaVector> mediaVector = MakeShared<FOdysseyMediaVector>(mVectorCell->GetScene());
    mediaProvider.Add(mediaVector);
    return mediaProvider;
}


uint64
UOdysseyTextureLayerImageVector::GetDrawingFlags() const
{
    uint64 DrawingFlags = !IsColored() ? FOdysseyVectorEngine::DRAWING_IGNORECOLOR : 0;
    DrawingFlags |= IsWireframe() ? FOdysseyVectorEngine::DRAWING_WIREFRAME : 0;
    return DrawingFlags;
}

FOdysseyVectorContent&
UOdysseyTextureLayerImageVector::GetVectorContent() const
{
    // A new vector cell, e.g. created when loading or duplicating, gets a new content.
    if (!VectorContent || VectorContent->GetCell() != mVectorCell)
    {
        UOdysseyTextureLayerImageVector* MutableThis = const_cast<UOdysseyTextureLayerImageVector*>(this);
        VectorContent = MakeShared<FOdysseyVectorContent>(MutableThis->GetRenderingId(), mVectorCell,
            [MutableThis]()
            {
                FOdysseyVectorContent::FDescription Description;
                UOdysseyTextureLayerStack* LayerStack = Cast<UOdysseyTextureLayerStack>(MutableThis->GetLayerStack());
                if (MutableThis->GetTexture() && LayerStack)
                {
                    Description.Size = MutableThis->GetDefaultRenderRect().Size();
                    // As the texture, the render holds sRGB encoded colors when the texture is sRGB.
                    Description.bSRGB = LayerStack->IsSRGB();
                }
                return Description;
            },
            [MutableThis]()
            {
                return MutableThis->GetDrawingFlags();
            },
            [MutableThis](const TArray<FIntRect>& Rects, bool bInteractive)
            {
                MutableThis->RenderingChanged(Rects, bInteractive);
            });
    }
    return *VectorContent;
}

void
UOdysseyTextureLayerImageVector::ColorSettingsChanged()
{
    // The content renders the scene again for the new sRGB setting when displayed.
    RenderingChanged();
}

void
UOdysseyTextureLayerImageVector::BeginDestroy()
{
    VectorContent.Reset();
    Super::BeginDestroy();
}

void
UOdysseyTextureLayerImageVector::PostInitProperties()
{
    Super::PostInitProperties();

    if (GetFlags() & RF_ClassDefaultObject)
        return;

    UTexture2D* texture = GetTexture();
    if( !texture || texture->Source.GetFormat() == TSF_Invalid )
        return;

    Width  = texture->Source.GetSizeX();
    Height = texture->Source.GetSizeY();

    mVectorCell = MakeShared<FOdysseyVectorCell>( this, new FOdysseyVectorGroupPaint( "Scene" ) );
    mVectorLayer->AppendChild( mVectorCell.Get() );

    // Follows the redraws of the scene from now on; renders it when displayed.
    GetVectorContent();
}

bool
UOdysseyTextureLayerImageVector::BuildRenderPipelineInternal(
    IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
    const TArray<const IOdysseyTextureRenderingAbility*>& iParents, const FOdysseyRenderContext* RenderContext
) const
{
    return GetVectorContent().BuildRenderFunction(oRenderFunction, FMatrix::Identity);
}

void
UOdysseyTextureLayerImageVector::PostLoad()
{
    Super::PostLoad();

    if (GetFlags() & RF_ClassDefaultObject)
        return;

    UTexture2D* texture = GetTexture();
    if( !texture || texture->Source.GetFormat() == TSF_Invalid )
        return;

    // Follows the redraws of the loaded scene; renders it when displayed.
    GetVectorContent();
    mVectorLayer->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    // textures must be assigned to brushes in PostLoad and not in Serialize(), because the UAsset won't be fully loaded
    // and there dimensions would be 0 at that point.
    mImporterV2.PostLoadTextures();
}

void
UOdysseyTextureLayerImageVector::PostDuplicate(EDuplicateMode::Type iDuplicateMode)
{
    Super::PostDuplicate(iDuplicateMode);

    UTexture2D* texture = GetTexture();
    if( !texture || texture->Source.GetFormat() == TSF_Invalid )
        return;

    Width = texture->Source.GetSizeX();
    Height = texture->Source.GetSizeY();
    // The render of the original is not duplicated: the content follows the scene of the duplicate.
    GetVectorContent();
}

FOdysseyVectorImportV2*
UOdysseyTextureLayerImageVector::GetImporterV2()
{
    return &mImporterV2;
}

void
UOdysseyTextureLayerImageVector::Serialize(FArchive& Ar)
{
    Super::Serialize( Ar );

    if( Ar.IsSaving() )
    {
        if ( mVectorCell != nullptr )
        {
            FOdysseyTextureLayerImageVectorExport::Write( this, Ar );
        }
    }

    if( Ar.IsLoading() )
    {
        if ( mVectorCell == nullptr )
        {
            // commented out: at that point, the texture owning the layer stack doe snot have width and height values.
            // This should be changed. As a bypass, I store dimensions in Width and Height UProperties.
            //UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
            //if(!layerStack)
            //    return;
            //UTexture2D* texture = layerStack->GetTexture();

            //Init( texture->Source.GetSizeX(), texture->Source.GetSizeY() );

            Init( Width, Height );
        }

        if (!FOdysseyTextureLayerImageVectorImport::Read( this, Ar ))
        {
            //Old Style Chunk Loading
            uint32 chunkID;
            uint64 chunkLen;
            uint64 chunkEnd;

            // Reads the first chunk (CHUNK_VECTOR_MAGIC)
            Ar << chunkID;
            Ar << chunkLen;

            chunkEnd = Ar.Tell() + chunkLen;

            switch( chunkID )
            {
                case FOdysseyFile::VectorV1::CHUNK_VECTOR_MAGIC_V1 :
                    //UE_LOG(LogTemp, Warning, TEXT("CHUNK_VECTOR_MAGIC_V1") );

                    FOdysseyVectorImportV1::Read( mVectorCell->GetScene(), Ar, chunkEnd );
                break;

                case FOdysseyFile::VectorV2::CHUNK_VECTOR_MAGIC_V2 :
                {
                    //UE_LOG(LogTemp, Warning, TEXT("CHUNK_VECTOR_MAGIC_V2") );

                    mImporterV2.Read( mVectorCell->GetScene(), Ar, chunkEnd );
                }
                break;

                default:
                    Ar.Seek( chunkEnd );
                break;
            }
        }

        mVectorLayer->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    }
}



void
UOdysseyTextureLayerImageVector::SetIsWireframe(bool Value)
{
    if( !IsEditable() )
        return;

    bIsWireframe = Value;
    mVectorCell->GetLayer()->RequestRedraw( mVectorCell.Get(), 0 );
    //RenderingChanged();

    MarkPackageDirty();
}

void
UOdysseyTextureLayerImageVector::SetIsColored(bool Value)
{
    if( !IsEditable() )
        return;

    bIsColored = Value;
    mVectorCell->GetLayer()->RequestRedraw( mVectorCell.Get(), 0 );
    //RenderingChanged();

    MarkPackageDirty();
}

bool
UOdysseyTextureLayerImageVector::IsWireframe() const
{
    return bIsWireframe;
}

bool
UOdysseyTextureLayerImageVector::IsColored() const
{
    return bIsColored;
}



TArray<FGuid>
UOdysseyTextureLayerImageVector::GetRenderingComposition(uint64 iRenderType, int iFrame) const
{
    return { GetRenderingId() };
}

void
UOdysseyTextureLayerImageVector::Merge(const TArray<UOdysseyLayer*>& iLayers)
{
    if( !IsEditable() )
        return;

    Modify();

    FOdysseyVectorGroupPaint* destinationScene = mVectorCell->GetScene();

    for( int i = 0; i < iLayers.Num(); i++ )
    {
        UOdysseyTextureLayerImageVector* vectorLayer = Cast<UOdysseyTextureLayerImageVector>(iLayers[i]);
        if (!vectorLayer)
            continue;

        FOdysseyVectorGroupPaint* scene = vectorLayer->GetVectorCell()->GetScene();

        for( FOdysseyVectorObject* child : scene->GetChildrenList() )
        {
            FOdysseyVectorObject* copiedChild = child->Copy();
            destinationScene->AppendChild( copiedChild );

            scene->CopyBuckets( destinationScene, false );
        }
    }

    destinationScene->UpdateMatrix();

    destinationScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    destinationScene->GetLayer()->RequestRedraw( destinationScene->GetCell(), 0 );
}

// Implements Interface IOdysseyVectorLayer::GetWidth
uint32
UOdysseyTextureLayerImageVector::GetWidth()
{
    return ( uint32 ) Width;
}

// Implements Interface IOdysseyVectorLayer::GetHeight
uint32
UOdysseyTextureLayerImageVector::GetHeight()
{
    return ( uint32 ) Height;
}

// Implements Interface IOdysseyVectorLayer::GetCellByIndex
FOdysseyVectorCell*
UOdysseyTextureLayerImageVector::GetCellByIndex( uint32 iIndex )
{
    return mVectorCell.Get();
}

// Implements Interface IOdysseyVectorLayer::Contains
bool
UOdysseyTextureLayerImageVector::Contains( FOdysseyVectorCell* iCandidateCell )
{
    return ( mVectorCell.Get() == iCandidateCell ) ? true : false;
}

// Implements Interface IOdysseyVectorLayer::GetLastCell
FOdysseyVectorCell*
UOdysseyTextureLayerImageVector::GetLastCell()
{
    return mVectorCell.Get();
}

// Implements Interface IOdysseyVectorLayer::GetLastCell
FOdysseyVectorCell*
UOdysseyTextureLayerImageVector::GetFirstCell()
{
    return mVectorCell.Get();
}

// Implements Interface IOdysseyVectorLayer::GetMaxCellFrom
FOdysseyVectorCell*
UOdysseyTextureLayerImageVector::GetMaxCellFrom( uint32 iIndex )
{
    return mVectorCell.Get();
}

// Implements Interface IOdysseyVectorLayer::GetMinCellFrom
FOdysseyVectorCell*
UOdysseyTextureLayerImageVector::GetMinCellFrom( uint32 iIndex )
{
    return mVectorCell.Get();
}

// Implements Interface IOdysseyVectorCell::GetIndex
int32
UOdysseyTextureLayerImageVector::GetIndex()
{
    return 0;
}

// Implements Interface IOdysseyVectorCell::GetLength
uint32
UOdysseyTextureLayerImageVector::GetLength()
{
    return 1;
}

// Implements Interface IOdysseyVectorCell::GetFrame
uint32
UOdysseyTextureLayerImageVector::GetFrame()
{
    return 0;
}

// Implements Interface IOdysseyVectorLayer::GetPaletteSetID
FGuid
UOdysseyTextureLayerImageVector::GetPaletteSetID( UOdysseyPalette* iPalette )
{
    UOdysseyTextureLayerStackUserData* userData = Cast<UOdysseyTextureLayerStackUserData>(GetTexture()->GetAssetUserDataOfClass(UOdysseyTextureLayerStackUserData::StaticClass()));

    if( userData )
    {
        for( int32 i = 0; i < userData->Palettes.Num(); i++ )
        {
            if( userData->Palettes[i]->mPalette == iPalette )
            {
                return userData->Palettes[i]->mSet;
            }
        }
    }

    return FGuid();
}

// Implements Interface IOdysseyVectorLayer::GetPaletteSetID
const TArray<UOdysseyPaletteSet*>
UOdysseyTextureLayerImageVector::GetPaletteSets() const
{
    TArray<UOdysseyPaletteSet*> paletteSets;
    UOdysseyTextureLayerStackUserData* textureUserData = Cast<UOdysseyTextureLayerStackUserData>(GetTexture()->GetAssetUserDataOfClass(UOdysseyTextureLayerStackUserData::StaticClass()));

    paletteSets = textureUserData->Palettes;

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

void
UOdysseyTextureLayerImageVector::OnRefreshReferencedPalette(UOdysseyPalette* iPalette)
{
    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetMediaProvider(0).GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.IsEmpty())
        return;

    for (int i = 0; i < mediaVectors.Num(); i++)
    {
        FOdysseyVectorCell* vectorCell = mediaVectors[i]->GetScene()->GetCell();
        vectorCell->GetLayer()->RequestRedraw(vectorCell, 0);
    }
}

#undef LOCTEXT_NAMESPACE

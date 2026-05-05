// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTextureLayerImageVector.h"

#include "OdysseyTextureLayerStack.h"
#include "OdysseyPixelFormat.h"
#include "OdysseyMediaVector.h"
#include "OdysseyVectorBlock.h"
#include "ULISLoaderModule.h"
#include "ULISUtils.h"
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
#include "TextureCompiler.h"
#include "UObject/ObjectSaveContext.h"
#include "OdysseyTextureLayerStackUserData.h"

#include "blend2d.h"

#define LOCTEXT_NAMESPACE "Texture"

UOdysseyTextureLayerImageVector::~UOdysseyTextureLayerImageVector()
{
    // bind refresh function to delegates on existing vector scenes at load. Needed to refresh necessary widgets.
    if (mVectorBlock)
        mVectorBlock->OnInvalidated().RemoveAll( this );

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


#if WITH_EDITOR
bool
UOdysseyTextureLayerImageVector::UpdateDrawingFlags() const
{
    uint64 drawingFlags = !IsColored() ? FOdysseyVectorEngine::DRAWING_IGNORECOLOR : 0;
    drawingFlags |= IsWireframe() ? FOdysseyVectorEngine::DRAWING_WIREFRAME : 0;
    bool changed = drawingFlags != mDrawingFlags;
    mDrawingFlags = drawingFlags;
    return changed;
}

void
UOdysseyTextureLayerImageVector::InitTexture()
{
    Super::InitTexture();

    UTexture2D* texture = GetRenderTexture();
    InitTextureWithBlockData(mVectorBlock->GetBlock(mDrawingFlags).Get(), texture, TextureSourceFormatForULISFormat(mVectorBlock->GetFormat()));
    texture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({ texture });
}
#endif

void
UOdysseyTextureLayerImageVector::PostInitProperties()
{
    Super::PostInitProperties();

    if (GetFlags() & RF_ClassDefaultObject)
        return;

    mVectorBlockId = FGuid::NewGuid();
    mVectorBlock = MakeShared<FOdysseyVectorBlock>();
    mVectorBlock->OnInvalidated().AddUObject(this, &UOdysseyTextureLayerImageVector::OnVectorBlockInvalidated);

    UTexture2D* texture = GetTexture();
    if( !texture || texture->Source.GetFormat() == TSF_Invalid )
        return;

    Width  = texture->Source.GetSizeX();
    Height = texture->Source.GetSizeY();

    mVectorCell = MakeShared<FOdysseyVectorCell>( this, new FOdysseyVectorGroupPaint( "Scene" ) );
    mVectorLayer->AppendChild( mVectorCell.Get() );

    ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
    //let's ensure the format has alpha, so add alpha channel of needed
    format = static_cast< ::ULIS::eFormat >(format | ULIS_W_ALPHA( 1 ) );
    mVectorBlock->Init(mVectorBlockId, mVectorCell, Width, Height, format);
}

bool
UOdysseyTextureLayerImageVector::BuildRenderPipelineInternal(
    FFrameNumber iFrame,
    uint64 iType,
    IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
    const IOdysseyTextureRenderingAbility::FCanRenderFunction& iCanRenderFunction,
    const TArray<const IOdysseyTextureRenderingAbility*>& iParents
) const
{
#if WITH_EDITOR
    bool drawingFlagsChanged = UpdateDrawingFlags();
    if ( drawingFlagsChanged )
    {
        CopyVectorBlockInRenderTexture(FIntRect(0, 0, mVectorBlock->GetWidth(), mVectorBlock->GetHeight()));
    }
    if ( mVectorBlock->NeedsRender() )
    {
        mVectorBlock->Render(mDrawingFlags);

        ::ULIS::FRectI rect = mVectorBlock->GetSanitizedRect();
        CopyVectorBlockInRenderTexture(ULISUtils::ToIntRect(rect));
    }
#endif

    return Super::BuildRenderPipelineInternal(iFrame, iType, oRenderFunction, iCanRenderFunction, iParents);
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

    ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
    //let's ensure the format has alpha, so add alpha channel of needed
    format = static_cast< ::ULIS::eFormat >(format | ULIS_W_ALPHA( 1 ) );

    mVectorBlock = MakeShared<FOdysseyVectorBlock>();
    mVectorBlock->Init(mVectorBlockId, mVectorCell, Width, Height, format);
    InitTexture();

    mVectorBlock->OnInvalidated().AddUObject(this, &UOdysseyTextureLayerImageVector::OnVectorBlockInvalidated);
    mVectorLayer->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    CopyVectorBlockInRenderTexture(FIntRect(0, 0, mVectorBlock->GetWidth(), mVectorBlock->GetHeight()));

    // textures must be assigned to brushes in PostLoad and not in Serialize(), because the UAsset won't be fully loaded
    // and there dimensions would be 0 at that point.
    mImporterV2.PostLoadTextures();
}

TSharedPtr<::ULIS::FBlock>
UOdysseyTextureLayerImageVector::GetBlock() const
{
    return mVectorBlock->GetBlock(mDrawingFlags);
}

void
UOdysseyTextureLayerImageVector::PostDuplicate(EDuplicateMode::Type iDuplicateMode)
{
    Super::PostDuplicate(iDuplicateMode);

    UTexture2D* texture = GetTexture();
    if( !texture || texture->Source.GetFormat() == TSF_Invalid )
        return;

    ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
    //let's ensure the format has alpha, so add alpha channel of needed
    format = static_cast< ::ULIS::eFormat >(format | ULIS_W_ALPHA( 1 ) );

    Width = texture->Source.GetSizeX();
    Height = texture->Source.GetSizeY();
    mVectorBlockId = FGuid::NewGuid();
    mVectorBlock->Init(mVectorBlockId, mVectorCell, Width, Height, format);
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

    Modify();

    bIsWireframe = Value;
    mVectorCell->GetLayer()->RequestRedraw( mVectorCell.Get(), 0 );
    //RenderingChanged();
}

void
UOdysseyTextureLayerImageVector::SetIsColored(bool Value)
{
    if( !IsEditable() )
        return;

    Modify();

    bIsColored = Value;
    mVectorCell->GetLayer()->RequestRedraw( mVectorCell.Get(), 0 );
    //RenderingChanged();
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



void
UOdysseyTextureLayerImageVector::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    Super::PostTransacted(iTransactionEvent);

    if ( iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo )
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();

    if (changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyTextureLayerImageVector, bIsColored)))
    {
        mVectorCell->GetLayer()->RequestRedraw( mVectorCell.Get(), 0 );
        RenderingChanged();
    }

    if (changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyTextureLayerImageVector, bIsWireframe)))
    {
        mVectorCell->GetLayer()->RequestRedraw( mVectorCell.Get(), 0 );
        RenderingChanged();
    }
}

TArray<FGuid>
UOdysseyTextureLayerImageVector::GetRenderingComposition(uint64 iRenderType, int iFrame) const
{
    return { GetRenderingId() };
}

void
UOdysseyTextureLayerImageVector::OnVectorBlockInvalidated( const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
    RenderingChanged(::ULISUtils::ToIntRects(iRects), iIsInteractive);
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
UOdysseyTextureLayerImageVector::PreSave(FObjectPreSaveContext SaveContext)
{
    Super::PreSave(SaveContext);

    //Setting mDrawingFlags here ensures the Texture will update correctly on the next call to BuildTextureRenderer()
    //If some drawing flags are needed
    //As UpdateDrawingFlags() will return true and enforce Texture redraw.
    mDrawingFlags = 0;
    InitTexture();
}

#if WITH_EDITOR
void
UOdysseyTextureLayerImageVector::CopyVectorBlockInRenderTexture(FIntRect iRect) const
{
    UTexture2D* texture = GetRenderTexture();
    TSharedPtr<::ULIS::FBlock> vectorBlock = mVectorBlock->GetBlock(mDrawingFlags);
    TSharedPtr<::ULIS::FBlock> block = vectorBlock;

    ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
    if (vectorBlock->Format() != format)
    {
        block = MakeShared<::ULIS::FBlock>(mVectorBlock->GetWidth(), mVectorBlock->GetHeight(), format);

        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
        ctx.ConvertFormat(
            *vectorBlock.Get(),
            *block.Get(),
            ULISUtils::ToULISRectI(iRect),
            ::ULIS::FVec2I(iRect.Min.X, iRect.Min.Y),
            ::ULIS::FSchedulePolicy::AsyncCacheEfficient
        );
        ctx.Finish();
    }

    FOdysseySurfaceTexture2DEditable surface(texture, block);
    surface.Invalidate({ ULISUtils::ToULISRectI(iRect) });
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
#endif

#undef LOCTEXT_NAMESPACE

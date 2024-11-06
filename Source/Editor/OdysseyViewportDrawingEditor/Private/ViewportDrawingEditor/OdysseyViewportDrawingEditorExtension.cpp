// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "ViewportDrawingEditor/OdysseyViewportDrawingEditorExtension.h"

#include "ViewportDrawingEditor/OdysseyViewportDrawingEditorGUI.h"
#include "PainterEditor/OdysseyPainterEditor.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"
#include "LevelEditorSequencerIntegration.h"
#include "MeshPaintAdapterFactory.h"
#include "MediaTexture.h"
#include "MediaPlayer.h"
#include "MediaPlaylist.h"
#include "OdysseyAnimation.h"
#include "ComponentReregisterContext.h"
#include "AnimationEditor/OdysseyAnimationEditorSource.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "Media/OdysseyAnimationMediaPlayer.h"
#include "MediaPlayerFacade.h"
#include "MediaPlate.h"
#include "MediaPlateComponent.h"
#include "OdysseyAnimationPlayer.h"
#include "IOdysseyViewportDrawingEditorAdapter.h"
#include "TextureEditor/OdysseyTextureEditorSource.h"
#include "TextureCompiler.h"
#include "Materials/MaterialExpressionTextureBase.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "OdysseyViewportDrawingEditorTextureBasedAdapter.h"
#include "OdysseyViewportDrawingEditorMeshBasedAdapter.h"
#include "OdysseyViewportDrawingEditorScreenBasedAdapter.h"
#include "IMeshPaintGeometryAdapter.h"
#include "MeshPaintHelpers.h"
#include "OdysseyViewportDrawingEditorUtils.h"
#include "OdysseyAnimationActor.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimationComponentTrack.h"
#include "OdysseyAnimationComponentSection.h"
#include "OdysseyAnimationComponentTemplate.h"
#include "MovieScene.h"

#define LOCTEXT_NAMESPACE "ViewportDrawingEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

FOdysseyViewportDrawingEditorExtension::~FOdysseyViewportDrawingEditorExtension()
{
}

FOdysseyViewportDrawingEditorExtension::FOdysseyViewportDrawingEditorExtension()
    : FOdysseyPainterEditorExtension(nullptr)
    , mGUI(nullptr)
    , mPaintingAdapterMethod(EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyTextureBased)
    , mActor(nullptr)
    , mComponent(nullptr)
    , mMaterial(nullptr)
    , mTexture(nullptr)
    , mCurrentSource(nullptr)
{}

FOdysseyViewportDrawingEditorExtension::FOdysseyViewportDrawingEditorExtension(FOdysseyPainterEditor* iEditor)
    : FOdysseyPainterEditorExtension(iEditor)
    , mGUI(nullptr)
    , mPaintingAdapterMethod(EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyTextureBased)
    , mActor(nullptr)
    , mComponent(nullptr)
    , mMaterial(nullptr)
    , mTexture(nullptr)
    , mCurrentSource(nullptr)
{
}

void
FOdysseyViewportDrawingEditorExtension::Initialize()
{
    //Handle Object Property Changed Callback to refresh when actors's visibility changes for example
    FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(this,&FOdysseyViewportDrawingEditorExtension::OnObjectPropertyChanged);
    GetEditor()->OnSourceChanged().AddRaw(this, &FOdysseyViewportDrawingEditorExtension::OnSourceChanged);

    FLevelEditorSequencerIntegration::Get().GetOnSequencersChanged().AddRaw( this, &FOdysseyViewportDrawingEditorExtension::OnSequencersChanged );
    mSequencers = FLevelEditorSequencerIntegration::Get().GetSequencers();
    SetAllDelegatesSequencers();

    SetPaintingAdapterMethod(EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyTextureBased);

    mGUI = MakeShareable(new FOdysseyViewportDrawingEditorGUI(this));
    mGUI->Initialize();
}

void
FOdysseyViewportDrawingEditorExtension::Finalize()
{
    mGUI->Finalize();

    SetActor(nullptr);

    mPaintingAdapter->SetTexture(nullptr);
    mPaintingAdapter->Finalize();
    mPaintingAdapter = nullptr;

    FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll(this);
    GetEditor()->OnSourceChanged().RemoveAll(this);
    FLevelEditorSequencerIntegration::Get().GetOnSequencersChanged().RemoveAll(this);
    ClearAllDelegatesSequencers();
    mSequencers.Empty();
}

void
FOdysseyViewportDrawingEditorExtension::OnSourceChanged()
{
    TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();

    if (mCurrentSource == source)
        return;

    mPaintingAdapter->SetTexture(nullptr);

    //If the source was a texture, then revert its parameters to their original values
    if (mCurrentSource && mCurrentSource->Id() == FOdysseyTextureEditorSource::StaticId())
    {
        TSharedPtr<FOdysseyTextureEditorSource> textureSource = StaticCastSharedPtr<FOdysseyTextureEditorSource>(mCurrentSource);
        UTexture2D* texture = textureSource->GetTexture();
        if (texture)
        {
            texture->MipGenSettings = mPreviousMipSettings;
            texture->UpdateResource();
            FTextureCompilingManager::Get().FinishCompilation({ texture });
            texture->MarkPackageDirty();
            //TODO: if user quits Unreal without quitting Iliad mode first, the texture stays in NoMipMaps.
            //Not the end of the world, but if users notice it, we may want to dig deeper into this issue.
            //This is a better alternative than forcing the save of the texture though (which was the previous version of this code)
        }
    }

    //If the source was an animation
    if (mCurrentSource && mCurrentSource->Id() == FOdysseyAnimationEditorSource::StaticId())
    {
        TSharedPtr<FOdysseyAnimationEditorSource> animationSource = StaticCastSharedPtr<FOdysseyAnimationEditorSource>(mCurrentSource);
        UOdysseyAnimationPlayer* player = animationSource->GetAnimationPlayer();
        if (player)
        {
            player->OnCurrentTimeChanged().RemoveAll(this);
        }
    }

    mCurrentSource = source;
    if (!source)
        return;

    //If the source is a texture, then change its parameters for display reasons
    if (mCurrentSource->Id() == FOdysseyTextureEditorSource::StaticId())
    {
        TSharedPtr<FOdysseyTextureEditorSource> textureSource = StaticCastSharedPtr<FOdysseyTextureEditorSource>(mCurrentSource);
        UTexture2D* texture = textureSource->GetTexture();
        if (texture)
        {
            mPreviousMipSettings = texture->MipGenSettings;
            texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
            texture->UpdateResource();
            FTextureCompilingManager::Get().FinishCompilation({ texture });
            texture->MarkPackageDirty();
        }
    }

    //If the source is an animation
    if (mCurrentSource && mCurrentSource->Id() == FOdysseyAnimationEditorSource::StaticId())
    {
        TSharedPtr<FOdysseyAnimationEditorSource> animationSource = StaticCastSharedPtr<FOdysseyAnimationEditorSource>(mCurrentSource);
        UOdysseyAnimationPlayer* player = animationSource->GetAnimationPlayer();
        if (player)
        {
            player->OnCurrentTimeChanged().AddRaw(this, &FOdysseyViewportDrawingEditorExtension::OnAnimationPlayerCurrentTimeChanged);
        }
    }

    mPaintingAdapter->SetTexture(Texture());
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

AActor*
FOdysseyViewportDrawingEditorExtension::Actor() const
{
    return mActor;
}

UMeshComponent*
FOdysseyViewportDrawingEditorExtension::Component() const
{
    return mComponent;
}

UMaterialInterface*
FOdysseyViewportDrawingEditorExtension::Material() const
{
    return mMaterial;
}

UTexture*
FOdysseyViewportDrawingEditorExtension::Texture() const
{
    return mTexture;

    /* if (!mCurrentSource || mCurrentSource->Id() != FOdysseyTextureEditorSource::StaticId())
        return nullptr;

    TSharedPtr<FOdysseyTextureEditorSource> textureSource = StaticCastSharedPtr<FOdysseyTextureEditorSource>(mCurrentSource);
    return textureSource->GetTexture(); */
}

IOdysseyViewportDrawingEditorAdapter*
FOdysseyViewportDrawingEditorExtension::GetOdysseyViewportDrawingEditorAdapter()
{
    return mPaintingAdapter.Get();
}

const TArray<UMeshComponent*>&
FOdysseyViewportDrawingEditorExtension::SelectableComponents() const
{
    return mSelectableComponents;
}

void
FOdysseyViewportDrawingEditorExtension::SelectableMaterials( TArray<UMaterialInterface*>& ioSelectableMaterials ) const
{
    if ( mComponent )
    {
        mComponent->GetUsedMaterials( ioSelectableMaterials );
    }
}

const TArray<FPaintableTexture>&
FOdysseyViewportDrawingEditorExtension::SelectableTextures() const
{
    return mSelectableTextures;
}

const TMap<TObjectPtr<UMeshComponent>, TSharedPtr<IMeshPaintGeometryAdapter>>&
FOdysseyViewportDrawingEditorExtension::ComponentToAdapterMap() const
{
    return mComponentToAdapterMap;
}

EOdysseyViewportDrawingPaintingAdapterMethod FOdysseyViewportDrawingEditorExtension::PaintingAdapterMethod() const
{
    return mPaintingAdapterMethod;
}

//TODO: Move to adapter
int32 FOdysseyViewportDrawingEditorExtension::GetUVIndexUsedByCurrentTexture()
{
    if (mMaterial != NULL && mMaterial->GetMaterial() != NULL)
    {
        for (UMaterialExpression* expression : mMaterial->GetMaterial()->GetExpressions())
        {
            UMaterialExpressionTextureBase* TextureBase = Cast<UMaterialExpressionTextureBase>(expression);
            if (TextureBase != NULL &&
                TextureBase->Texture != NULL &&
                TextureBase->Texture == Texture() )
            {
                UMaterialExpressionTextureSample* TextureSample = Cast<UMaterialExpressionTextureSample>(expression);
                if (TextureSample != NULL)
                {
                    UMaterialExpressionTextureCoordinate* TextureCoords = Cast<UMaterialExpressionTextureCoordinate>(TextureSample->Coordinates.Expression);
                    if (TextureCoords != NULL)
                    {
                        return TextureCoords->CoordinateIndex;
                    }
                    else
                    {
                        return TextureSample->ConstCoordinate;
                    }
                }
            }
        }
    }
    return 0;
}

//TODO: Move to adapter
float FOdysseyViewportDrawingEditorExtension::GetMeshComponentMaxSize() const
{
    if (mComponent)
    {
        FVector extent = mComponent->GetLocalBounds().BoxExtent;
        FVector scale = mActor->GetActorScale();
        return FMath::Max3(extent.X, extent.Y, extent.Z) * FMath::Max3(scale.X, scale.Y, scale.Z);
    }
    return 1;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
FOdysseyViewportDrawingEditorExtension::SetActor(AActor* iActor)
{
    if (mActor == iActor)
        return;

    // Clear everything before changing actor
    ClearSelectableComponents(); //also clear selected component / texture and selectable textures

    // Change the selected Actor
    mActor = iActor;

    // Refresh Selectable Components/Textures if needed
    if (mActor)
    {
        UpdateSelectableComponents();
        SelectDefaultComponent(); //Also Selects the default texture if needed
    }
}

void
FOdysseyViewportDrawingEditorExtension::SetComponent(UMeshComponent* iComponent)
{
    if (mComponent == iComponent)
        return;

    // Save Component Paint Settings
    if (mComponent)
    {
        FInstanceTexturePaintSettings& texturePaintSettings = mComponentToTexturePaintSettingsMap.FindOrAdd(mComponent);
        texturePaintSettings.mSelectedTexture = Texture();
    }

    // Change the selected component
    mComponent = iComponent;
    SelectDefaultMaterial();
}

void
FOdysseyViewportDrawingEditorExtension::SetMaterial(UMaterialInterface* iMaterial)
{
    if( iMaterial == mMaterial )
        return;

    mMaterial = iMaterial;
    ClearSelectableTextures(); // Also clears selected Texture
    UpdateSelectableTextures();

    if (mMaterial)
        SelectDefaultTexture();
}

bool
FOdysseyViewportDrawingEditorExtension::SetTexture(UTexture* iTexture, bool iWarnUserIfFailed)
{
    if (iTexture == mTexture)
        return true;

    if (iTexture)
    {
        bool canSetTexture = true;
        bool isMediaTexture = false;

        UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
        if (iTexture->IsA(UMediaTexture::StaticClass()))
        {
            isMediaTexture = true;
            canSetTexture = false;
            UMediaTexture* texture = Cast<UMediaTexture>(iTexture);
            UMediaPlayer* mediaPlayer = texture->GetMediaPlayer();
            if (mediaPlayer)
            {
                UMediaPlaylist& playlist = mediaPlayer->GetPlaylistRef();
                int32 index = mediaPlayer->GetPlaylistIndex();
                UMediaSource* mediaSource = playlist.Get(index);
                if (mediaSource && mediaSource->IsA(UOdysseyAnimation::StaticClass()))
                {
                    UOdysseyAnimation* animation = Cast<UOdysseyAnimation>(mediaSource);
                    canSetTexture = AssetEditorSubsystem->FindEditorForAsset(animation, true) == nullptr;
                }
            }

            if (!canSetTexture)
            {
                if (mActor->GetClass() == AMediaPlate::StaticClass())
                {
                    bool needsOpen = true;
                    AMediaPlate* mediaPlate = Cast<AMediaPlate>(mActor);
                    UMediaSource* mediaSource = mediaPlate->MediaPlateComponent->MediaPlaylist->Get(0);
                    if (mediaSource && mediaSource->IsA(UOdysseyAnimation::StaticClass()))
                    {
                        UOdysseyAnimation* animation = Cast<UOdysseyAnimation>(mediaSource);
                        canSetTexture = AssetEditorSubsystem->FindEditorForAsset(animation, true) == nullptr;
                    }
                }
            }
        }
        else if (iTexture->IsA(UTexture2D::StaticClass()) && AssetEditorSubsystem->FindEditorForAsset(iTexture, true) != nullptr)
        {
            canSetTexture = false;
        }

        if (!canSetTexture)
        {
            if (iWarnUserIfFailed && !isMediaTexture)
                FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("painter-editor-extension.texture-already-opened-dialog.message", "The selected texture is already opened in an other editor. Please close the editor before selecting this texture."), LOCTEXT("painter-editor-extension.texture-already-opened-dialog.title", "Selected Texture Already Opened"));
            else if (iWarnUserIfFailed && isMediaTexture)
                FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("painter-editor-extension.media-texture-fail-dialog.message", "A media texture doesn't have a media source and therefore can't be edited. Please setup all your media textures correctly by assigning them a media source."), LOCTEXT("painter-editor-extension.media-texture-fail-dialog.title", "Media source missing") );

            return false;
        }
    }

    SetTextureInternal(iTexture);

    return true;
}

void
FOdysseyViewportDrawingEditorExtension::SetTextureInternal(UTexture* iTexture)
{

    //Cleanup previous texture if it exist
    if (mTexture)
    {
        if (mTexture->IsA(UMediaTexture::StaticClass()))
        {
            UMediaTexture* texture = Cast<UMediaTexture>(mTexture);
            UMediaPlayer* mediaPlayer = texture->GetMediaPlayer();
            if (mediaPlayer)
            {
                UMediaPlaylist& playlist = mediaPlayer->GetPlaylistRef();
                int32 index = mediaPlayer->GetPlaylistIndex();
                UMediaSource* mediaSource = playlist.Get(index);
                if (mediaSource && mediaSource->IsA(UOdysseyAnimation::StaticClass()))
                {
                    TSharedRef<FMediaPlayerFacade> mediaPlayerFacade = mediaPlayer->GetPlayerFacade();
                    TSharedPtr<FOdysseyAnimationMediaPlayer> animationMediaPlayer = StaticCastSharedPtr<FOdysseyAnimationMediaPlayer>(mediaPlayerFacade->GetPlayer());
                    animationMediaPlayer->SetRenderType(IOdysseyImageRenderer::eRenderType::Render);
                    animationMediaPlayer->UnsetFrameToIncludeIntoDuration();
                }
            }
        }
    }

    mTexture = nullptr;
    mEditor->SetSource(nullptr);

    if (!iTexture)
        return;

    mTexture = iTexture;

    if (mTexture->IsA(UTexture2D::StaticClass()))
    {
        if (mComponent->IsA<UOdysseyAnimationComponent>())
        {
            UOdysseyAnimationComponent* animationComponent = Cast<UOdysseyAnimationComponent>(mComponent);
            if (!animationComponent)
            return;

            TSharedPtr<FOdysseyAnimationEditorSource> animationSource = MakeShared<FOdysseyAnimationEditorSource>(animationComponent->Animation);
            animationSource->SetExternalPlayer(animationComponent->GetActivePlayer());
            mEditor->SetSource(animationSource);
        }
        else
        {
        UTexture2D* texture = Cast<UTexture2D>(mTexture);

        //TODO: change the texture for display

        TSharedPtr<FOdysseyTextureEditorSource> source = MakeShared<FOdysseyTextureEditorSource>(texture);
        mEditor->SetSource(source);
        }
    }

    if (mTexture->IsA(UMediaTexture::StaticClass()))
    {
        UMediaTexture* texture = Cast<UMediaTexture>(mTexture);
        bool wasOpened = EnsureMediaPlateIsOpened();

        UMediaPlayer* mediaPlayer = texture->GetMediaPlayer();
        if (mediaPlayer)
        {
            UMediaPlaylist& playlist = mediaPlayer->GetPlaylistRef();
            int32 index = mediaPlayer->GetPlaylistIndex();
            UMediaSource* mediaSource = playlist.Get(index);
            if (mediaSource && mediaSource->IsA(UOdysseyAnimation::StaticClass()))
            {
                TSharedRef<FMediaPlayerFacade> mediaPlayerFacade = mediaPlayer->GetPlayerFacade();
                TSharedPtr<FOdysseyAnimationMediaPlayer> animationMediaPlayer = StaticCastSharedPtr<FOdysseyAnimationMediaPlayer>(mediaPlayerFacade->GetPlayer());
                animationMediaPlayer->SetRenderType(IOdysseyImageRenderer::eRenderType::Editor);

                UOdysseyAnimation* animation = Cast<UOdysseyAnimation>(mediaSource);
                TSharedPtr<FOdysseyAnimationEditorSource> animationSource = MakeShared<FOdysseyAnimationEditorSource>(animation);
                mEditor->SetSource(animationSource);
            }
        }
        else
        {
            //TODO: Warn user to add a media player to its mediatexture
        }

        if (wasOpened)
            SyncMediaPlayerWithAnimationCurrentFrame();
    }
}

void
FOdysseyViewportDrawingEditorExtension::SyncSequencerWithAnimationPlayer()
{
    if (!mComponent->IsA<UOdysseyAnimationComponent>())
        return;

    UOdysseyAnimationComponent* animationComponent = Cast<UOdysseyAnimationComponent>(mComponent);
    if (!animationComponent)
        return;

    UOdysseyAnimation* animation = animationComponent->GetActiveAnimation();
    if (!animation)
        return;

    UOdysseyAnimationPlayer* player = animationComponent->GetActivePlayer();
    if (!player)
        return;

    for (TWeakPtr<ISequencer> weakSequencer : mSequencers)
    {
        TSharedPtr<ISequencer> sequencer = weakSequencer.Pin();
        if (!sequencer)
            continue;

        UMovieSceneSequence* movieSceneSequence = sequencer->GetFocusedMovieSceneSequence();
        if (!movieSceneSequence)
            continue;

        FGuid binding = sequencer->GetHandleToObject(animationComponent, false);
        if (!binding.IsValid())
            continue;

        UMovieScene* movieScene = movieSceneSequence->GetMovieScene();
        if (!movieScene)
            continue;

        UOdysseyAnimationComponentTrack* track =  movieScene->FindTrack<UOdysseyAnimationComponentTrack>(binding);
        if (!track)
            continue;

        TArray<UMovieSceneSection*> sections = track->GetAllSections();

        //Using FindLastByPredicate instead of FindByPredicate to manage the case where sections are overlapping each other
        //In that case, the last section should be used to synchronize the sequencer and the animation
        int sectionIndex = sections.FindLastByPredicate(
            [sequencer](UMovieSceneSection* iSection)
            {
                if (!iSection->IsActive())
                    return false;

                if (!iSection->IsTimeWithinSection(sequencer->GetLocalTime().Time.FrameNumber))
                    return false;

                return true;
            }
        );

        if (sectionIndex == INDEX_NONE)
            continue;

        UOdysseyAnimationComponentSection* section = Cast<UOdysseyAnimationComponentSection>(sections[sectionIndex]);
        if (!section)
            continue;

        int animationDisplayedFrame = animation->GetFrameIndexAtTime(player->GetCurrentTime());

        FFrameRate tickResolution = movieScene->GetTickResolution();
        FFrameRate displayRate = sequencer->GetFocusedDisplayRate();

        TRange<FFrameNumber> sectionRange = section->GetTrueRange();
        FFrameNumber sectionStartFrame = sectionRange.GetLowerBoundValue();
        FFrameNumber sectionEndFrame = FFrameRate::TransformTime(FFrameRate::TransformTime(sectionRange.GetUpperBoundValue() - 1, tickResolution, displayRate).FloorToFrame(), displayRate, tickResolution).FrameNumber;

        double totalSeconds = animationDisplayedFrame / animation->FramesPerSecond;
        FFrameNumber frame = tickResolution.AsFrameNumber(totalSeconds) + sectionStartFrame;
        double totalSecondsNext = (animationDisplayedFrame + 1) / animation->FramesPerSecond;
        FFrameNumber frameNext = tickResolution.AsFrameNumber(totalSecondsNext) + sectionStartFrame;

        FFrameTime currentFrame = FFrameRate::TransformTime(FFrameRate::TransformTime(sequencer->GetLocalTime().Time.FrameNumber, tickResolution, displayRate).FloorToFrame(), displayRate, tickResolution);

        if (currentFrame >= frame && currentFrame < frameNext)
            return;

        if (frame < sectionStartFrame)
        {
            FOdysseyAnimationComponentSectionParams params;
            params.SectionStartFrame = sectionStartFrame;
            params.SectionEndFrame = sectionEndFrame;
            params.StartFrameOffset = section->StartFrameOffset;
            TRange<FFrameTime> range = TRange<FFrameTime>::Inclusive(sectionStartFrame, sectionStartFrame);
            FOdysseyAnimationComponentTemplate::EvaluateImmediate(animationComponent, range, params, tickResolution);
            return;
        }
        if (frame > sectionEndFrame)
        {
            FOdysseyAnimationComponentSectionParams params;
            params.SectionStartFrame = sectionStartFrame;
            params.SectionEndFrame = sectionEndFrame;
            params.StartFrameOffset = section->StartFrameOffset;
            TRange<FFrameTime> range = TRange<FFrameTime>::Inclusive(sectionEndFrame, sectionEndFrame);
            FOdysseyAnimationComponentTemplate::EvaluateImmediate(animationComponent, range, params, tickResolution);
            return;
        }

        frame = FMath::Clamp(frame, sectionStartFrame, sectionEndFrame);
        FFrameTime intervalNextFrame = FFrameRate::TransformTime(FFrameRate::TransformTime(frame, tickResolution, displayRate).CeilToFrame(), displayRate, tickResolution);
        FFrameTime intervalPrevFrame = FFrameRate::TransformTime(FFrameRate::TransformTime(frame, tickResolution, displayRate).FloorToFrame(), displayRate, tickResolution);

        if (frameNext >= intervalNextFrame)
        {
            sequencer->SetLocalTime(intervalNextFrame, STM_Interval);
        }
        else
        {
            sequencer->SetLocalTime(intervalPrevFrame, STM_Interval);
        }
    }
}

bool
FOdysseyViewportDrawingEditorExtension::EnsureMediaPlateIsOpened()
{
    if (!mTexture->IsA(UMediaTexture::StaticClass()))
        return false;

    if (mActor->GetClass() != AMediaPlate::StaticClass())
        return false;

    UMediaTexture* texture = Cast<UMediaTexture>(mTexture);

    bool needsOpen = true;
    AMediaPlate* mediaPlate = Cast<AMediaPlate>(mActor);
    UMediaPlayer* mediaPlayer = texture->GetMediaPlayer();
    if (mediaPlayer)
    {
        UMediaPlaylist& playlist = mediaPlayer->GetPlaylistRef();
        int32 index = mediaPlayer->GetPlaylistIndex();
        UMediaSource* mediaSource = playlist.Get(index);
        if (mediaSource && mediaPlate->MediaPlateComponent->IsMediaPlatePlaying()) //Is the mediaplate closed ?
            return false;
    }

    bool bPlayOnOpen = mediaPlate->MediaPlateComponent->bPlayOnOpen;
    mediaPlate->MediaPlateComponent->bPlayOnOpen = false;
    mediaPlate->MediaPlateComponent->Open();
    mediaPlate->MediaPlateComponent->bPlayOnOpen = bPlayOnOpen;
    return true;
}

void
FOdysseyViewportDrawingEditorExtension::Tick(float iDeltaTime)
{
    if (mTexture && mTexture->IsA(UMediaTexture::StaticClass()))
    {
        UMediaTexture* texture = Cast<UMediaTexture>(mTexture);

        bool wasOpened = EnsureMediaPlateIsOpened();

        UMediaPlayer* mediaPlayer = texture->GetMediaPlayer();
        if (mediaPlayer) //If the mediaplayer has been removed, remove the source
        {
            //Ensures the animationMediaPlayer
            UMediaPlaylist& playlist = mediaPlayer->GetPlaylistRef();
            int32 index = mediaPlayer->GetPlaylistIndex();
            UMediaSource* mediaSource = playlist.Get(index);
            if (mediaSource && mediaSource->IsA(UOdysseyAnimation::StaticClass()))
            {
                TSharedRef<FMediaPlayerFacade> mediaPlayerFacade = mediaPlayer->GetPlayerFacade();
                TSharedPtr<FOdysseyAnimationMediaPlayer> animationMediaPlayer = StaticCastSharedPtr<FOdysseyAnimationMediaPlayer>(mediaPlayerFacade->GetPlayer());
                animationMediaPlayer->SetRenderType(IOdysseyImageRenderer::eRenderType::Editor);

                if (!mCurrentSource)
                {
                    UOdysseyAnimation* animation = Cast<UOdysseyAnimation>(mediaSource);
                    TSharedPtr<FOdysseyAnimationEditorSource> animationSource = MakeShared<FOdysseyAnimationEditorSource>(animation);
                    mEditor->SetSource(animationSource);
                }

                if (wasOpened)
                {
                    SyncMediaPlayerWithAnimationCurrentFrame();
                }
                else
                {
                    SyncAnimationCurrentFrameWithMediaPlayer();
                }
            }
        }
        else if (mCurrentSource)
        {
            mEditor->SetSource(nullptr);
        }
    }
}

void FOdysseyViewportDrawingEditorExtension::SetPaintingAdapterMethod(EOdysseyViewportDrawingPaintingAdapterMethod iNewMethod)
{
    if (mPaintingAdapter)
        mPaintingAdapter->Finalize();

    mPaintingAdapterMethod = iNewMethod;

    switch (mPaintingAdapterMethod)
    {
        case EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyTextureBased :
            mPaintingAdapter = MakeShared<FOdysseyViewportDrawingEditorTextureBasedAdapter>(this);
        break;
        case EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyScreenBased:
            mPaintingAdapter = MakeShared<FOdysseyViewportDrawingEditorScreenBasedAdapter>(this);
        break;
        case EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyMeshBasedPlanar:
            mPaintingAdapter = MakeShared<FOdysseyViewportDrawingEditorMeshBasedAdapter>(this);
        break;
        default: mPaintingAdapter = nullptr; break;
    }

    mPaintingAdapter->Initialize();
    mPaintingAdapter->SetTexture(Texture());
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Private Methods

void
FOdysseyViewportDrawingEditorExtension::OnObjectPropertyChanged(UObject* iObject, struct FPropertyChangedEvent& iPropertyChangedEvent)
{
    // For now we do not know how to manage actors/components visibility in the viewport
    // This method is called when we change the "Rendering>Visible" property of an actor/component
    // But it is not called when just hidding the actor from the world outliner and we don't know how to detect that properly
    // So for now, a hidden actor/component can still be selected for texture edition
    // Having nothing shown in the viewport in that case makes it viable

    //TODO: Get the visibility value
    //AActor* actor = Cast<AActor>(iObject);
    /*if ( actor == mActor &&
        iPropertyChangedEvent.Property &&
        iPropertyChangedEvent.Property->GetName() == USceneComponent::GetVisiblePropertyName().ToString())
    {
        Refresh();
    }*/
}

//TODO: Remove when Painter will be merged into Extension
FOdysseyViewportDrawingEditorExtension::FOdysseyPaintingAdapterChanged& FOdysseyViewportDrawingEditorExtension::AdapterChangedDelegate()
{
    return mAdapterChangedDelegate;
}

void
FOdysseyViewportDrawingEditorExtension::ClearSelectableComponents()
{
    SetComponent(nullptr);
    mSelectableComponents.Empty();
    for (auto meshAdapterPair : mComponentToAdapterMap)
    {
        meshAdapterPair.Value->OnRemoved();
    }
    mComponentToAdapterMap.Empty();
    FMeshPaintAdapterFactory::CleanupGlobals();
}

void
FOdysseyViewportDrawingEditorExtension::UpdateSelectableComponents()
{
    //make sure the array in empty
    ClearSelectableComponents();

    if (!mActor)
        return;

    // Get Actor's components
    TArray<UMeshComponent*> actorComponents;
    TInlineComponentArray<UMeshComponent*> inlineActorComponents;
    mActor->GetComponents(actorComponents);
    for (UMeshComponent* Component : actorComponents)
    {
        actorComponents.AddUnique(Component); // Make sur to have unique components in the array
    }

    // Fill mSelectable Components with visible components and prepare mesh for texture edition
    TUniquePtr< FComponentReregisterContext > ComponentReregisterContext; //ES: I don't know what this is
    for (UMeshComponent* meshComponent : actorComponents)
    {
        TSharedPtr<IMeshPaintGeometryAdapter> meshAdapter = FMeshPaintAdapterFactory::CreateAdapterForMesh(meshComponent, 0); //ES: I don't know what this is
        if (meshComponent->IsVisible() && meshAdapter.IsValid() && meshAdapter->IsValid())
        {
            mSelectableComponents.Add(meshComponent);
            mComponentToAdapterMap.Add(meshComponent, meshAdapter);
            meshAdapter->OnAdded();
            MeshPaintHelpers::ForceRenderMeshLOD(meshComponent, 0);
            ComponentReregisterContext.Reset(new FComponentReregisterContext(meshComponent)); //ES: I don't know what this does
        }
    }
}

void
FOdysseyViewportDrawingEditorExtension::SelectDefaultComponent()
{
    //TODO: make it smarter, like SelectDefaultTexture does
    // for now we only select the first component of the selected actor

    if (!mActor)
        return;

    if (mSelectableComponents.Num() <= 0)
        return;

    SetComponent(mSelectableComponents[0]);
}

void
FOdysseyViewportDrawingEditorExtension::ClearSelectableTextures()
{
    SetTexture(nullptr, false);
    mSelectableTextures.Empty();
}

void
FOdysseyViewportDrawingEditorExtension::UpdateSelectableTextures()
{
    //make sure the array in empty
    ClearSelectableTextures();

    if (!mComponent)
        return;

    TSharedPtr<IMeshPaintGeometryAdapter> adapter = mComponentToAdapterMap.FindChecked(mComponent);
    FOdysseyViewportDrawingEditorUtils::RetrieveTexturesForComponent(mComponent, mSelectableTextures);
}

void
FOdysseyViewportDrawingEditorExtension::SelectDefaultTexture()
{
    if (!mComponent)
        return;

    if (mSelectableTextures.Num() <= 0)
        return;

    bool displayWarning = true;

    //try to select the previously selected texture for the selected component
    FInstanceTexturePaintSettings& texturePaintSettings = mComponentToTexturePaintSettingsMap.FindOrAdd(mComponent);
    if (texturePaintSettings.mSelectedTexture && mSelectableTextures.Contains(texturePaintSettings.mSelectedTexture))
    {
        if (texturePaintSettings.mSelectedTexture == Texture()) //if the texture is already selected we assume we have nothing to do
            return;

        bool succeeded = SetTexture(texturePaintSettings.mSelectedTexture, displayWarning);
        if (succeeded)
            return;

        displayWarning = false;
    }
    else
    {
        texturePaintSettings.mSelectedTexture = nullptr;
    }

    // select the first texture available for edition
    for (FPaintableTexture& paintableTexture : mSelectableTextures)
    {
        if (paintableTexture.Texture == Texture()) //if the texture is already selected we assume we have nothing to do
            break;

        if( !DoesMaterialUseTexture( mMaterial, paintableTexture.Texture ) )
            continue;

        bool succeeded = SetTexture(paintableTexture.Texture, displayWarning);
        if (succeeded)
            return;

        displayWarning = false;
    }
}

void
FOdysseyViewportDrawingEditorExtension::SelectDefaultMaterial()
{
    if (!mComponent)
        SetMaterial(nullptr);
    else
    {
        TArray<UMaterialInterface*> materialsArray;
        SelectableMaterials( materialsArray );
        if ( materialsArray.Num() > 0)
            SetMaterial( materialsArray[0] );
    }
}

TArray<TWeakPtr<ISequencer>>
FOdysseyViewportDrawingEditorExtension::Sequencers() const
{
    return mSequencers;
}

void
FOdysseyViewportDrawingEditorExtension::OnSequencersChanged()
{
    ClearAllDelegatesSequencers();

    mSequencers = FLevelEditorSequencerIntegration::Get().GetSequencers();

    SetAllDelegatesSequencers();
}

void
FOdysseyViewportDrawingEditorExtension::SetAllDelegatesSequencers()
{
    for (int i = 0; i < mSequencers.Num(); i++)
    {
        if( TSharedPtr<ISequencer> itSequencer =  mSequencers[i].Pin() )
        {
            //When we're playing or scrubbing, we delete the delegates of the sequencer with DisableDelegatesSequencer
            itSequencer->OnBeginScrubbingEvent().AddRaw(this, &FOdysseyViewportDrawingEditorExtension::DisableDelegatesSequencer);
            itSequencer->OnPlayEvent().AddRaw(this, &FOdysseyViewportDrawingEditorExtension::DisableDelegatesSequencer);

            //When we're done, we put them back with EnableDelegatesSequencer
            itSequencer->OnEndScrubbingEvent().AddRaw(this, &FOdysseyViewportDrawingEditorExtension::EnableDelegatesSequencer);
            itSequencer->OnStopEvent().AddRaw(this, &FOdysseyViewportDrawingEditorExtension::EnableDelegatesSequencer);

            //By default, they are enabled
            itSequencer->OnGlobalTimeChanged().AddRaw(this, &FOdysseyViewportDrawingEditorExtension::OnSyncPaintingWithSequencer);
            itSequencer->OnMovieSceneDataChanged().AddRaw( this, &FOdysseyViewportDrawingEditorExtension::OnSyncPaintingWithSequencerMovieSceneChanged );
        }
    }
}

void
FOdysseyViewportDrawingEditorExtension::ClearAllDelegatesSequencers()
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

void
FOdysseyViewportDrawingEditorExtension::OnSyncPaintingWithSequencer()
{
    if (!mCurrentSource || mCurrentSource->Id() != FOdysseyTextureEditorSource::StaticId())
        return;

    //Security, in case we're flipping in sequencer while drawing at the same time
    if( Texture() )
        mPaintingAdapter->FinishPainting();

    if (Component())
    {
        TArray<UMaterialInterface*> selectableMaterials;
        Component()->GetUsedMaterials(selectableMaterials);
        if (selectableMaterials.Num() > 0)
        {
            if (selectableMaterials.Find(Material()) == INDEX_NONE)
            {
                SetMaterial(selectableMaterials[0]);
            }
        }
    }
}

void FOdysseyViewportDrawingEditorExtension::OnSyncPaintingWithSequencerMovieSceneChanged(EMovieSceneDataChangeType iChangedType)
{
    OnSyncPaintingWithSequencer();
}

void FOdysseyViewportDrawingEditorExtension::DisableDelegatesSequencer()
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

void FOdysseyViewportDrawingEditorExtension::EnableDelegatesSequencer()
{
    for (int i = 0; i < mSequencers.Num(); i++)
    {
        if (TSharedPtr<ISequencer> itSequencer = mSequencers[i].Pin())
        {
            itSequencer->OnGlobalTimeChanged().AddRaw(this, &FOdysseyViewportDrawingEditorExtension::OnSyncPaintingWithSequencer);
            itSequencer->OnMovieSceneDataChanged().AddRaw(this, &FOdysseyViewportDrawingEditorExtension::OnSyncPaintingWithSequencerMovieSceneChanged);
        }
    }
    OnSyncPaintingWithSequencer();
}

void
FOdysseyViewportDrawingEditorExtension::OnAnimationPlayerCurrentTimeChanged()
{
    SyncMediaPlayerWithAnimationPlayer();
    SyncSequencerWithAnimationPlayer();
}

void
FOdysseyViewportDrawingEditorExtension::SyncMediaPlayerWithAnimationFrame(int iFrame)
{
    if (!mCurrentSource || mCurrentSource->Id() != FOdysseyAnimationEditorSource::StaticId())
        return;

    if (!mTexture || !mTexture->IsA(UMediaTexture::StaticClass()))
        return;

    TSharedPtr<FOdysseyAnimationEditorSource> animationSource = StaticCastSharedPtr<FOdysseyAnimationEditorSource>(mCurrentSource);
    UOdysseyAnimation* animation = animationSource->GetAnimation();
    if (!animation)
        return;

    UMediaTexture* texture = Cast<UMediaTexture>(mTexture);
    UMediaPlayer* mediaPlayer = texture->GetMediaPlayer();
    if (!mediaPlayer || mediaPlayer->IsPlaying())
        return;

    UMediaPlaylist& playlist = mediaPlayer->GetPlaylistRef();
    int32 index = mediaPlayer->GetPlaylistIndex();
    UMediaSource* mediaSource = playlist.Get(index);
    if ( !mediaSource || !mediaSource->IsA(UOdysseyAnimation::StaticClass()) )
        return;

    TSharedRef<FMediaPlayerFacade> mediaPlayerFacade = mediaPlayer->GetPlayerFacade();
    TSharedPtr<FOdysseyAnimationMediaPlayer> animationMediaPlayer = StaticCastSharedPtr<FOdysseyAnimationMediaPlayer>(mediaPlayerFacade->GetPlayer());
    TSharedPtr<FOdysseyAnimationMediaControls> animationMediaControls = animationMediaPlayer->GetAnimationControls();

    FTimespan frameDuration = FTimespan::FromSeconds(1 / animation->GetFramesPerSecond());

    animationMediaPlayer->SetFrameToIncludeIntoDuration(iFrame);

    //ensure timespan is aligne on the start of a frame
    FTimespan timespan = animation->GetFrameTimeRange(iFrame).GetLowerBoundValue();

    FInt32Range frameRange = animationMediaControls->GetFrameRange();
    FTimespan startTime = animation->GetFrameTimeRange(frameRange.GetLowerBoundValue()).GetLowerBoundValue();

    timespan -= startTime;
    mediaPlayer->Seek(timespan);
    mediaPlayer->SetBlockOnTimeRange(TRange<FTimespan>(timespan, timespan + frameDuration));
}

void
FOdysseyViewportDrawingEditorExtension::SyncMediaPlayerWithAnimationPlayer()
{
    if (!mCurrentSource || mCurrentSource->Id() != FOdysseyAnimationEditorSource::StaticId())
        return;

    TSharedPtr<FOdysseyAnimationEditorSource> animationSource = StaticCastSharedPtr<FOdysseyAnimationEditorSource>(mCurrentSource);
    UOdysseyAnimation* animation = animationSource->GetAnimation();
    UOdysseyAnimationPlayer* player = animationSource->GetAnimationPlayer();
    if (!animation || !player)
        return;

    FTimespan timespan = player->GetCurrentTime();

    //1
    int currentFrame = animation->GetFrameIndexAtTime(timespan);
    SyncMediaPlayerWithAnimationFrame(currentFrame);
}

void
FOdysseyViewportDrawingEditorExtension::SyncMediaPlayerWithAnimationCurrentFrame()
{
    if (!mCurrentSource || mCurrentSource->Id() != FOdysseyAnimationEditorSource::StaticId())
        return;

    TSharedPtr<FOdysseyAnimationEditorSource> animationSource = StaticCastSharedPtr<FOdysseyAnimationEditorSource>(mCurrentSource);
    UOdysseyAnimation* animation = animationSource->GetAnimation();
    if (!animation)
        return;

    SyncMediaPlayerWithAnimationFrame(animation->CurrentFrame);
}

void
FOdysseyViewportDrawingEditorExtension::SyncAnimationCurrentFrameWithMediaPlayer()
{
    if (!mCurrentSource || mCurrentSource->Id() != FOdysseyAnimationEditorSource::StaticId())
        return;

    if (!mTexture || !mTexture->IsA(UMediaTexture::StaticClass()))
        return;

    TSharedPtr<FOdysseyAnimationEditorSource> animationSource = StaticCastSharedPtr<FOdysseyAnimationEditorSource>(mCurrentSource);
    UOdysseyAnimation* animation = animationSource->GetAnimation();
    if (!animation)
        return;

    UMediaTexture* texture = Cast<UMediaTexture>(mTexture);
    UMediaPlayer* mediaPlayer = texture->GetMediaPlayer();
    UOdysseyAnimationPlayer* animationPlayer = animationSource->GetAnimationPlayer();
    if (!mediaPlayer || mediaPlayer->IsPlaying() || animationPlayer->Status == EOdysseyAnimationPlayerStatus::Playing)
        return;

    UMediaPlaylist& playlist = mediaPlayer->GetPlaylistRef();
    int32 index = mediaPlayer->GetPlaylistIndex();
    UMediaSource* mediaSource = playlist.Get(index);
    if ( !mediaSource || !mediaSource->IsA(UOdysseyAnimation::StaticClass()) )
        return;

    TSharedRef<FMediaPlayerFacade> mediaPlayerFacade = mediaPlayer->GetPlayerFacade();
    TSharedPtr<FOdysseyAnimationMediaPlayer> animationMediaPlayer = StaticCastSharedPtr<FOdysseyAnimationMediaPlayer>(mediaPlayerFacade->GetPlayer());
    TSharedPtr<FOdysseyAnimationMediaControls> animationMediaControls = animationMediaPlayer->GetAnimationControls();

    //Use of controls->GetTime() instead of mediaPlayer->GetTime()
    //Because controls->GetTime() is synchronous with mediaPlayer->Seek()
    //And mediaPlayer->GetTime() is asynchronous with mediaPlayer->Seek()
    FTimespan timespan = animationMediaControls->GetTime() + FTimespan(1); //for precision purposes, otherwise "frame" can be the previous frame because of double imprecision
    double seconds = timespan.GetTotalSeconds();
    int frame = seconds * animation->GetFramesPerSecond() + animationMediaControls->GetFrameRange().GetLowerBoundValue();

    if (frame != animation->CurrentFrame)
        FOdysseyObjectEditorUtils::SetPropertyValue(animation, GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame), frame);
}

void
FOdysseyViewportDrawingEditorExtension::AddReferencedObjects(FReferenceCollector& iCollector)
{
    FOdysseyPainterEditorExtension::AddReferencedObjects(iCollector);

    FMeshPaintAdapterFactory::AddReferencedObjectsGlobals(iCollector);

    TMap<TObjectPtr<UMeshComponent>, TSharedPtr<IMeshPaintGeometryAdapter>> map = ComponentToAdapterMap();
    for (TMap< TObjectPtr<UMeshComponent>, TSharedPtr<IMeshPaintGeometryAdapter>>::TIterator It(map); It; ++It)
    {
        //Prevent GC on the components we save painting settings for
        iCollector.AddReferencedObject(It.Key());
        It.Value()->AddReferencedObjects(iCollector);
    }
}

bool
FOdysseyViewportDrawingEditorExtension::GetDrawHUDParams(const FSceneView* View, FCanvas* Canvas, FOdysseyHUDSystem::FDrawHUDParams& oParams)
{
    AActor* actor = Actor();
    UMeshComponent* component = Component();
    if (!component || !component->IsA<UStaticMeshComponent>())
        return false;

    UStaticMeshComponent* staticMeshComponent = Cast<UStaticMeshComponent>(component);
    UStaticMesh* staticMesh = staticMeshComponent->GetStaticMesh();

    if (!staticMesh)
        return false;

    UTexture* texture = Texture();
    if (!texture)
        return false;

    FBox meshBoundingBox = staticMesh->GetBoundingBox();
    FVector meshSize = meshBoundingBox.GetSize();

    float meshW = meshSize.X;
    float meshH = meshSize.Y;
    float textureW = texture->GetSurfaceWidth();
    float textureH = texture->GetSurfaceHeight();

    FVector meshPosition = meshBoundingBox.Min;
    FMatrix textureToComponent = FTransform(
        FQuat::Identity,
        FVector(meshBoundingBox.Min.X, meshBoundingBox.Min.Y, 0.f),
        FVector(meshSize.X / textureW, meshSize.Y / textureH, 1.f)
    ).ToMatrixWithScale();



    if (actor->IsA<AMediaPlate>())
    {
        FQuat rotY(FVector(0, 1, 0), FMath::DegreesToRadians(90.f));
        FQuat rotX(FVector(1, 0, 0), FMath::DegreesToRadians(90.f));
        //FQuat rot = rotX * rotZ;
        FQuat rot = rotX * rotY;
        textureToComponent = FTransform(
            //FQuat::Identity,
            rot.GetNormalized(),
            FVector(0.f, meshBoundingBox.Min.Y, -meshBoundingBox.Min.Z),
            FVector(meshSize.Y / textureW, -meshSize.Z / textureH, 1.f)
        ).ToMatrixWithScale();
    }
    FMatrix componentToWorld = component->GetComponentToWorld().ToMatrixWithScale();
    FMatrix textureToWorld = textureToComponent * componentToWorld;

    oParams.mCanvas = Canvas;
    oParams.mTextureToHUD = FOdysseyHUDSystem::FDrawHUDParams::FTextureToHUD::CreateLambda(
        [textureToWorld, View](const FVector2D& iPosition)
        {
            FVector worldPoint = textureToWorld.TransformPosition(FVector(iPosition.X, iPosition.Y, 0.f));
            FVector2D hudPoint;
            View->WorldToPixel(worldPoint, hudPoint);

            return hudPoint;
        }
    );
    oParams.mTextureWidth = textureW;
    oParams.mTextureHeight = textureH;

    return true;
}

bool
FOdysseyViewportDrawingEditorExtension::GetHUDPlaneParams(FVector& oPlaneTopLeft, double& oW, double& oH, FVector& oXAxis, FVector& oYAxis)
{
    UMeshComponent* component = Component();
    AActor* actor = Actor();

    if (!component || !component->IsA<UStaticMeshComponent>())
        return false;

    UStaticMeshComponent* staticMeshComponent = Cast<UStaticMeshComponent>(component);
    UStaticMesh* staticMesh = staticMeshComponent->GetStaticMesh();

    if (!staticMesh)
        return false;

    FVector actorLocation = actor->GetActorLocation();

    // Display settings
    FTransform componentToWorld = component->GetComponentToWorld();

    FVector brushXAxis(1.0f, 0.f, 0.f);
    FVector brushYAxis(0.f, 1.f, 0.f);

    FBox bbox = staticMesh->GetBoundingBox();
    FVector bboxSize = bbox.GetSize();

    oW = bboxSize.X;
    oH = bboxSize.Y;

    if (actor->IsA<AMediaPlate>())
    {
        brushXAxis = FVector(0.f, 1.f, 0.f);
        brushYAxis = FVector(0.f, 0.f, -1.f);
        oW = bboxSize.Y;
        oH = bboxSize.Z;
    }

    oXAxis = componentToWorld.TransformVector(brushXAxis);
    oYAxis = componentToWorld.TransformVector(brushYAxis);

    oPlaneTopLeft = actorLocation - oXAxis * oW / 2.f - oYAxis * oH / 2.f;

    return true;
}

bool
FOdysseyViewportDrawingEditorExtension::ViewportToHUD(FEditorViewportClient* iViewportClient, const FVector2D& iViewportPoint, FVector2D& oHUDPoint)
{
    if (!mTexture)
        return false;

    UMeshComponent* component = Component();
    if (!component || !component->IsA<UStaticMeshComponent>())
        return false;

    UStaticMeshComponent* staticMeshComponent = Cast<UStaticMeshComponent>(component);
    UStaticMesh* staticMesh = staticMeshComponent->GetStaticMesh();
    if (!staticMesh)
        return false;

    FVector planeTopLeft;
    double w;
    double h;
    FVector xAxis;
    FVector yAxis;
    if (!GetHUDPlaneParams(planeTopLeft, w, h, xAxis, yAxis))
        return false;

    FSceneViewFamilyContext viewFamily(
        FSceneViewFamily::ConstructionValues(
            iViewportClient->Viewport,
            iViewportClient->GetScene(),
            iViewportClient->EngineShowFlags
        )
        .SetRealtimeUpdate(iViewportClient->IsRealtime())
    );
    FSceneView* view = iViewportClient->CalcSceneView(&viewFamily);

    const FVector planeTopRight = planeTopLeft + xAxis * w;
    const FVector planeBottomLeft = planeTopLeft + yAxis * h;

    FPlane plane(planeTopLeft, planeTopRight, planeBottomLeft);

    FVector rayOrigin;
    FVector rayDirection;

    view->DeprojectFVector2D(iViewportPoint, rayOrigin, rayDirection);

    FVector worldPoint = FMath::RayPlaneIntersection(rayOrigin, rayDirection, plane);

    FTransform componentToWorld = component->GetComponentToWorld();
    FVector componentPoint = componentToWorld.InverseTransformPosition(worldPoint);

    FBox bbox = staticMesh->GetBoundingBox();
    FVector bboxMin = bbox.Min;

    AActor* actor = Actor();

    if (actor->IsA<AMediaPlate>())
    {
        componentPoint.Y -= bboxMin.Y;
        componentPoint.Z += bboxMin.Z;
        componentPoint.Z *= -1;
        oHUDPoint = FVector2D(componentPoint.Y * mTexture->GetSurfaceWidth() / w, componentPoint.Z * mTexture->GetSurfaceHeight() / h);
    }
    else
    {
        componentPoint.X -= bboxMin.X;
        componentPoint.Y -= bboxMin.Y;
        oHUDPoint = FVector2D(componentPoint.X * mTexture->GetSurfaceWidth() / w, componentPoint.Y * mTexture->GetSurfaceHeight() / h);
    }

    return true;
}

bool
FOdysseyViewportDrawingEditorExtension::IsPlaneComponent() const
{
    UMeshComponent* component = Component();
    if (!component || !component->IsA<UStaticMeshComponent>())
        return false;

    UStaticMeshComponent* staticMeshComponent = Cast<UStaticMeshComponent>(component);
    UStaticMesh* staticMesh = staticMeshComponent->GetStaticMesh();
    if (!staticMesh)
        return false;

    FStaticMeshLODResources& LODModel = staticMesh->GetRenderData()->LODResources[0];

    // Retrieve mesh vertex and index data
    const int32 NumVertices = LODModel.VertexBuffers.PositionVertexBuffer.GetNumVertices();

    TArray<FVector> MeshVertices;
    TArray<uint32> MeshIndices;

    MeshVertices.Reset();
    MeshVertices.AddDefaulted(NumVertices);
    for (int32 Index = 0; Index < NumVertices; Index++)
    {
        const FVector& Position = (FVector)LODModel.VertexBuffers.PositionVertexBuffer.VertexPosition(Index);
        MeshVertices[Index] = Position;
    }

    const int32 NumIndices = LODModel.IndexBuffer.GetNumIndices();
    MeshIndices.Reset();
    MeshIndices.AddDefaulted(NumIndices);
    const FIndexArrayView ArrayView = LODModel.IndexBuffer.GetArrayView();
    for (int32 Index = 0; Index < NumIndices; Index++)
    {
        MeshIndices[Index] = ArrayView[Index];
    }

    if (MeshIndices.Num() < 3)
        return false;

    uint32 i1 = MeshIndices[0];
    uint32 i2 = MeshIndices[1];
    uint32 i3 = MeshIndices[2];

    FVector p1 = MeshVertices[i1];
    FVector p2 = MeshVertices[i2];
    FVector p3 = MeshVertices[i3];

    FPlane plane(p1, p2, p3);
    FVector baseNormal = plane.GetNormal();

    for (int i = 3; i < MeshIndices.Num(); i+=3)
    {
        i1 = MeshIndices[i];
        i2 = MeshIndices[i+1];
        i3 = MeshIndices[i+2];

        p1 = MeshVertices[i1];
        p2 = MeshVertices[i2];
        p3 = MeshVertices[i3];

        plane = FPlane(p1, p2, p3);
        FVector normal = plane.GetNormal();

        FVector diff = baseNormal - normal;
        if (!diff.IsNearlyZero())
            return false;
    }

    return true;
}

#undef LOCTEXT_NAMESPACE

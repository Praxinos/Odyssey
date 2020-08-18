// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "SOdysseyFlipbookTimelineView.h"

#include "AssetRegistryModule.h"
#include "Engine/Texture2D.h"
#include "GenericPlatform/GenericPlatformMath.h"

#include "SOdysseyPlaybackControls.h"
#include "SOdysseyTimeline.h"
#include "SOdysseyFlipbookTimelineTrack.h"
#include "OdysseyBlock.h"
#include "OdysseySurfaceEditable.h"
#include "OdysseyTexture.h"

#include "SOdysseyTextureConfigureWindow.h"


#define LOCTEXT_NAMESPACE "OdysseyFlipbook"

//CONSTRUCTION/DESTRUCTION-----------------------------------------------


SOdysseyFlipbookTimelineView::~SOdysseyFlipbookTimelineView()
{
}


void SOdysseyFlipbookTimelineView::Construct(const FArguments& InArgs)
{
    mFlipbook = InArgs._Flipbook;

    mIsPlaying = false;
    mIsLooping = false;
    mScrubPositionBeforePlay = 0.0f;

	mOnCurrentKeyframeChanged = InArgs._OnCurrentKeyframeChanged;
	mOnPlayStarted = InArgs._OnPlayStarted;
	mOnPlayStopped = InArgs._OnPlayStopped;
	mOnScrubStarted = InArgs._OnScrubStarted;
	mOnScrubStopped = InArgs._OnScrubStopped;
	mOnScrubPositionChanged = InArgs._OnScrubPositionChanged;
	//mOnStructureChanged = InArgs._OnStructureChanged;
    mOnFlipbookChanged = InArgs._OnFlipbookChanged;
	mOnSpriteCreated = InArgs._OnSpriteCreated;
	mOnTextureCreated = InArgs._OnTextureCreated;

    ChildSlot
    [
        SNew(SVerticalBox)

        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SOverlay)
            + SOverlay::Slot()
            .HAlign(HAlign_Center)
            [
                SNew(SOdysseyPlaybackControls)
                .IsPlaying(this, &SOdysseyFlipbookTimelineView::IsPlaying)
                .IsLooping(this, &SOdysseyFlipbookTimelineView::IsLooping)
                .OnPlayClicked(this, &SOdysseyFlipbookTimelineView::OnPlayClicked)
                .OnPauseClicked(this, &SOdysseyFlipbookTimelineView::OnPauseClicked)
                .OnBeginningClicked(this, &SOdysseyFlipbookTimelineView::OnBeginningClicked)
                .OnEndClicked(this, &SOdysseyFlipbookTimelineView::OnEndClicked)
                .OnPreviousClicked(this, &SOdysseyFlipbookTimelineView::OnPreviousClicked)
                .OnNextClicked(this, &SOdysseyFlipbookTimelineView::OnNextClicked)
                .OnLoopClicked(this, &SOdysseyFlipbookTimelineView::OnLoopClicked)
                //.OnFrameRateChanged(this, &SOdysseyFlipbookTimelineView::OnFrameRateChanged)
            ]
			+ SOverlay::Slot()
			.HAlign(HAlign_Left)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					//.HAlign(HAlign_Center)
					.Text(LOCTEXT("AddKeyframe", "Add Keyframe"))
					.OnClicked(this, &SOdysseyFlipbookTimelineView::OnAddFrameClicked)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.Visibility(this, &SOdysseyFlipbookTimelineView::FixCurrentFrameVisibility)
					//.HAlign(HAlign_Center)
					.Text(LOCTEXT("Fix Current Keyframe", "Fix Current Keyframe"))
					.OnClicked(this, &SOdysseyFlipbookTimelineView::OnFixCurrentFrameClicked)
				]
			]
			+ SOverlay::Slot()
			.HAlign(HAlign_Right)
			[
				SNew(SNumericDropDown<float>)
				.bShowNamedValue(false)
				.DropDownValues(FrameRateDropDownValues())
                .LabelText(LOCTEXT("FrameRate", "Frame rate"))
                .MinDesiredValueWidth(50)
                .OnValueChanged(this, &SOdysseyFlipbookTimelineView::OnFrameRateChanged)
                .Orientation(EOrientation::Orient_Horizontal)
                .Value(this, &SOdysseyFlipbookTimelineView::GetFrameRate)
			]
        ]

		+ SVerticalBox::Slot()
		.Padding(0, 0, 0, 0)
		.AutoHeight()
		[
			SAssignNew(mTimelineWidget, SOdysseyTimeline)
            .OnScrubStarted(this, &SOdysseyFlipbookTimelineView::OnScrubStarted)
            .OnScrubPositionChanged(this, &SOdysseyFlipbookTimelineView::OnScrubPositionChanged)
            .OnScrubStopped(InArgs._OnScrubStopped)
            [
                SAssignNew(mTimelineTrackWidget, SOdysseyFlipbookTimelineTrack)
				.Flipbook(mFlipbook)
				.FrameSize(this, &SOdysseyFlipbookTimelineView::FrameSize)
                .OnFlipbookChanged(mOnFlipbookChanged)
                .OnKeyframeRemoved(InArgs._OnKeyframeRemoved)
            ]
		]
    ];
}

//In Frames
float
SOdysseyFlipbookTimelineView::GetScrubPosition() const
{
    return mTimelineWidget->ScrubPosition();
}

//The keyframe index at Scrub position
int
SOdysseyFlipbookTimelineView::GetCurrentKeyframeIndex() const
{
    return GetKeyframeIndexAtScrubPosition(GetScrubPosition());
}

int
SOdysseyFlipbookTimelineView::GetKeyframeIndexAtScrubPosition(float iPosition) const
{
    if (iPosition < 0 || iPosition >= mFlipbook->GetNumFrames())
    {
        return -1;
    }

    int32 position = 0;

    for (int32 i = 0; i < mFlipbook->GetNumKeyFrames(); i++)
    {
        position += mFlipbook->GetKeyFrameChecked(i).FrameRun;
        
        if (position > iPosition)
            return i;
    }

    return -1;
}

float
SOdysseyFlipbookTimelineView::GetScrubPositionForKeyFrameIndex(int32 iIndex) const
{
    if (iIndex < 0 || iIndex >= mFlipbook->GetNumKeyFrames())
        return -1;

    int32 position = 0; //int32 to avoid float imprecision in the for loop

    for (int32 i = 0; i < iIndex; i++)
    {
        position += mFlipbook->GetKeyFrameChecked(i).FrameRun;
    }

    return position;
}

void
SOdysseyFlipbookTimelineView::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
    
    if (!mIsPlaying)
        return;
    
    float scrubPosition = mTimelineWidget->ScrubPosition() + InDeltaTime * mFlipbook->GetFramesPerSecond();
    if (mIsLooping)
    {
        scrubPosition = FGenericPlatformMath::Fmod(scrubPosition, mFlipbook->GetNumFrames());
    }
    else if (scrubPosition >= mFlipbook->GetNumFrames())
    {
        //If we don't loop and we play after the flipbook duration, then we stop playing
		mTimelineWidget->ScrubPosition(mScrubPositionBeforePlay);
        Stop();
        return;
    }

    mTimelineWidget->ScrubPosition(scrubPosition);
}

void
SOdysseyFlipbookTimelineView::OnScrubStarted()
{
    //If we're already playing, then it means a manual scrub started
    //So we stop playing and just continue scrubbing manually
    if (mIsPlaying)
    {
	    mIsPlaying = false;
        mOnPlayStopped.ExecuteIfBound();
        return;
    }
	
    //If we're not playing, just start scrubbing
    mOnScrubStarted.ExecuteIfBound();
}

void
SOdysseyFlipbookTimelineView::OnScrubPositionChanged(float iOldPosition)
{
    mOnScrubPositionChanged.ExecuteIfBound(iOldPosition);

    int32 previousFrame = GetKeyframeIndexAtScrubPosition(iOldPosition);
    int32 nextFrame = GetCurrentKeyframeIndex();

    //if both frame are out of range we do nothing
    if (previousFrame == -1 && nextFrame == -1)
        return;

    //if at least one frame is in range and both frame are different, we admit we changed current frame
    if (nextFrame != previousFrame)
    {
        mOnCurrentKeyframeChanged.ExecuteIfBound(nextFrame);
    }
}

void
SOdysseyFlipbookTimelineView::Play()
{
    if (mIsPlaying)
        return;

	mIsPlaying = true;

    //start scrubbing
    mOnScrubStarted.ExecuteIfBound();

    //Activate the play, which is basically a automated scrub
	mOnPlayStarted.ExecuteIfBound();
}

void
SOdysseyFlipbookTimelineView::Stop()
{
    if (!mIsPlaying)
        return;

    mIsPlaying = false;

    //We stop the play to stop any automated scrubbing
	mOnPlayStopped.ExecuteIfBound();

    //We stop the scrubbing
    mOnScrubStopped.ExecuteIfBound();
}

bool
SOdysseyFlipbookTimelineView::IsLooping() const
{
	return mIsLooping;
}

bool
SOdysseyFlipbookTimelineView::IsPlaying() const
{
	return mIsPlaying;
}

bool
SOdysseyFlipbookTimelineView::IsScrubbing() const
{
	return mIsPlaying || mTimelineWidget->IsScrubbing();
}

float
SOdysseyFlipbookTimelineView::FrameSize() const
{
    return mTimelineWidget->FrameSize();
}

void
SOdysseyFlipbookTimelineView::OnFrameRateChanged(float iFrameRate)
{
	//Using a mutator is mandatory to change the framerate of the flipbook
	FScopedFlipbookMutator mutator(mFlipbook);
	mutator.FramesPerSecond = iFrameRate; //This changes directly the flipbook framerate
}

FReply
SOdysseyFlipbookTimelineView::OnPlayClicked()
{
    mScrubPositionBeforePlay = mTimelineWidget->ScrubPosition();
	Play();
	return FReply::Handled();
}

FReply
SOdysseyFlipbookTimelineView::OnPauseClicked()
{
	Stop();
	return FReply::Handled();
}

FReply
SOdysseyFlipbookTimelineView::OnBeginningClicked()
{
	mTimelineWidget->ScrubPosition(0.5f);
	return FReply::Handled();
}

FReply
SOdysseyFlipbookTimelineView::OnEndClicked()
{
	mTimelineWidget->ScrubPosition(mFlipbook->GetNumFrames() - 0.5f);
	return FReply::Handled();
}

FReply
SOdysseyFlipbookTimelineView::OnPreviousClicked()
{
    int32 index = GetKeyframeIndexAtScrubPosition(mTimelineWidget->ScrubPosition());
    if (index <= 0 || index >= mFlipbook->GetNumKeyFrames())
        return FReply::Handled();

    float position = GetScrubPositionForKeyFrameIndex(index - 1);
    mTimelineWidget->ScrubPosition(position + 0.5f);

	return FReply::Handled();
}

FReply
SOdysseyFlipbookTimelineView::OnNextClicked()
{
	int32 index = GetKeyframeIndexAtScrubPosition(mTimelineWidget->ScrubPosition());
    if (index < 0 || index >= mFlipbook->GetNumKeyFrames() - 1)
        return FReply::Handled();

    float position = GetScrubPositionForKeyFrameIndex(index + 1);
    mTimelineWidget->ScrubPosition(position + 0.5f);

	return FReply::Handled();
}

FReply
SOdysseyFlipbookTimelineView::OnLoopClicked()
{
	mIsLooping = !mIsLooping;
	return FReply::Handled();
}

FReply
SOdysseyFlipbookTimelineView::OnAddFrameClicked()
{
	if (AddFrame())
    {
        mTimelineWidget->ScrubPosition(mFlipbook->GetNumFrames() - 0.5f);
    }
	return FReply::Handled();
}

FReply
SOdysseyFlipbookTimelineView::OnFixCurrentFrameClicked()
{
    FixFrame(GetCurrentKeyframeIndex());
	return FReply::Handled();
}

bool
SOdysseyFlipbookTimelineView::FixFrame(int32 iIndex)
{
    if (iIndex < 0 || iIndex >= mFlipbook->GetNumKeyFrames())
        return false;

	const FPaperFlipbookKeyFrame& keyframe = mFlipbook->GetKeyFrameChecked(iIndex);

	UPaperSprite* sprite = keyframe.Sprite;
	UTexture2D* texture = sprite ? sprite->GetSourceTexture() : nullptr;
	if (texture)
		return false;

    // Displays a modal window asking for Width and Height of the new texture to draw in
	// If validated, it creates a new sprite and a new texture using the same name and path as the flipbook but adding some suffixes
    TSharedPtr<SOdysseyTextureConfigureWindow> textureConfigurationWindow = SNew( SOdysseyTextureConfigureWindow );
    GEditor->EditorAddModalWindow( textureConfigurationWindow.ToSharedRef() );

	//If cancel is clicked, we do nothing
    if(!textureConfigurationWindow->GetWindowAnswer())
        return false;

    int32 width = textureConfigurationWindow->GetWidth();
    int32 height = textureConfigurationWindow->GetHeight();

	texture = CreateTexture(width, height);
	if (!texture)
		return false;

    bool spriteExists = !!sprite;
	if (spriteExists)
	{
		//Set the texture in the existing sprite
		UClass* spriteClass = sprite->StaticClass();

		
		FSoftObjectProperty* sourceTextureProperty = FindFProperty<FSoftObjectProperty>(spriteClass, "SourceTexture");
		sourceTextureProperty->SetObjectPropertyValue(sourceTextureProperty->ContainerPtrToValuePtr<UPaperSprite>(sprite), texture);
	}
    else
    {
		sprite = CreateSprite(texture);
		if (!sprite)
			return false;
            
    }
    
	{
		FScopedFlipbookMutator mutator(mFlipbook);
		mutator.KeyFrames[iIndex].Sprite = sprite;
	}
    
    if (!spriteExists)
    {
        mOnSpriteCreated.ExecuteIfBound(sprite);
    }
    
    mOnTextureCreated.ExecuteIfBound(texture);
	mTimelineTrackWidget->SetFrame(iIndex, texture);
	mOnFlipbookChanged.ExecuteIfBound();

	return true;
}

bool
SOdysseyFlipbookTimelineView::AddFrame()
{
	// Displays a modal window asking for Width and Height of the new texture to draw in
	// If validated, it creates a new sprite and a new texture using the same name and path as the flipbook but adding some suffixes
    TSharedPtr<SOdysseyTextureConfigureWindow> textureConfigurationWindow = SNew( SOdysseyTextureConfigureWindow );
    GEditor->EditorAddModalWindow( textureConfigurationWindow.ToSharedRef() );

    //If cancel is clicked, we do nothing
    if(!textureConfigurationWindow->GetWindowAnswer())
        return false;

    int32 width = textureConfigurationWindow->GetWidth();
    int32 height = textureConfigurationWindow->GetHeight();

    UTexture2D* texture = CreateTexture(width, height);
    if (!texture)
        return false;

    UPaperSprite* sprite = CreateSprite(texture);
    if (!sprite)
        return false;

	FPaperFlipbookKeyFrame keyframe;
	keyframe.Sprite = sprite;
	keyframe.FrameRun = 1;
	
	{
		FScopedFlipbookMutator mutator(mFlipbook);
		mutator.KeyFrames.Add(keyframe);
	}

	mTimelineTrackWidget->AddFrame(texture, 1);

    mOnSpriteCreated.ExecuteIfBound(sprite);
    mOnTextureCreated.ExecuteIfBound(texture);
	mOnFlipbookChanged.ExecuteIfBound();
    return true;
}


UTexture2D*
SOdysseyFlipbookTimelineView::CreateTexture(int32 width, int32 height)
{
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    FString PackageName = FPaths::GetPath( mFlipbook->GetPathName() ) + "/";
    FString AssetName = mFlipbook->GetName() + "_Texture";
    AssetTools.CreateUniqueAssetName(PackageName,AssetName,PackageName,AssetName);

    UPackage* package = CreatePackage( nullptr, *PackageName );

    FOdysseyBlock block( width, height, ETextureSourceFormat::TSF_BGRA8, nullptr, nullptr, true );
    
    UTexture2D* texture = NewObject<UTexture2D>(package, FName(AssetName), RF_Public | RF_Standalone | RF_Transactional );
    texture->Source.Init( width, height, 1, 1, TSF_BGRA8 );
    texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
    texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    texture->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;
    UOdysseyTextureAssetUserData* userData = NewObject< UOdysseyTextureAssetUserData >(texture, NAME_None, RF_Public);
    userData->GetLayerStack()->Init( texture->Source.GetSizeX(), texture->Source.GetSizeY() );
    texture->AddAssetUserData( userData );
    texture->PostEditChange();
    
    CopyBlockDataIntoUTexture( &block, texture );

	texture->UpdateResource();
	FAssetRegistryModule::AssetCreated(texture);
	UPackage::SavePackage(package, texture, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *AssetName);

	package->MarkAsFullyLoaded();
	texture->MarkPackageDirty();
    return texture;
}

UPaperSprite*
SOdysseyFlipbookTimelineView::CreateSprite(UTexture2D* iTexture)
{
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    FString PackageName = FPaths::GetPath( mFlipbook->GetPathName() ) + "/";
    FString AssetName = mFlipbook->GetName() + "_Sprite";
    AssetTools.CreateUniqueAssetName(PackageName,AssetName,PackageName,AssetName);

    UPackage* package = CreatePackage( nullptr, *PackageName );
    
    UPaperSprite* sprite = NewObject<UPaperSprite>(package, FName(AssetName), RF_Public | RF_Standalone | RF_Transactional );

    //Set the correct Render Geometry Type
    UClass* spriteClass = sprite->StaticClass();

	FStructProperty* renderGeometryProperty = FindFProperty<FStructProperty>(spriteClass,"RenderGeometry");
    if (!renderGeometryProperty)
        return NULL;

	FByteProperty* geometryType = FindFProperty<FByteProperty>(renderGeometryProperty->Struct, "GeometryType");
    if (!geometryType)
        return NULL;

	geometryType->SetPropertyValue_InContainer(renderGeometryProperty->ContainerPtrToValuePtr<FSpriteGeometryCollection>(sprite), ESpritePolygonMode::SourceBoundingBox);

    //Init sprite
	FSpriteAssetInitParameters spriteInitParams;
	spriteInitParams.SetTextureAndFill(iTexture);

	const UPaperImporterSettings* importerSettings = GetDefault<UPaperImporterSettings>();
	importerSettings->ApplySettingsForSpriteInit(spriteInitParams, ESpriteInitMaterialLightingMode::Automatic);
	sprite->InitializeSprite(spriteInitParams);

    //Finalize asset creation
	FAssetRegistryModule::AssetCreated(sprite);
	UPackage::SavePackage(package, sprite, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *AssetName);

	package->MarkAsFullyLoaded();
	sprite->MarkPackageDirty();

    return sprite;
}

EVisibility
SOdysseyFlipbookTimelineView::FixCurrentFrameVisibility() const
{
    int32 index = GetCurrentKeyframeIndex();
    if (index < 0 || index >= mFlipbook->GetNumKeyFrames())
        return EVisibility::Collapsed;

	const FPaperFlipbookKeyFrame& keyframe = mFlipbook->GetKeyFrameChecked(index);

	UPaperSprite* sprite = keyframe.Sprite;
	UTexture2D* texture = sprite ? sprite->GetSourceTexture() : nullptr;
	if (texture)
		return EVisibility::Collapsed;
	return EVisibility::Visible;
}

TArray<SNumericDropDown<float>::FNamedValue>
SOdysseyFlipbookTimelineView::FrameRateDropDownValues() const
{
    TArray<SNumericDropDown<float>::FNamedValue> values;
    values.Add(SNumericDropDown<float>::FNamedValue(1.0f, LOCTEXT("1Fps", "1 FPS"), LOCTEXT("1FpsDescription", "1 frame per second")));
    values.Add(SNumericDropDown<float>::FNamedValue(2.0f, LOCTEXT("2Fps", "2 FPS"), LOCTEXT("2FpsDescription", "2 frames per second")));
    values.Add(SNumericDropDown<float>::FNamedValue(3.0f, LOCTEXT("3Fps", "3 FPS"), LOCTEXT("3FpsDescription", "3 frames per second")));
    values.Add(SNumericDropDown<float>::FNamedValue(6.0f, LOCTEXT("6Fps", "6 FPS"), LOCTEXT("6FpsDescription", "6 frames per second")));
    values.Add(SNumericDropDown<float>::FNamedValue(12.0f, LOCTEXT("12Fps", "12 FPS"), LOCTEXT("12FpsDescription", "12 frames per second")));
    values.Add(SNumericDropDown<float>::FNamedValue(24.0f, LOCTEXT("24Fps", "24 FPS"), LOCTEXT("24FpsDescription", "24 frames per second")));
    values.Add(SNumericDropDown<float>::FNamedValue(29.97f, LOCTEXT("29.97Fps", "29.97 FPS"), LOCTEXT("29.97FpsDescription", "29.97 frames per second")));
    values.Add(SNumericDropDown<float>::FNamedValue(30.0f, LOCTEXT("30Fps", "30 FPS"), LOCTEXT("30FpsDescription", "30 frames per second")));
    values.Add(SNumericDropDown<float>::FNamedValue(60.0f, LOCTEXT("60Fps", "60 FPS"), LOCTEXT("60FpsDescription", "60 frames per second")));

    return values;
}

float
SOdysseyFlipbookTimelineView::GetFrameRate() const
{
    return mFlipbook->GetFramesPerSecond();
}

#undef LOCTEXT_NAMESPACE

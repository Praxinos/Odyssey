// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "FlipbookEditor/OdysseyFlipbookEditorExtension.h"

#include "PainterEditor/OdysseyPainterEditor.h"
#include "FlipbookEditor/OdysseyFlipbookEditorGUI.h"
#include "FlipbookEditor/OdysseyFlipbookEditorTimelineTab.h"
#include "Widgets/SOdysseyFlipbookTimelineView.h"
#include "TextureEditor/OdysseyTextureEditorSource.h"
#include "PaperSprite.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

FOdysseyFlipbookEditorExtension::~FOdysseyFlipbookEditorExtension()
{
}

FOdysseyFlipbookEditorExtension::FOdysseyFlipbookEditorExtension(FOdysseyPainterEditor* iEditor)
    : FOdysseyPainterEditorExtension(iEditor)
    , mFlipbookWrapper(MakeShared<FOdysseyFlipbookWrapper>())
    , mPreviewSurface(nullptr)
    , mGUI(nullptr)
{
}

void
FOdysseyFlipbookEditorExtension::Initialize()
{
    mGUI = MakeShared<FOdysseyFlipbookEditorGUI>(this);
    mGUI->Initialize();
}

void
FOdysseyFlipbookEditorExtension::Finalize()
{
    mGUI->Finalize();
    SetFlipbook(nullptr);
}

void
FOdysseyFlipbookEditorExtension::BuildLayout(FOdysseyEditorLayoutBuilder& iBuilder)
{
    mGUI->BuildLayout(iBuilder);
}

void
FOdysseyFlipbookEditorExtension::SetFlipbook(UPaperFlipbook* iFlipbook)
{
    if (iFlipbook == GetFlipbook())
        return;

    if (!iFlipbook)
    {
        UPaperFlipbook* flipbook = GetFlipbook();
        if (flipbook)
        {
            //Remove all additional Edited Objects (Sprites and Textures)
            for (int32 index = 0; index < flipbook->GetNumKeyFrames(); ++index)
            {
                UPaperSprite* sprite = mFlipbookWrapper->GetKeyframeSprite(index);
                if (!sprite)
                    continue;

                GetEditor()->RemoveEditedObject(sprite);

                UTexture2D* texture = mFlipbookWrapper->GetKeyframeTexture(index);
                if (!texture)
                    continue;
                    
                GetEditor()->RemoveEditedObject(texture);
            }
        }

        GetEditor()->SetSource(nullptr);
        mFlipbookWrapper->SetFlipbook(nullptr);
        mFlipbookWrapper->OnSpriteTextureChanged().RemoveAll(this);
        mGUI->OnFlipbookChanged();
        return;
    }
    
    mFlipbookWrapper->SetFlipbook(iFlipbook);

    //Find all additional Edited Objects (Sprites and Textures)
    for (int32 index = 0; index < iFlipbook->GetNumKeyFrames(); ++index)
    {
        UPaperSprite* sprite = mFlipbookWrapper->GetKeyframeSprite(index);
        if (!sprite)
            continue;

        GetEditor()->AddEditedObject(sprite);

        UTexture2D* texture = mFlipbookWrapper->GetKeyframeTexture(index);
        if (!texture)
            continue;
            
        GetEditor()->AddEditedObject(texture);
    }

    // Set Sprite Changed Callback
    mFlipbookWrapper->OnSpriteTextureChanged().AddRaw(this, &FOdysseyFlipbookEditorExtension::OnSpriteTextureChanged);

    //TODO: Activate GUI

    //Set Texture Source if needed
    if (iFlipbook->GetNumKeyFrames() > 0)
    {
        UTexture2D* texture = mFlipbookWrapper->GetKeyframeTexture(0);
        TSharedPtr<FOdysseyTextureEditorSource> source = MakeShared<FOdysseyTextureEditorSource>(texture);
        GetEditor()->SetSource(source);
    }

    mGUI->OnFlipbookChanged();
}

UPaperFlipbook*
FOdysseyFlipbookEditorExtension::GetFlipbook()
{
    return mFlipbookWrapper->GetFlipbook();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

TSharedPtr<FOdysseyFlipbookWrapper>
FOdysseyFlipbookEditorExtension::FlipbookWrapper()
{
    return mFlipbookWrapper;
}

UTexture*
FOdysseyFlipbookEditorExtension::PreviewTexture()
{
    return mPreviewSurface.Texture();
}

void
FOdysseyFlipbookEditorExtension::PreviewTexture(UTexture2D* iTexture)
{
    return mPreviewSurface.Texture(iTexture);
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Overrides

void
FOdysseyFlipbookEditorExtension::SetTextureAtKeyframeIndex(int32 iKeyframeIndex)
{
    TSharedPtr<FOdysseyFlipbookEditorTimelineTab> timelineTab = GetEditor()->FindTab<FOdysseyFlipbookEditorTimelineTab>();
    
    UTexture2D* texture = mFlipbookWrapper->GetKeyframeTexture(iKeyframeIndex);
    if (timelineTab->Timeline()->IsScrubbing())
    {
        mPreviewSurface.Texture(texture);
        return;
    }

    TSharedPtr<FOdysseyTextureEditorSource> source = MakeShared<FOdysseyTextureEditorSource>(texture);
    GetEditor()->SetSource(source);
}

void
FOdysseyFlipbookEditorExtension::OnSpriteTextureChanged(UPaperSprite* iSprite, UTexture2D* iOldTexture)
{
    UTexture2D* texture = iSprite->GetSourceTexture();
    UPaperFlipbook* flipbook = mFlipbookWrapper->GetFlipbook();
    for (int i = 0; i < flipbook->GetNumKeyFrames(); i++)
    {
        UPaperSprite* sprite = mFlipbookWrapper->GetKeyframeSprite(i);
        if (sprite == iSprite)
        {
            if (iOldTexture)
                GetEditor()->RemoveEditedObject(iOldTexture);

            if (texture)
                GetEditor()->AddEditedObject(texture);
        }
    }

    TSharedPtr<FOdysseyFlipbookEditorTimelineTab> timelineTab = GetEditor()->FindTab<FOdysseyFlipbookEditorTimelineTab>();
    int32 index = timelineTab->Timeline()->GetCurrentKeyframeIndex();

    UPaperSprite* sprite = mFlipbookWrapper->GetKeyframeSprite(index);
    if (sprite != iSprite)
        return;

    SetTextureAtKeyframeIndex(index);
}

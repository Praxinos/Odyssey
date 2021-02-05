// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyFlipbookEditorToolkit.h"

#include "IOdysseyFlipbookEditorModule.h"

#define LOCTEXT_NAMESPACE "OdysseyFlipbookEditorToolkit"

/////////////////////////////////////////////////////
// FOdysseyFlipbookEditorToolkit
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyFlipbookEditorToolkit::~FOdysseyFlipbookEditorToolkit()
{
}

FOdysseyFlipbookEditorToolkit::FOdysseyFlipbookEditorToolkit() :
	FOdysseyPainterEditorToolkit( TEXT("OdysseyFlipbookEditorApp") )
{
}

void
FOdysseyFlipbookEditorToolkit::OpenAsset(UObject* iObject)
{
	UPaperFlipbook* flipbook = Cast<UPaperFlipbook>(iObject);
	IOdysseyFlipbookEditorModule* odysseyFlipbookEditorModule = &FModuleManager::GetModuleChecked<IOdysseyFlipbookEditorModule>("OdysseyFlipbookEditor");
	odysseyFlipbookEditorModule->CreateOdysseyFlipbookEditor(flipbook);
}

FText
FOdysseyFlipbookEditorToolkit::GetBaseToolkitName() const
{
    return LOCTEXT( "AppLabel", "Odyssey Flipbook Editor" );
}

FName
FOdysseyFlipbookEditorToolkit::GetToolkitFName() const
{
    return FName( "OdysseyFlipbookEditor" );
}

FString
FOdysseyFlipbookEditorToolkit::GetWorldCentricTabPrefix() const
{
    return LOCTEXT( "WorldCentricTabPrefix", "Flipbook" ).ToString();
}

TArray<UObject*>
FOdysseyFlipbookEditorToolkit::GetAllEditedObjects()
{
	TArray<UObject*> objects;
	objects.Add(mEditedObject);


	UPaperFlipbook* flipbook = Cast<UPaperFlipbook>(mEditedObject);
	TSharedPtr<FOdysseyFlipbookWrapper> flipbookWrapper = MakeShareable(new FOdysseyFlipbookWrapper(flipbook));

	for (int32 index = 0; index < flipbook->GetNumKeyFrames(); ++index)
	{
		UPaperSprite* sprite = flipbookWrapper->GetKeyframeSprite(index);
        if (!sprite)
            continue;

		objects.Add(sprite);

		UTexture2D* texture = flipbookWrapper->GetKeyframeTexture(index);
        if (!texture)
            continue;
            
        objects.Add(texture);
	}

	return objects;
}

#undef LOCTEXT_NAMESPACE
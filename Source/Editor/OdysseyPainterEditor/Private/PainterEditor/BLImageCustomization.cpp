#include "BLImageCustomization.h"
#include "OdysseyVectorBrush.h"
#include "ContentBrowserModule.h" // for FContentBrowserModule
#include "PropertyEditor/Public/IPropertyTypeCustomization.h"
#include "DetailWidgetRow.h"
#include "IContentBrowserSingleton.h" // for FAssetPickerConfig
#include "PropertyCustomizationHelpers.h" // for SObjectPropertyEntryBox

#include "SlateBasics.h"

#define LOCTEXT_NAMESPACE "BLImageCustomization"

class FBLImageDetails : public IPropertyTypeCustomization
{
public:
    /** Makes a new instance of this detail layout class for a specific detail view requesting it */
    static TSharedRef<IPropertyTypeCustomization> MakeInstance()
    {
        return MakeShareable(new FBLImageDetails());
    }

    virtual void CustomizeHeader( TSharedRef<IPropertyHandle> StructPropertyHandle
	                            , class FDetailWidgetRow& HeaderRow
	                            , IPropertyTypeCustomizationUtils& StructCustomizationUtils ) override;

    virtual void CustomizeChildren( TSharedRef<IPropertyHandle> StructPropertyHandle
                                  , class IDetailChildrenBuilder& StructBuilder
		                          , IPropertyTypeCustomizationUtils& StructCustomizationUtils ) override;

    private:
        void OnObjectChanged( const FAssetData& AssetData
                            , TSharedRef<IPropertyHandle> StructPropertyHandle );
        void ObjectPath( TSharedRef<IPropertyHandle> StructPropertyHandle );
        FReply OnClicked( TSharedRef<IPropertyHandle> StructPropertyHandle );
        bool FilterAsset( const struct FAssetData& InAssetData );

    private:
        TSharedPtr<SButton> mBrushButton;
        FSlateBrush* mBrushIcon;
        TSharedPtr<FAssetThumbnailPool> mAssetThumbnailPool;
};

void
FBLImageDetails::CustomizeHeader( TSharedRef<IPropertyHandle> StructPropertyHandle
	                            , class FDetailWidgetRow& HeaderRow
	                            , IPropertyTypeCustomizationUtils& StructCustomizationUtils )
{
    TSharedPtr<IPropertyHandle> textureProperty = StructPropertyHandle->GetChildHandle("texture");

    mAssetThumbnailPool = MakeShareable( new FAssetThumbnailPool( 1024 ) );

    mBrushIcon = new FSlateBrush();
    mBrushIcon->SetResourceObject(nullptr);
    mBrushIcon->ImageSize.X = 64;
    mBrushIcon->ImageSize.Y = 16;
    //mBrushIcon->DrawAs = ImageType;

    mBrushButton = SNew(SButton)
			      .Visibility( EVisibility::Visible )
			      .Text( LOCTEXT("CreateDefault", "Create Default") )
			      .ToolTipText( LOCTEXT("CreateDefaultToolTip", "Reconstructs section ordering based on start time") )
			      .OnClicked(this, &FBLImageDetails::OnClicked, StructPropertyHandle )
			      .HAlign(HAlign_Center)
			      .VAlign(VAlign_Center)
                  [
                      SNew(SImage)
                      .Image( mBrushIcon )
                  ];

    ObjectPath( StructPropertyHandle );

	HeaderRow.NameContent()[StructPropertyHandle->CreatePropertyNameWidget()]
	.ValueContent()[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			mBrushButton.ToSharedRef()
/*
            SNew(SObjectPropertyEntryBox)
                .AllowedClass(          UTexture2D::StaticClass() )
                .ObjectPath(            this, &FBLImageDetails::ObjectPath, StructPropertyHandle )
                .ThumbnailPool(         mAssetThumbnailPool )
                .OnObjectChanged(       this, &FBLImageDetails::OnObjectChanged, StructPropertyHandle )
                .OnShouldFilterAsset(   this, &FBLImageDetails::FilterAsset )
                .AllowClear(            true )
                .DisplayUseSelected(    true )
                .DisplayBrowse(         true )
                .EnableContentPicker(   true )
                .DisplayCompactSize(    true )
                .DisplayThumbnail(      true )
                .AllowCreate(           true )
                .ThumbnailSizeOverride( FIntPoint( 64, 16 ) )
*/
		]
	];

    textureProperty->SetOnPropertyValueChanged( FSimpleDelegate::CreateSP( this, &FBLImageDetails::ObjectPath, StructPropertyHandle ) );
}

FReply
FBLImageDetails::OnClicked( TSharedRef<IPropertyHandle> StructPropertyHandle )
{
    FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
    FString brushPath = FPaths::ProjectContentDir() + FString("/Brushes/Vector");
    // Configure filter for asset picker
    FAssetPickerConfig Config;

    Config.InitialAssetViewType = EAssetViewType::List;
    Config.ThumbnailScale = 1.0f; // make thumbnails as small as possible
    Config.OnAssetSelected = FOnAssetSelected::CreateRaw( this, &FBLImageDetails::OnObjectChanged, StructPropertyHandle );
    // All the filter I tried didn't work, so I ended up filtering via the OnShouldFilterAsset delegate
    Config.OnShouldFilterAsset = FOnShouldFilterAsset::CreateRaw( this, &FBLImageDetails::FilterAsset );
    Config.bAllowNullSelection = true;
    Config.bAllowDragging = false;

    FSlateApplication::Get().AddModalWindow( SNew(SWindow)
                                              .Title( LOCTEXT( "Pick Vector Brush", "Pick Vector Brush" ) )
                                              .ClientSize( FVector2D( 256, 256 ) )
                                               [
                                                   SNew(SBox)
                                                   .WidthOverride(300.f)
                                                   .HeightOverride(300.f)
                                                   [
                                                       ContentBrowserModule.Get().CreateAssetPicker(Config)
                                                   ]
                                               ]
                                              , mBrushButton
                                              , false
                                             );

    return FReply::Handled();
}

bool
FBLImageDetails::FilterAsset( const struct FAssetData& InAssetData )
{
    static FString vectorBrushesDir = FString("/Brushes/Vector");
    // If the asset lies in the Vector Brush directory, show it (return false).
    // It was the only way to have the content browser display the vector brushes onyl.
    // All other solutions I've tried using the FAssetPickerConfig.Filter didn't seem
    // to offer the possibility to filter a specific folder.
    if( InAssetData.ObjectPath.ToString().Contains( vectorBrushesDir ) )
    {
        return false;
    }

    return true;
}

//FString
void
FBLImageDetails::ObjectPath( TSharedRef<IPropertyHandle> StructPropertyHandle ) 
{
    FProperty *property = StructPropertyHandle.Get().GetProperty();
    TArray<UObject*> OuterObjects;

    StructPropertyHandle.Get().GetOuterObjects( OuterObjects );

    if( OuterObjects.Num() == 1 )
    {
        UObject* OuterObject = OuterObjects[0];
        const FOdysseyVectorBrush* vectorBrush = property->ContainerPtrToValuePtr<FOdysseyVectorBrush>( OuterObject, 0 ); 

        mBrushIcon->SetResourceObject( vectorBrush->texture );

        //return ( vectorBrush->texture ) ? vectorBrush->texture->GetPathName() : FString();
    }

    //return FString();
}

void
FBLImageDetails::OnObjectChanged( const FAssetData& AssetData
                                , TSharedRef<IPropertyHandle> StructPropertyHandle )
{
    UTexture2D* texture = AssetData.IsValid() ? CastChecked<UTexture2D>( AssetData.GetAsset() ) : nullptr;
    FProperty *property = StructPropertyHandle.Get().GetProperty();
    TArray<UObject*> OuterObjects;

    StructPropertyHandle.Get().GetOuterObjects( OuterObjects );

    if( texture )
    {
        // We have to change the texture settings or else we won't be able to read the pixels.
        // This loader is only vector-brush oriented, so it does not really matter, we can
        // leave the textures that way.
        texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
        texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
        texture->SRGB = false;
        texture->UpdateResource();
    }

    mBrushIcon->SetResourceObject( texture );

    for ( int i = 0; i < OuterObjects.Num(); i++ )
    {
        UObject* OuterObject = OuterObjects[0];
        FPropertyChangedEvent propertyChangedEvent = FPropertyChangedEvent( property
                                                                          , EPropertyChangeType::ValueSet
                                                                          , OuterObjects );
        FOdysseyVectorBrush* vectorBrush = property->ContainerPtrToValuePtr<FOdysseyVectorBrush>( OuterObject, 0 ); 

        vectorBrush->texture = texture;

        OuterObject->PostEditChangeProperty( propertyChangedEvent );
    }
}

void
FBLImageDetails::CustomizeChildren( TSharedRef<IPropertyHandle> StructPropertyHandle
                                  , class IDetailChildrenBuilder& StructBuilder
		                          , IPropertyTypeCustomizationUtils& StructCustomizationUtils )
{
    StructPropertyHandle->SetOnPropertyValueChanged( FSimpleDelegate::CreateRaw( this, &FBLImageDetails::ObjectPath, StructPropertyHandle ) );

}

void
FBLImageCustomization::Register()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomPropertyTypeLayout(FOdysseyVectorBrush::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FBLImageDetails::MakeInstance));

    PropertyModule.NotifyCustomizationModuleChanged();
}

#undef LOCTEXT_NAMESPACE
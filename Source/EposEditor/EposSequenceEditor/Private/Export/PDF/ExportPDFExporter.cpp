// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/PDF/ExportPDFExporter.h"

#include "Engine/TextureRenderTarget2D.h"
#include "HAL/FileManager.h"
#include "ImageUtils.h"
#include "IImageWrapperModule.h"
#include "Input/HittestGrid.h"
#include "ISequencer.h"
#include "Serialization/BufferArchive.h"
#include "Slate/WidgetRenderer.h"

#include "Export/ExportStruct.h"
#include "Export/SceneRenderer.h"
#include "Export/PDF/ExportPDFSettings.h"
#include "Export/PDF/PDFDocExportWidget.h"

#include "hpdf.h"

#define LOCTEXT_NAMESPACE "ExportPDFExporter"

// Disable warning "interaction between '_setjmp' and C++ object destruction is non-portable"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4611)
#endif

jmp_buf sgPDFEnv;

void
pdf_error_handler( HPDF_STATUS error_no, HPDF_STATUS detail_no, void* user_data )
{
    printf( "ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no, (HPDF_UINT)detail_no );

    longjmp( sgPDFEnv, 1 );
}


//---

FExportPDFExporter::FExportPDFExporter( TWeakPtr<ISequencer> iSequencer, const FExportStruct* iStruct, const FExportPDFOptions* iOptions )
    : mSequencer( iSequencer )
    , mStruct( iStruct )
    , mPDFOptions( iOptions )
{
}

bool
FExportPDFExporter::Export()
{
    UPDFDocExportWidget* pdf_widget = nullptr;
    UClass* pdf_doc_class = mPDFOptions->PDFDocWidgetSoftClass.LoadSynchronous();
    if( !pdf_doc_class )
        return false;

    UWorld* world = GEditor->GetEditorWorldContext().World();
    pdf_widget = CreateWidget<UPDFDocExportWidget>( world, pdf_doc_class );
    check( pdf_widget );

    pdf_widget->OnConstructPDFLayout( *mStruct, false );

    int32 number_of_pages = pdf_widget->GetTotalPDFPageNumber();

    //---

    HPDF_Doc pdf = HPDF_New( pdf_error_handler, NULL );
    if( !pdf )
    {
        printf( "ERROR: cannot create pdf object.\n" );
        return false;
    }
    if( setjmp( sgPDFEnv ) )
    {
        HPDF_Free( pdf );
        return false;
    }

    HPDF_SetCompressionMode( pdf, HPDF_COMP_ALL );

    /* create default-font */
    HPDF_Font font = HPDF_GetFont( pdf, "Helvetica", NULL );

    //---

    for( int32 i = 1; i <= number_of_pages; i++ )
    {
        pdf_widget->HandlePDFPageNumber( i );

        //---
        //--- Get page parameters
        //---

        HPDF_PageSizes page_size;
        switch( pdf_widget->GetPDFPageFormat( i ) )
        {
            default:
            case EPDFPageFormat::A4:
                page_size = HPDF_PAGE_SIZE_A4;
                break;
        }

        HPDF_PageDirection page_direction;
        switch( pdf_widget->GetPDFPageOrientation( i ) )
        {
            case EPDFPageOrientation::Portrait:
                page_direction = HPDF_PAGE_PORTRAIT;
                break;
            default:
            case EPDFPageOrientation::Landscape:
                page_direction = HPDF_PAGE_LANDSCAPE;
                break;
        }

        // Add a new page object
        HPDF_Page page = HPDF_AddPage( pdf );

        // Set the layout of the page
        HPDF_Page_SetSize( page, page_size, page_direction );

        HPDF_REAL page_width = HPDF_Page_GetWidth( page );
        HPDF_REAL page_height = HPDF_Page_GetHeight( page );

        // Compute the ratio of the page
        float page_ratio = page_width / page_height; // >1 -> landscape | <1 -> portrait

        // Compute the size of the window (respecting the ratio of the page)
        FVector2D window_size;
        if( page_ratio < 1 )
            window_size.Set( 1920 * page_ratio, 1920 );
        else
            window_size.Set( 1920, 1920 / page_ratio );

        //DEBUG
        //window_size.Set( page_width, page_height );
        //window_size *= 2.f;
        //window_size /= 2.f;

        //---
        //--- Create a virtual window corresponding to the page size and fill it with the widget
        //---

        //--- From ...\Editor\UMGEditor\Private\WidgetBlueprintEditorUtils.cpp#2183 : FWidgetBlueprintEditorUtils::DrawSWidgetInRenderTargetInternal(...)

        FVector2D Offset( 0.f, 0.f );
        FVector2D ScaledSize( window_size );
        //FVector2D ScaledSize( 0.f, 0.f );
        TSharedPtr<SWidget> WindowContent = pdf_widget->TakeWidget();

        TSharedRef<SVirtualWindow> Window = SNew( SVirtualWindow );
        TUniquePtr<FHittestGrid> HitTestGrid = MakeUnique<FHittestGrid>();
        Window->SetContent( WindowContent.ToSharedRef() );
        Window->Resize( ScaledSize );
        //Window->Resize( FVector2D( 1920, 1080 ) );

        //-

        Window->SlatePrepass( 1.0f );
        FVector2D DesiredSizeWindow = Window->GetDesiredSize();

        //ScaledSize = FVector2D( 1920, 1080 );
        //ScaledSize = UnscaledSize * Scale;

        //---

        UTextureRenderTarget2D* TextureRenderTarget = NewObject<UTextureRenderTarget2D>();

        TextureRenderTarget->Filter = TF_Bilinear;
        TextureRenderTarget->ClearColor = FLinearColor::Transparent;
        TextureRenderTarget->SRGB = true;
        TextureRenderTarget->RenderTargetFormat = RTF_RGBA8;

        uint32 ScaledSizeX = static_cast<uint32>( ScaledSize.X );
        uint32 ScaledSizeY = static_cast<uint32>( ScaledSize.Y );

        const bool bForceLinearGamma = false;
        const EPixelFormat RequestedFormat = FSlateApplication::Get().GetRenderer()->GetSlateRecommendedColorFormat();
        TextureRenderTarget->InitCustomFormat( ScaledSizeX, ScaledSizeY, RequestedFormat, bForceLinearGamma );

        //---

        FWidgetRenderer WidgetRenderer;
        WidgetRenderer.SetIsPrepassNeeded( false );

        WidgetRenderer.DrawWindow( TextureRenderTarget, *HitTestGrid, Window, 1.f, ScaledSize, 0.1f );

        float image_ratio = TextureRenderTarget->SizeX / float( TextureRenderTarget->SizeY );

        //TextureRenderTarget = WidgetRenderer.DrawWidget( mPDFDocWidget->TakeWidget(), ScaledSize );

        //---
        //--- Create and add the pdf page containing the png image
        //---

        //FRenderTarget* RenderTarget = TextureRenderTarget->GameThread_GetRenderTargetResource();
        //TArray<FColor> source_colors;
        //bool bReadSuccess = RenderTarget->ReadPixels( source_colors );
        //if( !bReadSuccess )
        //    return false;

        ////int32 resized_width = int32( page_width ) * 2;
        ////int32 resized_height = int32( page_height ) * 2;

        ////TArray<FColor> thumbnail_colors;
        ////FImageUtils::ImageResize( TextureRenderTarget->SizeX, TextureRenderTarget->SizeY, source_colors, resized_width, resized_height, thumbnail_colors, true, false );

        //TArray64<uint8> png_data;
        ////FImageUtils::PNGCompressImageArray( resized_width, resized_height, thumbnail_colors, png_data );

        //FImageUtils::PNGCompressImageArray( TextureRenderTarget->SizeX, TextureRenderTarget->SizeY, source_colors, png_data );

        //---

        FBufferArchive png_data;
        bool bSuccess = FImageUtils::ExportRenderTarget2DAsPNG( TextureRenderTarget, png_data );
        if( !bSuccess )
            return false;

        ////DEBUG
        //{
        //    check( TextureRenderTarget->GetFormat() == PF_B8G8R8A8 );

        //    FRenderTarget* RenderTarget = TextureRenderTarget->GameThread_GetRenderTargetResource();
        //    FIntPoint Size = RenderTarget->GetSizeXY();

        //    TArray64<uint8> RawData;
        //    bool bSuccess = FImageUtils::GetRawData( TextureRenderTarget, RawData );

        //    IImageWrapperModule& ImageWrapperModule = FModuleManager::Get().LoadModuleChecked<IImageWrapperModule>( TEXT( "ImageWrapper" ) );

        //    //TSharedPtr<IImageWrapper> PNGImageWrapper = ImageWrapperModule.CreateImageWrapper( EImageFormat::JPEG );
        //    TSharedPtr<IImageWrapper> PNGImageWrapper = ImageWrapperModule.CreateImageWrapper( EImageFormat::PNG );

        //    PNGImageWrapper->SetRaw( RawData.GetData(), RawData.GetAllocatedSize(), Size.X, Size.Y, ERGBFormat::BGRA, 8 );

        //    const TArray64<uint8> PNGData = PNGImageWrapper->GetCompressed( 100 );

        //    Buffer.Serialize( (void*)PNGData.GetData(), PNGData.GetAllocatedSize() );
        //}

        //---

        ////DEBUG
        //FString file_name = TEXT( "samples-c3-" ) + FString::FromInt( i ) + TEXT( ".png" );
        //FString pathfile = TEXT( "C:/Users/Mike/Documents/Unreal Projects/dev_50_epos/Plugins/Odyssey/samples/export/" ) + file_name;
        //{
        //    TUniquePtr<FArchive> PNGFileAr( IFileManager::Get().CreateFileWriter( *pathfile ) );
        //    if( !PNGFileAr )
        //        return false;

        //    PNGFileAr->Serialize( const_cast<uint8*>( png_data.GetData() ), png_data.Num() );
        //    PNGFileAr->Flush();
        //    PNGFileAr->Close();
        //    PNGFileAr = nullptr;
        //}

        //---

        HPDF_Destination dst = HPDF_Page_CreateDestination( page );
        HPDF_Destination_SetXYZ( dst, 0, HPDF_Page_GetHeight( page ), 1 );
        HPDF_SetOpenAction( pdf, dst );

        HPDF_Page_SetFontAndSize( page, font, 12 );

        HPDF_Box margin = { 0, 0, 0, 0 };
        margin.top = margin.bottom = page_height * margin.left / page_width;

        float new_image_x = margin.left;
        float new_image_y = margin.top;
        float new_image_width;
        float new_image_height;

        if( page_ratio < image_ratio )
        {
            new_image_width = page_width - margin.left - margin.right;
            new_image_height = new_image_width / image_ratio;
        }
        else
        {
            new_image_height = page_height - margin.top - margin.bottom;
            new_image_width = new_image_height * image_ratio;
        }

        //HPDF_Image image = HPDF_LoadJpegImageFromMem( pdf, const_cast<uint8*>( Buffer.GetData() ), Buffer.Num() );
        HPDF_Image image = HPDF_LoadPngImageFromMem( pdf, const_cast<uint8*>( png_data.GetData() ), png_data.Num() );
        //HPDF_Image image = HPDF_LoadPngImageFromFile( pdf, StringCast<ANSICHAR>( *pathfile ).Get() );

        //DEBUG
        //new_image_x = 10;
        //new_image_y = 10;
        //new_image_width = HPDF_Image_GetWidth( image );
        //new_image_height = HPDF_Image_GetHeight( image );

        // Draw image to the canvas
        HPDF_Page_DrawImage( page
                             , image
                             , new_image_x, new_image_y /* 0, 0 is on bottom left of the page */
                             , new_image_width, new_image_height
        );
    }

    if( !IFileManager::Get().DirectoryExists( *mPDFOptions->ExportPath.Path ) )
        IFileManager::Get().MakeDirectory( *mPDFOptions->ExportPath.Path, true /* bRecursive */ );

    // Save the document to a file
    FString pdf_extension( TEXT( ".pdf" ) );
    FString pathfile = mPDFOptions->ExportPath.Path / mPDFOptions->ExportFile;
    if( !pathfile.EndsWith( pdf_extension ) )
        pathfile += pdf_extension;

    HPDF_SaveToFile( pdf, StringCast<ANSICHAR>( *pathfile ).Get() );

    // Clean up
    HPDF_Free( pdf );

    return true;
}

//---

// Renable warning "interaction between '_setjmp' and C++ object destruction is non-portable"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#undef LOCTEXT_NAMESPACE

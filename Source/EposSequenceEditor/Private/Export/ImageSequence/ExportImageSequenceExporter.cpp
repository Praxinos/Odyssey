// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/ImageSequence/ExportImageSequenceExporter.h"

#include "ImageWriteQueue.h"
#include "ImageWriteTask.h"
#include "ISequencer.h"

#include "Export/SceneRenderer.h"
#include "Export/ImageSequence/ExportImageSequenceNamingFormatter.h"
#include "Export/ImageSequence/ExportImageSequenceSettings.h"
#include "Export/ExportStruct.h"

#define LOCTEXT_NAMESPACE "ExportImageSequenceExporter"

//---

FExportImageSequenceExporter::FExportImageSequenceExporter( TWeakPtr<ISequencer> iSequencer, const FExportStruct* iStruct, const FExportImageSequenceOptions* iOptions )
    : mSequencer( iSequencer )
    , mStruct( iStruct )
    , mImageSequenceOptions( iOptions )
{
    mImageWriteQueue = &FModuleManager::LoadModuleChecked<IImageWriteQueueModule>( "ImageWriteQueue" ).GetWriteQueue();
}

bool
FExportImageSequenceExporter::Export()
{
    TArray<FColor> samples;

    for( int32 i = 0; i < mStruct->Panels.Num(); i++ )
    {
        FExportImageSequenceNamingFormatter name_formatter( mSequencer, &mStruct->Panels[i], i, mImageSequenceOptions );
        FString name;
        bool is_formatted = name_formatter.FormatName( mImageSequenceOptions->Pattern, name );
        if( !is_formatted )
            continue;

        FString pathfile = mImageSequenceOptions->ExportPath.Path / name;

        FSceneRenderer renderer( mSequencer, &mStruct->Panels[i], mImageSequenceOptions->ImageSize );
        bool rendering = renderer.RenderPlane( samples );
        if( !rendering )
            continue;

        TUniquePtr<FImageWriteTask> ImageTask = MakeUnique<FImageWriteTask>();
        ImageTask->Format = EImageFormat( mImageSequenceOptions->FileFormat );
        ImageTask->CompressionQuality = 100;
        ImageTask->Filename = pathfile;
        ImageTask->PixelData = MakeUnique<TImagePixelData<FColor>>( mImageSequenceOptions->ImageSize, TArray64<FColor>( MoveTemp( samples ) ) );
        ImageTask->PixelPreProcessors.Add( TAsyncAlphaWrite<FColor>( 255 ) );

        /*TFuture<bool> CompletionFuture =*/ mImageWriteQueue->Enqueue( MoveTemp( ImageTask ) );
    }

    return true;
}

//---

#undef LOCTEXT_NAMESPACE

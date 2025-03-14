// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

//---

struct FExportImageSequenceOptions;
struct FExportStruct;
class IImageWriteQueue;
class ISequencer;

class FExportImageSequenceExporter
{
public:
    FExportImageSequenceExporter( TWeakPtr<ISequencer> iSequencer, const FExportStruct* iStruct, const FExportImageSequenceOptions* iOptions );

    bool Export();

private:
    TWeakPtr<ISequencer>                mSequencer;
    const FExportStruct*                mStruct { nullptr };
    const FExportImageSequenceOptions*  mImageSequenceOptions { nullptr };

    IImageWriteQueue*                   mImageWriteQueue { nullptr };
};

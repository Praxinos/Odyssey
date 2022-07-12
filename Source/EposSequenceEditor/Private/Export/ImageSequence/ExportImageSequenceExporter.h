// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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

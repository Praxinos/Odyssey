#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationCellsContainer;
class FOdysseyAnimationCell;

class FOdysseyAnimationCellsContainerExport
{
public:
    static void ODYSSEYANIMATION_API Write( FOdysseyAnimationCellsContainer* iAnimationCellsContainer, FArchive &Ar );

private:
    ////////////////////////////////////
    static void WriteOffset( FOdysseyAnimationCellsContainer* iAnimationCellsContainer, FArchive &Ar );
    static void WriteCells( FOdysseyAnimationCellsContainer* iAnimationCellsContainer, FArchive &Ar );
    static void WriteCellType( TSharedPtr<FOdysseyAnimationCell> iCell, FArchive &Ar );
    static void WriteCell( TSharedPtr<FOdysseyAnimationCell> iCell, FArchive &Ar );
};

#include "Undo/OdysseyVectorUndo.h"

FOdysseyVectorUndo::~FOdysseyVectorUndo()
{
}

FOdysseyVectorUndo::FOdysseyVectorUndo()
    : mApplied( true )
{
}

void
FOdysseyVectorUndo::Apply( UObject* iIgnored )
{
    mApplied = true;
}

void
FOdysseyVectorUndo::Revert( UObject* iIgnored )
{
    mApplied = false;
}

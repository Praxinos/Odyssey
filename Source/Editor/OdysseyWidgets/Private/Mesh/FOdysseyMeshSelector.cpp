// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Mesh/FOdysseyMeshSelector.h"

#define LOCTEXT_NAMESPACE "OdysseyMeshSelector"


/////////////////////////////////////////////////////
// FOdysseyMeshSelector
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyMeshSelector::FOdysseyMeshSelector() :
    CurrentMesh(nullptr),
    CurrentLOD(-1),
    CurrentUV(-1),
    MeshColor( FLinearColor( 0.8f, 0.8f, 0.8f, 0.25f ))
{
}


FOdysseyMeshSelector::~FOdysseyMeshSelector()
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Getter / Setter

UStaticMesh* FOdysseyMeshSelector::GetCurrentMesh() const
{
    return CurrentMesh;
}

void FOdysseyMeshSelector::SetCurrentMesh( UStaticMesh* InCurrentMesh )
{
    CurrentMesh = InCurrentMesh;
}

int FOdysseyMeshSelector::GetCurrentLOD() const
{
    return CurrentLOD;
}


int FOdysseyMeshSelector::GetCurrentUVChannel() const
{
    return CurrentUV;
}

int FOdysseyMeshSelector::GetMaxLOD() const
{
    if( !CurrentMesh )
        return -1;

    if( !CurrentMesh->RenderData )
        return -1;

    return CurrentMesh->RenderData->LODResources.Num();
}

int FOdysseyMeshSelector::GetMaxUVChannelForCurrentLOD()
{
    int NumLODLevels = CurrentMesh->RenderData->LODResources.Num();

    if( CurrentLOD < 0 || CurrentLOD > NumLODLevels )
        return -1;


    return CurrentMesh->RenderData->LODResources[CurrentLOD].VertexBuffers.StaticMeshVertexBuffer.GetNumTexCoords();
}

FLinearColor FOdysseyMeshSelector::GetMeshColor() const
{
    return MeshColor;
}

//CALLBACKS -------------------------------------------


void FOdysseyMeshSelector::SetMeshColor( FLinearColor InNewColor )
{
    MeshColor = InNewColor;
}

void FOdysseyMeshSelector::SetCurrentLOD(int InNewLOD)
{
    CurrentLOD = FMath::Clamp(InNewLOD, -1, GetMaxLOD());
}

void FOdysseyMeshSelector::SetCurrentUVChannel(int InNewUV)
{
    CurrentUV = FMath::Clamp(InNewUV, -1, GetMaxUVChannelForCurrentLOD());
}

ECheckBoxState FOdysseyMeshSelector::GetLODCheckState(int InLOD)
{
    return (CurrentLOD == InLOD ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
}

ECheckBoxState FOdysseyMeshSelector::GetUVChannelCheckState(int InUVChannel)
{
    return (CurrentUV == InUVChannel ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
}

#undef LOCTEXT_NAMESPACE

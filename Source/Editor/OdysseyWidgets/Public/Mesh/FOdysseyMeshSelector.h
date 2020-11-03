// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
#include "Styling/SlateTypes.h"
#include "Color/SOdysseyColorSlider.h"

/**
 * The Model of the MeshSelector
 */
class ODYSSEYWIDGETS_API FOdysseyMeshSelector :  public TSharedFromThis<FOdysseyMeshSelector>
{

    friend class SOdysseyMeshSelector; // For the access to private callbacks

public:
    // Construction / Destruction
    FOdysseyMeshSelector();
    ~FOdysseyMeshSelector();

public:
    //Getter / Setter
    UStaticMesh* GetCurrentMesh() const;
    void SetCurrentMesh( UStaticMesh* inCurrentMesh );
    int GetCurrentLOD() const;
    int GetCurrentUVChannel() const;
    int GetMaxLOD() const;
    int GetMaxUVChannelForCurrentLOD();
    FLinearColor GetMeshColor() const;

private:
    //CallBacks
    void SetMeshColor( FLinearColor InNewColor );
    void SetCurrentLOD(int InNewLOD);
    void SetCurrentUVChannel(int InNewUV);
    ECheckBoxState GetLODCheckState(int InLOD);
    ECheckBoxState GetUVChannelCheckState( int InUVChannel );


private:
    // Private data
    UStaticMesh* CurrentMesh;

    /** The Level Of Detail we draw the UV Map of*/
    int CurrentLOD;

    /** The UV we currentlty wish to display */
    int CurrentUV;

    /** The HUD color of the Mesh */
    FLinearColor MeshColor;

};

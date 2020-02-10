// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"


enum class ePosition : char
{
    kIn,
    kBefore,
    kAfter,
    kInvalid
};

template< typename T > class ODYSSEYMATHS_API FOdysseyNTree {

//Construction Destruction
public:
    FOdysseyNTree( T iNodeContent );
    ~FOdysseyNTree();
    
public:
    FOdysseyNTree<T>* AddNode( T iNodeContent, int iIndexEmplace = -1 );
    FOdysseyNTree<T>* AddNode( FOdysseyNTree<T>* iNode, int iIndexEmplace = -1 );

    void DeleteNode( int iIndex );
    FOdysseyNTree<T>* MoveNodeTo( FOdysseyNTree* iNewPositionInTree, ePosition iPosition = ePosition::kAfter );
    
    T GetNodeContent();
    T* GetNodeContentPtr();
    void SetNodeContent( T& iNodeContent );
    
    TArray<FOdysseyNTree*> GetNodes();
    
    void DepthFirstSearchTree( TArray<T>* ioContents );
    
    
private:
    T mNodeContent;
    TArray<FOdysseyNTree*> mNodes;
    FOdysseyNTree* mParent;
    
};

//Implementation
#include "OdysseyTree.inl"

#pragma once
#include "common_include.h"
#include "Core/Manager/Manager_Base.h"
#include "Core/Utils/Math/MathFwd.h"
#include "Node.h"
#include "AStar.h"
#include "Core/Graphics/Texture/JTexture.h"

using namespace DirectX;
using namespace NAV;

#define NAV_MAP NavTest::Get()


class TestGrid;

class NavTest : public TSingleton<NavTest>
{
public:
    void Initialize();
    void Update(float DeltaTime);
    void Render();
    void Release();

public:
    void DrawNode(FVector2 grid, FXMVECTOR InColor);
    FVector WorldPosFromGridPos(int row, int col);
    FVector WorldPosFromGridPos(FVector2 worldGrid);
    FVector2 GridFromWorldPoint(FVector worldPos);
    void SetGraph();
    void SetObstacle();
    void SetChildNode(int i, int j);
    void DrawUnWalkable();
    void LoadMapFile(const JText& FileName);
    
    FVector PlayerPos;
    FVector NewPPos;
    FVector NpcPos;
    // FVector NewNPos;
    // std::vector<FVector> Npcs;
    // std::vector<FVector> NewNpcs;
    std::vector<FVector2> Obstacles;
    
    FVector2 GridWorldSize;
    float NodeRadius;
    FVector NodeCenter;
    float NodeDiameter;
    FVector2 GridDivs;
    FVector2 NodeScale;
    FVector GridCenter = FVector::ZeroVector;
    FVector GridTopLeft = FVector::ZeroVector;
    bool firstRun = true;
    int ObstacleScale  = 1;
    JTexture* MapFile;
    
    std::vector<std::vector<Ptr<Node>>> mGridGraph;
    std::vector<Ptr<NAV::Node>> tempPath;
    
};

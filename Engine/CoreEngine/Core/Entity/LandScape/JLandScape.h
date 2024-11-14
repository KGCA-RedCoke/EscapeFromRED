#pragma once
#include "Core/Entity/Actor/JActor.h"
#include "Core/Graphics/ShaderStructs.h"

class JTexture;

struct FMapDesc
{
	int32_t Column;
	int32_t Row;
	float   CellDistance;
	float   ScaleHeight;
};

class JLandScape : public JActor
{
public:
	JLandScape();
	JLandScape(JTextView InName);
	~JLandScape() override;

public:
	void Initialize() override;
	void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void Destroy() override;

public:
	void Draw() override;

public:
	bool Serialize_Implement(std::ofstream& FileStream) override;
	bool DeSerialize_Implement(std::ifstream& InFileStream) override;

private:
	void GenerateLandScape();
	void GenVertex();
	void GenIndex();
	void GenFaceNormal();
	void GenVertexNormal();
	void GenBuffer();

protected:
	FMapDesc mMapDescription;

	using IndexGroup = JArray<uint32_t>;

	JArray<Vertex::FVertexInfo_Base>  mVertexInfo;
	JArray<uint32_t>                  mIndexInfo;
	JArray<FVector>                   mFaceInfo;
	JArray<IndexGroup>                mVertexGroup;
	Buffer::FBufferInstance_LandScape mInstanceBuffer;

	Ptr<class JMaterialInstance> mMaterial;

	Ptr<JTexture> mAlbedoMap;
	Ptr<JTexture> mHeightMap;
	Ptr<JTexture> mNormalMap;

	friend class GUI_Editor_LandScape;
};

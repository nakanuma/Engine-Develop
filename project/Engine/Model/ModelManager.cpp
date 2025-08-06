#include "ModelManager.h"
#include <DirectXBase.h>
#include <DirectXUtil.h>
#include <fstream>
#include <numbers>
#include <sstream>

#include <SRVManager.h>

ModelManager::ModelData ModelManager::CreateRingModel(ID3D12Device* device, float outerRadius, float innerRadius) {
    ModelManager::ModelData modelData;

    const uint32_t kRingDevide = 32; // 分割数
    const float radianPerDivide = 2.0f * std::numbers::pi_v<float> / float(kRingDevide);

    // 頂点データとインデックスデータのコンテナ
    std::vector<ModelManager::VertexData> vertices;
    std::vector<uint32_t> indices;

    // リング形状の頂点を生成
    for (uint32_t index = 0; index < kRingDevide; ++index) {
        float sin = std::sin(index * radianPerDivide);
        float cos = std::cos(index * radianPerDivide);
        float sinNext = std::sin((index + 1) * radianPerDivide);
        float cosNext = std::cos((index + 1) * radianPerDivide);
        float u = float(index) / float(kRingDevide);
        float uNext = float(index + 1) / float(kRingDevide);

        // 頂点の定義
        vertices.push_back({
            {-sin * outerRadius, 0.0f, cos * outerRadius, 1.0f}, // 外側1
            {u, 0.0f}, // UV
            {0.0f, 1.0f, 0.0f} // 法線
        });
        vertices.push_back({
            {-sinNext * outerRadius, 0.0f, cosNext * outerRadius, 1.0f}, // 外側2
            {uNext, 0.0f}, // UV
            {0.0f, 1.0f, 0.0f} // 法線
        });
        vertices.push_back({
            {-sin * innerRadius, 0.0f, cos * innerRadius, 1.0f}, // 内側1
            {u, 1.0f}, // UV
            {0.0f, 1.0f, 0.0f} // 法線
        });
        vertices.push_back({
            {-sinNext * innerRadius, 0.0f, cosNext * innerRadius, 1.0f}, // 内側2
            {uNext, 1.0f}, // UV
            {0.0f, 1.0f, 0.0f} // 法線
        });

        // インデックスを生成
        indices.push_back(4 * index + 0);
        indices.push_back(4 * index + 2);
        indices.push_back(4 * index + 1);
        indices.push_back(4 * index + 1);
        indices.push_back(4 * index + 2);
        indices.push_back(4 * index + 3);
    }

	modelData.vertices = vertices;
	modelData.indices = indices;

    // 頂点バッファリソースを作成
    modelData.vertexResource = CreateBufferResource(device, sizeof(ModelManager::VertexData) * vertices.size());

    // 頂点バッファビューを設定
	modelData.vertexBufferView;
    modelData.vertexBufferView.BufferLocation = modelData.vertexResource->GetGPUVirtualAddress();
    modelData.vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * vertices.size());
    modelData.vertexBufferView.StrideInBytes = sizeof(VertexData);

    // 頂点データをコピー
    VertexData* vertexData = nullptr;
    modelData.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
    std::memcpy(vertexData, vertices.data(), sizeof(VertexData) * vertices.size());

    // インデックスバッファリソースを作成
    modelData.indexResource = CreateBufferResource(device, sizeof(uint32_t) * indices.size());

    // インデックスバッファビューを設定
    modelData.indexBufferView.BufferLocation = modelData.indexResource->GetGPUVirtualAddress();
    modelData.indexBufferView.SizeInBytes = UINT(sizeof(uint32_t) * indices.size());
    modelData.indexBufferView.Format = DXGI_FORMAT_R32_UINT;

    // インデックスデータをコピー
    uint32_t* indexData = nullptr;
    modelData.indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
    std::memcpy(indexData, indices.data(), sizeof(uint32_t) * indices.size());

    return modelData;
}

ModelManager::ModelData ModelManager::CreateCylinderModel(ID3D12Device* device) { 
	ModelManager::ModelData modelData;
	
	const uint32_t kCylinderDevide = 32; // 分割数
	const float kTopRadius = 1.0f;		 // 上半径
	const float kBottomRadius = 1.0f;	 // 下半径
	const float kHeight = 3.0f;			 // 高さ
	const float radianPerDivide = 2.0f * std::numbers::pi_v<float> / float(kCylinderDevide);

	// 頂点データとインデックスデータのコンテナ
	std::vector<ModelManager::VertexData> vertices;
	std::vector<uint32_t> indices;

	// Cylinderの頂点データを生成
	for (uint32_t index = 0; index < kCylinderDevide; ++index) {
		float sin = std::sin(index * radianPerDivide);
		float cos = std::cos(index * radianPerDivide);
		float sinNext = std::sin((index + 1) * radianPerDivide);
		float cosNext = std::cos((index + 1) * radianPerDivide);
		float u = float(index) / float(kCylinderDevide);
		float uNext = float(index + 1) / float(kCylinderDevide);
		
		// 頂点の定義 : {position, texcoord(V方向を反転), normal}
		vertices.push_back({
		    {-sin * kTopRadius, kHeight, cos * kTopRadius, 1.0f},
            {u, 1.0f - 0.0f},
            {-sin, 0.0f, cos}
        });
		vertices.push_back({
		    {-sinNext * kTopRadius, kHeight, cosNext * kTopRadius, 1.0f},
            {uNext, 1.0f - 0.0f},
		    {-sinNext, 0.0f, cosNext}
		});

		vertices.push_back({
		    {-sin * kBottomRadius, 0.0f, cos * kBottomRadius, 1.0f},
            {u, 1.0f - 1.0f},
		    {-sin, 0.0f, cos}
		});
		vertices.push_back({
		    {-sin * kBottomRadius, 0.0f, cos * kBottomRadius, 1.0f},
            {u, 1.0f - 1.0f},
		    {-sin, 0.0f, cos}
		});

		vertices.push_back({
		    {-sinNext * kTopRadius, kHeight, cosNext * kTopRadius, 1.0f},
            {uNext, 1.0f - 0.0f},
		    {-sinNext, 0.0f, cosNext}
		});
		vertices.push_back({
		    {-sinNext * kBottomRadius, 0.0f, cosNext * kBottomRadius, 1.0f},
            {uNext, 1.0f - 1.0f},
		    {-sinNext, 0.0f, cosNext}
		});

		// インデックスを生成
		indices.push_back(6 * index + 0);
		indices.push_back(6 * index + 1);
		indices.push_back(6 * index + 2);
		indices.push_back(6 * index + 3);
		indices.push_back(6 * index + 4);
		indices.push_back(6 * index + 5);
	}

	modelData.vertices = vertices;
	modelData.indices = indices;

	// 頂点バッファリソースを作成
	modelData.vertexResource = CreateBufferResource(device, sizeof(ModelManager::VertexData) * vertices.size());
	
	// 頂点バッファビューを設定
	modelData.vertexBufferView;
	modelData.vertexBufferView.BufferLocation = modelData.vertexResource->GetGPUVirtualAddress();
	modelData.vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * vertices.size());
	modelData.vertexBufferView.StrideInBytes = sizeof(VertexData);

	// 頂点データをコピー
	VertexData* vertexData = nullptr;
	modelData.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, vertices.data(), sizeof(VertexData) * vertices.size());


	// インデックスバッファリソースを作成
	modelData.indexResource = CreateBufferResource(device, sizeof(uint32_t) * indices.size());

	// インデックスバッファビューを設定
	modelData.indexBufferView.BufferLocation = modelData.indexResource->GetGPUVirtualAddress();
	modelData.indexBufferView.SizeInBytes = UINT(sizeof(uint32_t) * indices.size());
	modelData.indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	// インデックスデータをコピー
	uint32_t* indexData = nullptr;
	modelData.indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	std::memcpy(indexData, indices.data(), sizeof(uint32_t) * indices.size());

	return modelData;
}

ModelManager::ModelData ModelManager::CreateSkyBoxModel(ID3D12Device* device) {
	ModelManager::ModelData modelData;

	// 頂点データとインデックスデータのコンテナ
	std::vector<VertexData> vertices(24);
	std::vector<uint32_t> indices;

	// SkyBoxの頂点データを生成
	// 右面
	vertices[0].position = {1.0f, 1.0f, 1.0f, 1.0f};
	vertices[1].position = {1.0f, 1.0f, -1.0f, 1.0f};
	vertices[2].position = {1.0f, -1.0f, 1.0f, 1.0f};
	vertices[3].position = {1.0f, -1.0f, -1.0f, 1.0f};
	// 左面
	vertices[4].position = {-1.0f, 1.0f, -1.0f, 1.0f};
	vertices[5].position = {-1.0f, 1.0f, 1.0f, 1.0f};
	vertices[6].position = {-1.0f, -1.0f, -1.0f, 1.0f};
	vertices[7].position = {-1.0f, -1.0f, 1.0f, 1.0f};
	// 前面
	vertices[8].position = {-1.0f, 1.0f, 1.0f, 1.0f};
	vertices[9].position = {1.0f, 1.0f, 1.0f, 1.0f};
	vertices[10].position = {-1.0f, -1.0f, 1.0f, 1.0f};
	vertices[11].position = {1.0f, -1.0f, 1.0f, 1.0f};
	// 後面
	vertices[12].position = {1.0f, 1.0f, -1.0f, 1.0f};
	vertices[13].position = {-1.0f, 1.0f, -1.0f, 1.0f};
	vertices[14].position = {1.0f, -1.0f, -1.0f, 1.0f};
	vertices[15].position = {-1.0f, -1.0f, -1.0f, 1.0f};
	// 上面
	vertices[16].position = {-1.0f, 1.0f, -1.0f, 1.0f};
	vertices[17].position = {1.0f, 1.0f, -1.0f, 1.0f};
	vertices[18].position = {-1.0f, 1.0f, 1.0f, 1.0f};
	vertices[19].position = {1.0f, 1.0f, 1.0f, 1.0f};
	// 下面
	vertices[20].position = {-1.0f, -1.0f, 1.0f, 1.0f};
	vertices[21].position = {1.0f, -1.0f, 1.0f, 1.0f};
	vertices[22].position = {-1.0f, -1.0f, -1.0f, 1.0f};
	vertices[23].position = {1.0f, -1.0f, -1.0f, 1.0f};

	// texcoord, normalは適当に埋める
	for (auto& v : vertices) {
		v.texcoord = {0.0f, 0.0f};
		v.normal = {0.0f, 1.0f, 0.0f};
	}

	// 各面のインデックス（内側を向くように
	for (uint32_t i = 0; i < 6; ++i) {
		uint32_t base = i * 4;
		indices.push_back(base + 0);
		indices.push_back(base + 1);
		indices.push_back(base + 2);
		indices.push_back(base + 2);
		indices.push_back(base + 1);
		indices.push_back(base + 3);
	}

	modelData.vertices = vertices;
	modelData.indices = indices;

	// 頂点バッファリソースを作成
	modelData.vertexResource = CreateBufferResource(device, sizeof(ModelManager::VertexData) * vertices.size());

	// 頂点バッファビューを設定
	modelData.vertexBufferView;
	modelData.vertexBufferView.BufferLocation = modelData.vertexResource->GetGPUVirtualAddress();
	modelData.vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * vertices.size());
	modelData.vertexBufferView.StrideInBytes = sizeof(VertexData);

	// 頂点データをコピー
	VertexData* vertexData = nullptr;
	modelData.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, vertices.data(), sizeof(VertexData) * vertices.size());

	// インデックスバッファリソースを作成
	modelData.indexResource = CreateBufferResource(device, sizeof(uint32_t) * indices.size());

	// インデックスバッファビューを設定
	modelData.indexBufferView.BufferLocation = modelData.indexResource->GetGPUVirtualAddress();
	modelData.indexBufferView.SizeInBytes = UINT(sizeof(uint32_t) * indices.size());
	modelData.indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	// インデックスデータをコピー
	uint32_t* indexData = nullptr;
	modelData.indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	std::memcpy(indexData, indices.data(), sizeof(uint32_t) * indices.size());

	return modelData;
}

ModelManager::ModelData ModelManager::LoadModelFile(const std::string& directoryPath, const std::string& filename, ID3D12Device* device) {
	// 1. 中で必要となる変数の宣言
	ModelData modelData;           // 構築するModelData
	std::vector<Float4> positions; // 位置
	std::vector<Float3> normals;   // 法線
	std::vector<Float2> texcoords; // テクスチャ座標
	std::string line;              // ファイルから読んだ1行を格納するもの

	// 2. ファイルを開く
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes()); // メッシュがないのは対応しない

	// RootNodeを読む
	modelData.rootNode = ReadNode(scene->mRootNode);

	// 3. 実際にファイルを読み、ModelDataを構築していく
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());                    // 法線がないMeshは今回は非対応
		assert(mesh->HasTextureCoords(0));             // TexcoordがないMeshは今回非対応
		modelData.vertices.resize(mesh->mNumVertices); // 最初に頂点数分のメモリを確保しておく
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
			/// 右手系->左手系への変換
			modelData.vertices[vertexIndex].position = {-position.x, position.y, position.z, 1.0f};
			modelData.vertices[vertexIndex].normal = {-normal.x, normal.y, normal.z};
			modelData.vertices[vertexIndex].texcoord = {texcoord.x, texcoord.y};
		}
		// Indexの解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);

			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				modelData.indices.push_back(vertexIndex);
			}
		}
		// SkinCluster構築用のデータ取得を追加
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			aiBone* bone = mesh->mBones[boneIndex];
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = modelData.skinClusterData[jointName];

			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
			aiVector3D scale, translate;
			aiQuaternion rotate;
			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
			Matrix bindPoseMatrix = Matrix::MakeAffine({scale.x, scale.y, scale.z}, {rotate.x, -rotate.y, -rotate.z, rotate.w}, {-translate.x, translate.y, translate.z});
			jointWeightData.inverseBindPoseMatrix = Matrix::Inverse(bindPoseMatrix);

			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeights.push_back({bone->mWeights[weightIndex].mWeight, bone->mWeights[weightIndex].mVertexId});
			}
		}
	}

	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.material.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
		}
	}

	// vertexResourceの作成
	modelData.vertexResource = CreateBufferResource(DirectXBase::GetInstance()->GetDevice(), sizeof(VertexData) * modelData.vertices.size());

	// 頂点バッファビューを作成する
	modelData.vertexBufferView;
	// リソースの先頭のアドレスから使う
	modelData.vertexBufferView.BufferLocation = modelData.vertexResource->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点のサイズ
	modelData.vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	// 1頂点あたりのサイズ
	modelData.vertexBufferView.StrideInBytes = sizeof(VertexData);

	// 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	// 書き込むためのアドレスを取得
	modelData.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	// 頂点データをリソースにコピー
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());

	// indexResourceの作成
	modelData.indexResource = CreateBufferResource(DirectXBase::GetInstance()->GetDevice(), sizeof(uint32_t) * modelData.indices.size());

	// インデックスバッファビューを作成する
	modelData.indexBufferView.BufferLocation = modelData.indexResource->GetGPUVirtualAddress();
	modelData.indexBufferView.SizeInBytes = UINT(sizeof(uint32_t) * modelData.indices.size());
	modelData.indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	// インデックスリソースにデータを書き込む
	uint32_t* indexData = nullptr;
	modelData.indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	std::memcpy(indexData, modelData.indices.data(), sizeof(uint32_t) * modelData.indices.size());

	// 4. ModelDataを返す
	return modelData;
}

ModelManager::MaterialData ModelManager::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename, ID3D12Device* device) {
	// 1. 中で必要となる変数の宣言
	MaterialData materialData; // 構築するMaterialData
	std::string line;          // ファイルから読んだ1行を格納するもの

	// 2. ファイルを開く
	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open());                             // とりあえず開けなかったら止める

	// 3. 実際にファイルを読み、MaterialDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		// identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			// 連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
			// 画像を読み込む
			materialData.textureHandle = TextureManager::Load(materialData.textureFilePath, device);
		}
	}

	// 4. MaterialDataを返す
	return materialData;
}

ModelManager::Node ModelManager::ReadNode(aiNode* node) {
	Node result;

	aiVector3D scale, translate;
	aiQuaternion rotate;
	node->mTransformation.Decompose(scale, rotate, translate);             // assimpの行列からSRTを抽出する関数を利用
	result.transform.scale = {scale.x, scale.y, scale.z};                  // Scaleはそのまま
	result.transform.rotate = {rotate.x, -rotate.y, -rotate.z, rotate.w};  // x軸を反転、さらに回転方向が逆なので軸を反転させる
	result.transform.translate = {-translate.x, translate.y, translate.z}; // x軸を反転
	result.localMatrix = result.transform.MakeAffineMatrix();

	// aiMatrix4x4 aiLocalMatrix = node->mTransformation; // nodeのlocalMatrixを取得
	// aiLocalMatrix.Transpose(); // 列ベクトル形式を行ベクトル形式に転置
	// for (uint32_t i = 0; i < 4; ++i) { // 行列を結果にコピー
	//     for (uint32_t j = 0; j < 4; ++j) {
	//         result.localMatrix.r[i][j] = aiLocalMatrix[i][j];
	//     }
	// }
	result.name = node->mName.C_Str();          // Node名を格納
	result.children.resize(node->mNumChildren); // 子供の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		// 再帰的に読んで階層構造を作っていく
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}
	return result;
}

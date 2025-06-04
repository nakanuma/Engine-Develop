#include "ShaderManager.h"

// Engine
#include <StringUtil.h>
#include <Logger.h>

// C++
#include <cassert>

ShaderManager* ShaderManager::GetInstance() {
	static ShaderManager instance;
	return &instance;
}

void ShaderManager::Initialize() {
	HRESULT result = S_FALSE;

	// dxcCompilerを初期化
	dxcUtils_ = nullptr;
	dxcCompiler_ = nullptr;
	result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(result));
	result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(result));

	// 現時点でincludeはしないが、includeに対応するための設定を行っておく
	includeHandler_ = nullptr;
	result = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(result));

	///
	///	各シェーダーのコンパイル
	/// 
	
	// Object3D
	LoadShader("Object3D_VS", L"resources/Shaders/Object3D.VS.hlsl", L"vs_6_0");
	LoadShader("Object3D_PS", L"resources/Shaders/Object3D.PS.hlsl", L"ps_6_0");

	// Particle
	LoadShader("Particle_VS", L"resources/Shaders/Particle.VS.hlsl", L"vs_6_0");
	LoadShader("Particle_PS", L"resources/Shaders/Particle.PS.hlsl", L"ps_6_0");

	// SobelFilter
	LoadShader("SobelFilter_VS", L"resources/Shaders/SobelFilter.VS.hlsl", L"vs_6_0");
	LoadShader("SobelFilter_PS", L"resources/Shaders/SobelFilter.PS.hlsl", L"ps_6_0");

	// Grayscale
	LoadShader("Grayscale_VS", L"resources/Shaders/Grayscale.VS.hlsl", L"vs_6_0");
	LoadShader("Grayscale_PS", L"resources/Shaders/Grayscale.PS.hlsl", L"ps_6_0");

	// Vignette
	LoadShader("Vignette_VS", L"resources/Shaders/Vignette.VS.hlsl", L"vs_6_0");
	LoadShader("Vignette_PS", L"resources/Shaders/Vignette.PS.hlsl", L"ps_6_0");

	// BoxFilter
	LoadShader("BoxFilter_VS", L"resources/Shaders/BoxFilter.VS.hlsl", L"vs_6_0");
	LoadShader("BoxFilter_PS", L"resources/Shaders/BoxFilter.PS.hlsl", L"ps_6_0");

	// GaussianFilter
	LoadShader("GaussianFilter_VS", L"resources/Shaders/GaussianFilter.VS.hlsl", L"vs_6_0");
	LoadShader("GaussianFilter_PS", L"resources/Shaders/GaussianFilter.PS.hlsl", L"ps_6_0");

	// RadialBlur
	LoadShader("RadialBlur_VS", L"resources/Shaders/RadialBlur.VS.hlsl", L"vs_6_0");
	LoadShader("RadialBlur_PS", L"resources/Shaders/RadialBlur.PS.hlsl", L"ps_6_0");

	// Skybox
	LoadShader("Skybox_VS", L"resources/Shaders/Skybox.VS.hlsl", L"vs_6_0");
	LoadShader("Skybox_PS", L"resources/Shaders/Skybox.PS.hlsl", L"ps_6_0");

	// Skinning
	LoadShader("SkinningObject3D_VS", L"resources/Shaders/SkinningObject3D.VS.hlsl", L"vs_6_0");

	// InstancedObject
	LoadShader("InstancedObject_VS", L"resources/Shaders/InstancedObject.VS.hlsl", L"vs_6_0");
	LoadShader("InstancedObject_PS", L"resources/Shaders/InstancedObject.PS.hlsl", L"ps_6_0");
}

void ShaderManager::LoadShader(const std::string& name, const std::wstring& path, const wchar_t* profile) { 
	// シェーダーのコンパイル
	IDxcBlob* blob = CompileShader(path, profile); 
	// コンパイルしたシェーダーを登録
	shaders_[name] = blob;
}

IDxcBlob* ShaderManager::GetShader(const std::string& name) const { 
	auto it = shaders_.find(name); 
	if (it != shaders_.end()) {
		return it->second.Get();
	}
	return nullptr;
}

IDxcBlob* ShaderManager::CompileShader(const std::wstring& filePath, const wchar_t* profile) { 
	HRESULT result = S_FALSE;

	// これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompilerShader, path:{}, profile:{}\n", filePath, profile)));

	// 1. hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	result = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	// 読めなかったら止める
	assert(SUCCEEDED(result));
	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8; // UTF8の文字コードであることを通知

	// 2. Comileする
	LPCWSTR arguments[] = {
	    filePath.c_str(), // コンパイル対象のhlslファイル名
	    L"-E",
	    L"main", // エントリーポイントの指定
	    L"-T",
	    profile, // ShaderProfileの設定
	    L"-Zi",
	    L"-Qembed_debug", // デバッグ用の情報を埋め込む
	    L"-Od",           // 最適化を外しておく
	    L"-Zpr",          // メモリレイアウトは行優先
	};
	// 実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	result = dxcCompiler_->Compile(
	    &shaderSourceBuffer,        // 読み込んだファイル
	    arguments,                  // コンパイルオプション
	    _countof(arguments),        // コンパイルオプションの数
	    includeHandler_.Get(),      // includeが含まれた諸々
	    IID_PPV_ARGS(&shaderResult) // コンパイル結果
	);
	// コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(result));

	// 3. 警告・エラーがでていないか確認する
	// 警告・エラーが出てたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		assert(false);
	}

	// 4. Compile結果を受け取って返す
	// コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	result = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(result));
	// 成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));
	// もう使わないリソースを開放
	shaderSource->Release();
	shaderResult->Release();
	// 実行用のバイナリを返却
	return shaderBlob;
}

#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <DirectXBase.h>
#include <DirectXUtil.h>

// =========================================================
// 定数バッファのラッパークラス
// =========================================================
template<class Type> class ConstBuffer {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="isEmpty">trueを指定した場合には空で生成</param>
	ConstBuffer(bool isEmpty = false) {
		if (!isEmpty)
			Create();
	};

	/// <summary>
	/// リソースを作成します。
	/// </summary>
	void Create() {
		// リソースを作る
		resource_ = CreateBufferResource(DirectXBase::GetInstance()->GetDevice(), sizeof(Type));
		// データを書き込む
		data_ = nullptr;
		// 書き込むためのアドレスを取得
		resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
	};

	/// <summary>
	/// コピー不可にする
	/// </summary>
	ConstBuffer(const ConstBuffer&) = delete;
	ConstBuffer(ConstBuffer&&) = delete;
	ConstBuffer& operator=(const ConstBuffer&) = delete;
	ConstBuffer& operator=(ConstBuffer&&) = delete;

	// =========================================================
	// Member Variables
	// =========================================================

	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;		/* 定数バッファリソース */
	Type* data_;											/* 定数バッファデータ ポインタ */
};

#pragma once
#include "DirectXUtil.h"
#include "DirectXBase.h"

template<class Type>class ConstBuffer {
public:
	// trueを指定した場合には空で生成される
	ConstBuffer(bool isEmpty = false) { if (!isEmpty)Create(); };

	void Create() {
		// リソースを作る
		resource_ = CreateBufferResource(DirectXBase::GetInstance()->GetDevice(), sizeof(Type));
		// データを書き込む
		data_ = nullptr;
		// 書き込むためのアドレスを取得
		resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
	};

	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	Type* data_;

	// コピー不可にする
	ConstBuffer(const ConstBuffer&) = delete;
	ConstBuffer(ConstBuffer&&) = delete;
	ConstBuffer& operator=(const ConstBuffer&) = delete;
	ConstBuffer& operator=(ConstBuffer&&) = delete;
};

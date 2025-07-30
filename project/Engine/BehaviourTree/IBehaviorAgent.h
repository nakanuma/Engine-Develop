#pragma once

/// <summary>
/// 汎用的な対象オブジェクト用インターフェース
/// </summary>
class IBehaviorAgent {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IBehaviorAgent() = default;

	/// <summary>
	/// 自身のデータを取得する用
	/// </summary>
	/// <returns></returns>
	virtual void* GetUserData() = 0;
};

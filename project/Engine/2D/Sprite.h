#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <DirectXBase.h>
#include <MyMath.h>

// ---------------------------------------------------------
// Foward Declaration
// ---------------------------------------------------------
class SpriteCommon;

// =========================================================
// 1つのスプライトを表すクラス
// =========================================================
class Sprite {
public:
	/// <summary>
	/// 頂点情報を格納する構造体
	/// </summary>
	struct VertexData {
		Float4 position;		/* 頂点位置 */
		Float2 texcoord;		/* テクスチャ座標 */
		Float3 normal;			/* 法線ベクトル */
	};

	/// <summary>
	/// マテリアルを表す構造体
	/// </summary>
	struct Material {
		Float4 color;					/* 色 */
		int32_t enableLighting;			/* ライティング有効化フラグ */
		float shininess;				/* 光沢 */
		float ratio;					/* マスクの割合 */
		float padding;					/* パディング */
		Matrix uvTransform;				/* UV変換行列 */
		int32_t useCircleMask;			/* 円形マスク使用フラグ */
		Float3 padding2;				/* パディング */
	};

	/// <summary>
	/// シェーダーに渡す変換行列をまとめた構造体
	/// </summary>
	struct TransformationMatrix {
		Matrix WVP;							/* ワールドビュープロジェクション行列 */
		Matrix World;						/* ワールド行列 */
		Matrix WorldInverseTranspose;		/* ワールド逆転置行列 */
	};

public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// スプライトの初期化処理を行います。
	/// </summary>
	/// <param name="spriteCommon">スプライト共通処理クラス</param>
	/// <param name="textureIndex">テクスチャインデックス</param>
	void Initialize(SpriteCommon* spriteCommon, uint32_t textureIndex);

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// スプライトの描画処理を行います。
	/// </summary>
	void Draw();

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// スプライトの位置を取得します。
	/// </summary>
	/// <returns>現在位置（Float2）</returns>
	const Float2 GetPosition() const { return position_; }

	/// <summary>
	/// スプライトの位置を設定します。
	/// </summary>
	/// <param name="position">新しい位置（Float2）</param>
	void SetPosition(const Float2& position) { this->position_ = position; }

	/// <summary>
	/// スプライトの回転を取得します。
	/// </summary>
	/// <returns>現在の回転角（radian）</returns>
	float GetRotation() const { return rotation; }

	/// <summary>
	/// スプライトの回転を設定します。
	/// </summary>
	/// <param name="rotation">新しい回転角（radian）</param>
	void SetRotation(float rotation) { this->rotation = rotation; }

	/// <summary>
	/// スプライトの色を取得します。
	/// </summary>
	/// <returns>現在の色（Float4）</returns>
	const Float4& GetColor() const { return materialData_->color; }

	/// <summary>
	/// スプライトの色を設定します。
	/// </summary>
	/// <param name="color">新しい色（Float4）</param>
	void SetColor(const Float4& color) { materialData_->color = color; }

	/// <summary>
	/// スプライトのサイズを取得します。
	/// </summary>
	/// <returns>現在のサイズ（Float2）</returns>
	const Float2& GetSize() const { return size_; }

	/// <summary>
	/// スプライトのサイズを設定します。
	/// </summary>
	/// <param name="size">新しいサイズ（Float2）</param>
	void SetSize(const Float2& size) { this->size_ = size; }

	/// <summary>
	/// スプライトのアンカーポイントを取得します。
	/// </summary>
	/// <returns>現在のアンカーポイント（Float2）</returns>
	const Float2& GetAnchorPoint() const { return anchorPoint; }

	/// <summary>
	/// スプライトのアンカーポイントを設定します。
	/// </summary>
	/// <param name="anchorPoint">新しいアンカーポイント（Float2）</param>
	void SetAnchorPoint(const Float2& anchorPoint) { this->anchorPoint = anchorPoint; }

	/// <summary>
	/// スプライトの左右フリップ状態を取得します。
	/// </summary>
	/// <returns>左右フリップ状態（true: フリップ中, false: 通常）</returns>
	bool IsFlipX() const { return isFlipX_; }

	/// <summary>
	/// スプライトの左右フリップ状態を設定します。
	/// </summary>
	/// <param name="flipX">新しい左右フリップ状態（true: フリップ中, false: 通常）</param>
	void SetFlipX(bool flipX) { isFlipX_ = flipX; }

	/// <summary>
	/// スプライトの上下フリップ状態を取得します。
	/// </summary>
	/// <returns>上下フリップ状態（true: フリップ中, false: 通常）</returns>
	bool IsFlipY() const { return isFlipY_; }

	/// <summary>
	/// スプライトの上下フリップ状態を設定します。
	/// </summary>
	/// <param name="flipY">新しい上下フリップ状態（true: フリップ中, false: 通常）</param>
	void SetFlipY(bool flipY) { isFlipY_ = flipY; }

	/// <summary>
	/// スプライトのテクスチャ左上座標を取得します。
	/// </summary>
	/// <returns>現在のテクスチャ左上座標（Float2）</returns>
	const Float2& GetTextureLeftTop() const { return textureLeftTop_; }

	/// <summary>
	/// スプライトのテクスチャ左上座標を設定します。
	/// </summary>
	/// <param name="textureLeftTop">新しいテクスチャ左上座標（Float2）</param>
	void SetTextureLeftTop(const Float2& textureLeftTop) { this->textureLeftTop_ = textureLeftTop; }

	/// <summary>
	/// スプライトのテクスチャサイズを取得します。
	/// </summary>
	/// <returns>現在のテクスチャサイズ（Float2）</returns>
	const Float2& GetTextureSize() const { return textureSize_; }

	/// <summary>
	/// スプライトのテクスチャサイズを設定します。
	/// </summary>
	/// <param name="textureSize">新しいテクスチャサイズ（Float2）</param>
	void SetTextureSize(const Float2& textureSize) { this->textureSize_ = textureSize; }

	Material* materialData_ = nullptr;				/* マテリアルデータ */

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// テクスチャサイズをイメージに合わせます。
	/// </summary>
	void AdjustTextureSize();

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// ----- System -----
	SpriteCommon* spriteCommon = nullptr;										/* スプライト共通処理クラス */

	// ----- Resources -----
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;						/* 頂点バッファ */
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;						/* インデックスバッファ */
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;					/* マテリアルバッファ */
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_;		/* 変換行列バッファ */

	VertexData* vertexData_ = nullptr;											/* 頂点データ */
	uint32_t* indexData_ = nullptr;												/* インデックスデータ */
	TransformationMatrix* transformationMatrixData_ = nullptr;					/* 変換行列データ */

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;									/* 頂点バッファビュー */
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;									/* インデックスバッファビュー */

	// ----- Parameters -----
	uint32_t textureIndex_;														/* テクスチャインデックス */

	Transform transform_;														/* 変換行列 */
	Float2 position_ = { 0.0f, 0.0f };											/* 位置 */
	float rotation = 0.0f;														/* 回転角 */

	Float2 size_ = { 640.0f, 360.0f };											/* スプライトのサイズ */
	Float2 anchorPoint = { 0.0f, 0.0f };										/* アンカーポイント */

	bool isFlipX_ = false;														/* 左右フリップ */
	bool isFlipY_ = false;														/* 上下フリップ */

	Float2 textureLeftTop_ = { 0.0f, 0.0f };									/* テクスチャ左上座標 */
	Float2 textureSize_ = { 100.0f, 100.0f };									/* テクスチャサイズ */
};

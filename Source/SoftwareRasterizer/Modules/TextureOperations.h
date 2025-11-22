#pragma once

#include "..\State\Texture2D.h"
#include "..\Core\Types.h"

namespace SoftwareRasterizer
{

	class TextureOperations
	{

	public:

		static void validate(const Texture2D* texture);

		static void FillTextureColor(Texture2D* texture, const Vector4& color);
		static void FillTextureDepth(Texture2D* texture, float depth);

		static Vector4 FetchTexelColor(const Texture2D* texture, int tx, int ty);
		static float FetchTexelDepth(const Texture2D* texture, int tx, int ty);

		static void StoreTexelColor(Texture2D* texture, int tx, int ty, const Vector4& color);
		static void StoreTexelDepth(Texture2D* texture, int tx, int ty, float depth);

	};

}

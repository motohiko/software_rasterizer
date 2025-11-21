#pragma once

#include "..\State\Texture2D.h"
#include "..\Types.h"

namespace SoftwareRasterizer
{

	class TextureOperations
	{

	public:

		static void validate(const Texture2D* texture);

		static void fillColor(Texture2D* texture, const Vector4& color);
		static void fillDepth(Texture2D* texture, float depth);

		static Vector4 fetchTexelColor(const Texture2D* texture, int tx, int ty);
		static float fetchTexelDepth(const Texture2D* texture, int tx, int ty);

		static void storeTexelColor(Texture2D* texture, int tx, int ty, const Vector4& color);
		static void storeTexelDepth(Texture2D* texture, int tx, int ty, float depth);

	};

}

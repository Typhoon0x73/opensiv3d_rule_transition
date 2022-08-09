# include <Siv3D.hpp> // OpenSiv3D v0.6.4

//#define CUSTOM_SELECT_TEXTURE
SIV3D_SET(EngineOption::Renderer::OpenGL); // Windows で Direct3D の代わりに OpenGL を使用

const Array<String> custom_texture_paths {
	U"Resources/rule800/11.png",
	U"Resources/rule800/39.png",
	U"Resources/rule800/40.png",
	U"Resources/rule800/54.png",
	U"Resources/rule800/97.png",
	U"Resources/rule800/115.png",
	U"Resources/rule800/150.png",
	U"Resources/rule800/224.jpg",
	U"Resources/rule800/226.jpg",
};

#ifdef CUSTOM_SELECT_TEXTURE
const uint32_t texture_count = custom_texture_paths.size();
#else
const uint32_t texture_count = 233;
#endif//CUSTOM_SELECT_TEXTURE

void LoadTextures(Array<Texture>* v1, ListBoxState* v2)
{
	assert(v1);
	assert(v2);
	v1->reserve(texture_count);
#ifdef CUSTOM_SELECT_TEXTURE
	for (const auto& path : custom_texture_paths)
	{
		v2->items.push_back(FileSystem::FileName(path));
		v1->emplace_back(Texture{ path });
	}
#else
	for (int i = 0; i < texture_count; i++)
	{
		v2->items.push_back(U"{0}.{1}"_fmt(i + 1, (i > 175 ? U"jpg" : U"png")));
		v1->emplace_back(Texture{ U"Resources/rule800/{0}"_fmt(v2->items[i]) });
	}
#endif // CUSTOM_SELECT_TEXTURE
}

// シェーダー側と合わせたバッファ構造体
struct RuleFade
{
	float  softRange;
	float  fadeTime;
};

void Main()
{
	// シェーダーの読み込み
	PixelShader ps = HLSL{ U"Resources/rule_fade.hlsl", U"PS" }
	| GLSL{ U"Resources/rule_fade.frag", { { U"PSConstants2D", 0 }, { U"RuleFade", 1 } } };

	if (not ps)
	{
		throw Error{ U"Failed to load a shader file" };
	}

	// コンスタントバッファ
	ConstantBuffer<RuleFade> cb{ {0.02f, 0.0f} };

	// いつもの
	const Texture texture{ U"example/windmill.png" };


	// 画像読み込みと切り替え用リストアイテム作成
	Array<Texture> masks;
	ListBoxState lbs;
	LoadTextures(&masks, &lbs);
	int select = 0;
	lbs.selectedItemIndex = select;

	while (System::Update())
	{
		// 各パラメータ設定
		double value = cb->fadeTime;
		if (SimpleGUI::Slider(U"fadeTime:{0}"_fmt(cb->fadeTime), value, Vec2{ 0, 350 }, 300))
		{
			cb->fadeTime = static_cast<float>(value);
		}
		value = cb->softRange;
		if (SimpleGUI::Slider(U"softRange:{0}"_fmt(cb->softRange), value, Vec2{ 0, 400 }, 300))
		{
			cb->softRange = static_cast<float>(value);
		}
		if (SimpleGUI::ListBox(lbs, Vec2{ 600, 10 }, 160, 580))
		{
			select = static_cast<int>(lbs.selectedItemIndex.value());
		}

		// マスクトランジション
		{
			Graphics2D::SetPSTexture(1, masks[select]);
			Graphics2D::SetPSConstantBuffer(1, cb);
			const ScopedCustomShader2D shader{ ps };
			texture.draw(0, 0);
		}
	}
}

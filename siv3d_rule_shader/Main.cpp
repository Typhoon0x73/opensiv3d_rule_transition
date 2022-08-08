# include <Siv3D.hpp> // OpenSiv3D v0.6.4

// シェーダー側と合わせたバッファ構造体
struct RuleFade
{
	float  softRange;
	float  fadeTime;
};

void Main()
{
	// シェーダーの読み込み
	PixelShader ps = HLSL{ U"Resources/rule_fade.hlsl" };
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
	masks.reserve(233);
	for (int i = 0; i < 233; i++)
	{
		lbs.items.push_back(U"{0}.{1}"_fmt(i + 1, (i > 175 ? U"jpg" : U"png")));
		masks.emplace_back(Texture{ U"Resources/rule800/{0}"_fmt(lbs.items[i])});
	}
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

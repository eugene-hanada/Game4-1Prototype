#include <DxLib.h>
#include <list>
#include <memory>
#include <algorithm>
#include <cmath>

class Character;

using CharaList = std::list<std::unique_ptr<Character>>;

enum class CharType
{
	Player,
	Enemy,
	Bullet,
};

// キャラのベース
class Character
{
public:
	Character(bool isEnemy, float posX, float posY) :
		posX_{posX}, posY_{posY}, isAlive_{true}
	{
		if (isEnemy)
		{
			color_ = 0xff0000;
		}
		else
		{
			color_ = 0x0000ff;
		}
	}

	// 更新
	virtual void Update(CharaList& list) = 0;

	// 描画
	virtual void Draw(void)
	{
		DrawCircle(static_cast<int>(posX_), static_cast<int>(posY_), static_cast<int>(r_), color_);
	}

	virtual CharType GetCharType(void) = 0;

	float GetX(void) const
	{
		return posX_;
	}

	float GetY(void) const
	{
		return posY_;
	}

	virtual void OnHit(CharType type) = 0;
protected:

	// X座標
	float posX_;

	// Y座標
	float posY_;

	// 色
	int color_;
	
	float r_ = 20.0f;

	bool isAlive_;
};

class Bullet :
	public Character
{
public:
	Bullet(float x, float y, float rot) :
		Character{ false, x,y }, rot_{ rot }
	{
		r_ = 7.0f;
		color_ = 0xffffff;
	}

	void Update(CharaList& list) final
	{
		posX_ += std::cos(rot_) * 5.0f;
		posY_ += std::sin(rot_) * 5.0f;
	}
private:
	CharType GetCharType(void)
	{
		return CharType::Bullet;
	}

	void OnHit(CharType type) final
	{
		if (type == CharType::Enemy)
		{
			isAlive_ = false;
		}
	}
	float rot_;
};

// プレイヤー
class Player :
	public Character
{
public:
	Player() :
		Character{false,640.0f, 360.0f}, rot_{0.0f}, trigger_{false}
	{

	}
private:

	void  Update(CharaList& list) final
	{
		// 移動
		if (CheckHitKey(KEY_INPUT_W))
		{
			posY_ -= 2.0f;
		}
		if (CheckHitKey(KEY_INPUT_S))
		{
			posY_ += 2.0f;
		}
		if (CheckHitKey(KEY_INPUT_A))
		{
			posX_ -= 2.0f;
		}
		if (CheckHitKey(KEY_INPUT_D))
		{
			posX_ += 2.0f;
		}

		// 回転
		int x, y = 0;
		GetMousePoint(&x, &y);
		float tmpX = 640.0f - x;
		float tmpY = 360.0f - y;
		rot_ = std::atan2(tmpY, tmpX) + DX_PI;

		if ((GetMouseInput() & MOUSE_INPUT_LEFT))
		{
			if (!trigger_)
			{
				trigger_ = true;
				list.emplace_back(std::make_unique<Bullet>(posX_, posY_, rot_));
			}
		}
		else
		{
			trigger_ = false;
		}

	}

	// 描画
	void Draw(void) final
	{
		Character::Draw();
		DrawLine(
			static_cast<int>(posX_), static_cast<int>(posY_),
			static_cast<int>(posX_ + std::cos(rot_) * 20.0f), static_cast<int>(posY_ + std::sin(rot_) * 20.0f),
			0xffffff, 5
		);
	}

	CharType GetCharType(void) final
	{
		return CharType::Player;
	}

	void OnHit(CharType type) final
	{
		if (type == CharType::Enemy)
		{
			isAlive_ = false;
		}
	}

	// 回転角度
	float rot_;
	bool trigger_;
};

// 敵
class Enemy :
	public Character
{
public:
	Enemy(float x, float y) :
		Character{ true,x,y }
	{

	}

	
private:

	void Update(CharaList& list) final
	{
		// プレイヤー見つける
		const auto& player = std::find_if(list.begin(), list.end(), [](std::unique_ptr<Character>& c) {return c->GetCharType() == CharType::Player; });

		// 距離調べる
		float x = (*player)->GetX();
		float y = (*player)->GetY();
		float tmpX = x - posX_;
		float tmpY = y - posY_;
		float length = std::pow(tmpX, 2) + std::pow(tmpY, 2);
		
		if (length <= std::pow(200.0f,2))
		{
			// 一定の距離以下なら追尾する
			float r = std::atan2(tmpY, tmpX);
			posX_ += std::cos(r) * 1.5f;
			posY_ += std::sin(r) * 1.5f;
		}
	}

	CharType GetCharType(void) final
	{
		return CharType::Enemy;
	}

	void OnHit(CharType type) final
	{
		if (type == CharType::Bullet)
		{
			isAlive_ = false;
		}
	}
};



int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int mCmdShow)
{
	ChangeWindowMode(true);
	SetGraphMode(1280, 720, 32);
	DxLib_Init();


	CharaList list;

	list.emplace_back(std::make_unique<Player>());
	list.emplace_back(std::make_unique<Enemy>(100.0f, 100.0f));
	while (!ProcessMessage())
	{
		std::for_each(list.begin(), list.end(), [&list](std::unique_ptr<Character>& c) {c->Update(list); });
		SetDrawScreen(DX_SCREEN_BACK);
		ClsDrawScreen();
		std::for_each(list.begin(), list.end(), [&list](std::unique_ptr<Character>& c) {c->Draw(); });
		ScreenFlip();
	}
	DxLib_End();
}
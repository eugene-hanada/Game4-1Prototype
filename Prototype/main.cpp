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
};

// �L�����̃x�[�X
class Character
{
public:
	Character(bool isEnemy, float posX, float posY) :
		posX_{posX}, posY_{posY}
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

	// �X�V
	virtual void Update(CharaList& list) = 0;

	// �`��
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
protected:

	// X���W
	float posX_;

	// Y���W
	float posY_;
private:

	// �F
	int color_;
	
	static constexpr float r_ = 20.0f;
};

// �v���C���[
class Player :
	public Character
{
public:
	Player() :
		Character{false,640.0f, 360.0f}, rot_{0.0f}
	{

	}
private:

	void  Update(CharaList& list) final
	{
		// �ړ�
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

		// ��]
		int x, y = 0;
		GetMousePoint(&x, &y);
		float tmpX = 640.0f - x;
		float tmpY = 360.0f - y;
		rot_ = std::atan2(tmpY, tmpX) + DX_PI;


	}

	// �`��
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

	// ��]�p�x
	float rot_;
};


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
		// �v���C���[������
		const auto& player = std::find_if(list.begin(), list.end(), [](std::unique_ptr<Character>& c) {return c->GetCharType() == CharType::Enemy; });

		// �������ׂ�
		float x = (*player)->GetX();
		float y = (*player)->GetY();
		float length = std::pow(posX_ - x, 2) + std::pow(posY_ - y, 2);
		
		if (length <= 70.0f)
		{
			// ���̋����ȉ��Ȃ�ǔ�����

		}
	}

	CharType GetCharType(void) final
	{
		return CharType::Enemy;
	}
};

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int mCmdShow)
{
	ChangeWindowMode(true);
	SetGraphMode(1280, 720, 32);
	DxLib_Init();


	CharaList list;

	list.emplace_back(std::make_unique<Player>());

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
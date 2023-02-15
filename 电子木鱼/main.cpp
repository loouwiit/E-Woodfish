#include <iostream>
#include <fstream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
//#include "windows.cpp"

/*
* 1.0.0.1 ���Ű汾
* 1.0.0.2 �Ż�ľ��������߼�
* 1.0.0.3 ��������������6
* 1.0.0.4 ����Զ�ģʽ ��ȡloop�е�key��knock
* 1.0.0.5 ��Ӵ���֧�� �Ż������߼�������CPUռ�� 2023/2/11
* 1.0.0.6 ȷ�ϴ���֧����Ч �޸���������ʧ�ܺ����¼��ش����bug
*/

#ifdef _DEBUG
#define MAIN main()
#else
#ifdef WIN32
#include <wtypes.h>
#define MAIN __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
#define MAIN WinMain()
#endif
#endif
#define max(A,B) ((A<B)?B:A)
#define min(A,B) ((A>B)?B:A)

constexpr unsigned origenal_Size[2] = { 1920 / 2, 1080 / 2 };
sf::RenderWindow window;
sf::Event event;
sf::View view;

sf::Clock sfClock;
constexpr char SPF = 1000 / 60; //62.5FPS

sf::SoundBuffer sound_Buffer;
constexpr char sound_Number = 6;
sf::Sound sound[sound_Number];
char now_Sound_Number;

sf::Texture texture;
sf::Sprite fish;
float fish_Scale = 1;

sf::Time auto_Knock_Time = sf::milliseconds(1000);
sf::Time next_Knock_Time = -auto_Knock_Time;
bool auto_Knock = false;

bool fream_Knock = false;
char fream_Active = 0;
constexpr sf::Keyboard::Key fream_Keys[13] = { sf::Keyboard::Up,sf::Keyboard::Up,sf::Keyboard::Down,sf::Keyboard::Down,
	sf::Keyboard::Left,sf::Keyboard::Right,sf::Keyboard::Left,sf::Keyboard::Right,
	sf::Keyboard::B,sf::Keyboard::A,sf::Keyboard::B,sf::Keyboard::A,sf::Keyboard::Unknown };

sf::Font font;
constexpr char text_Buffer_Lenght = 30;
constexpr char text_Number = 80 + 1;
wchar_t text_Buffer[text_Buffer_Lenght] = L"";
sf::Text text[text_Number];
char now_Text_Number;

unsigned long long merits = 0; //����
//constexpr int File_Version = 1;

template <typename T>
inline sf::Vector2<T> operator/(sf::Vector2<T> vec, int num);

void initlize();
void loop();
void enedlize();

void key(sf::Event::KeyEvent event_Key);
void knock(sf::Vector2f position);

int MAIN
{
	initlize();
	while (window.isOpen())
		loop();
	enedlize();

	return 0;
}

void initlize()
{
	std::ifstream file;
	int file_Version = 0;
	file.open("./����ľ��/����.���±�", std::ios::in | std::ios::binary);
	file.read((char*)&merits, sizeof(merits));
	file.close();

	sound_Buffer.loadFromFile("./����ľ��/����ľ��.ogg");
	for (char i = 0; i < sound_Number; i++) sound[i].setBuffer(sound_Buffer);

	texture.loadFromFile("./����ľ��/����ľ��.png");
	fish.setTexture(texture);
	fish.setOrigin((sf::Vector2f)texture.getSize() / 2);
	fish.setPosition(0, 0);

	if (!font.loadFromFile("C:/windows/fonts/msyh.ttc")) font.loadFromFile("C:/windows/fonts/msyh.ttf");
	swprintf_s(text_Buffer, text_Buffer_Lenght, L"��ǰ����:%llu", merits);

	for (char i = 1; i < text_Number; i++)
	{
		text[i].setString(sf::String(L"����+1"));
		text[i].setFont(font);
		text[i].setFillColor(sf::Color(0xFFFFFF00));
		text[i].setOrigin(0, 20);
	}
	text[0].setString(sf::String(text_Buffer));
	text[0].setFont(font);
	text[0].setFillColor(sf::Color(0xFFFFFFFF));


	window.create(sf::VideoMode(origenal_Size[0], origenal_Size[1]), L"����ľ��");
	window.setIcon(texture.getSize().x, texture.getSize().y, texture.copyToImage().getPixelsPtr());
	view.reset(sf::FloatRect(-(sf::Vector2f)window.getSize() / 2, (sf::Vector2f)window.getSize()));
	window.setView(view);
	text[0].setPosition(-(sf::Vector2f)window.getSize() / 2 + sf::Vector2f(20, 20));

	//Register_Touch(window.getSystemHandle());
}

void loop()
{
	//�¼�����
	while (window.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::Closed:
			window.close();
			break;
		case sf::Event::Resized:
			view.setSize((float)event.size.width, (float)event.size.height);
			//fish_Scale = min((float)event.size.width / (float)origenal_Size[0], (float)event.size.height / (float)origenal_Size[1]); //1.0.0.1
			fish_Scale = min((float)event.size.width / (float)texture.getSize().x, (float)event.size.height / (float)texture.getSize().y); //1.0.0.2�Ż�
			fish.setScale(fish_Scale,fish_Scale);
			text[0].setPosition(-(float)event.size.width / 2+20, -(float)event.size.height / 2+20);
			window.setView(view);
			break;
		case sf::Event::KeyPressed:
			key(event.key);
			break;
		case sf::Event::MouseButtonPressed:
			if (fream_Knock) fream_Knock = false, fream_Active = 0;
			knock(window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y }));
			break;
		case sf::Event::TouchBegan:
			if (fream_Knock) fream_Knock = false, fream_Active = 0;
			knock(window.mapPixelToCoords({ event.touch.x,event.touch.y }));
			break;
		default:
			break;
		}
	}

	//�Զ�ˢ��
	if (fream_Knock)
	{
		//����+1
		knock({ fish.getPosition().x + fish.getOrigin().x * fish_Scale * 0.7f,
			   fish.getPosition().y + fish.getOrigin().y * fish_Scale * -0.5f });
	}
	else if (auto_Knock && sfClock.getElapsedTime() > next_Knock_Time)
	{
		next_Knock_Time = sfClock.getElapsedTime() + auto_Knock_Time;

		//����+1
		knock({ fish.getPosition().x + fish.getOrigin().x * fish_Scale * 0.7f,
			fish.getPosition().y + fish.getOrigin().y * fish_Scale * -0.5f });
	}

	//״̬����
	if (fish.getScale().x < fish_Scale)
		fish.setScale(fish.getScale().x + 0.01f, fish.getScale().x + 0.01f);
	for (char i = 1; i < text_Number; i++)
	{
		if (text[i].getFillColor().a > 0)
		{
			//��ʾ��
			text[i].setFillColor(sf::Color(0xFF, 0xFF, 0xFF, max(text[i].getFillColor().a - 5, 0)));
			text[i].move(0, -3.f);
		}
	}

	//��Ⱦ
	window.clear(sf::Color(0x000000FF));
	window.draw(fish);
	for (char i = 0; i < text_Number; i++)
		if (text[i].getFillColor().a > 0)
			window.draw(text[i]);
	window.display();

	sf::sleep(sf::milliseconds(SPF)); //����˯�ߣ��պ���˵
}

void enedlize()
{
	std::fstream file;
	unsigned long long file_Merits = 0;
	int file_Version = 0;
	file.open("./����ľ��/����.���±�", std::ios::in | std::ios::out | std::ios::binary);
	if (!file.is_open())
	{
		std::ofstream temp;
		temp.open("./����ľ��/����.���±�");
		for (char i = 0; i < 8; i++) temp.write("", 1); //��ֹ������ȡ����
		temp.close();
		file.open("./����ľ��/����.���±�", std::ios::in | std::ios::out | std::ios::binary);
	}
	file.read((char*)&file_Merits, sizeof(file_Merits));
	if (merits > file_Merits) //�����˻򹦵±ȹ��±��Ķ�
	{
		file.seekp(-file.gcount(), std::ios::cur);//���˻�ȥ
		file.write((char*)&merits, sizeof(merits));
	}
	file.close();
	return;
}

void key(sf::Event::KeyEvent event_Key)
{
	switch (event_Key.code)
	{
	case sf::Keyboard::Escape:
		//����ESC���˳�
		window.close();
		break;
	case sf::Keyboard::A:
		//A:�Զ�
		auto_Knock = !auto_Knock;
		break;
	default:
		//����+1
		knock({ fish.getPosition().x + fish.getOrigin().x * fish_Scale * 0.7f,
			fish.getPosition().y + fish.getOrigin().y * fish_Scale * -0.5f });
		break;
	}
	if (fream_Knock == true) fream_Knock = false, fream_Active = 0;
	else if (event.key.code == fream_Keys[fream_Active])
	{
		//�ж�fream_Knock
		fream_Active++;
		if (fream_Keys[fream_Active] == sf::Keyboard::Unknown) fream_Knock = true;
	}
	else fream_Active = event.key.code == fream_Keys[0] ? 1 : 0;
}

void knock(sf::Vector2f position)
{
	merits++;
	fish.setScale(0.95f * fish_Scale, 0.95f * fish_Scale);
	now_Sound_Number++, now_Sound_Number %= sound_Number;
	sound[now_Sound_Number].play();

	now_Text_Number = (now_Text_Number + 1) % (text_Number - 1) + 1;
	text[now_Text_Number].setPosition(position);
	text[now_Text_Number].setFillColor(sf::Color(0xFFFFFFFF));

	swprintf_s(text_Buffer, text_Buffer_Lenght, L"��ǰ����:%llu", merits);
	text[0].setString(sf::String(text_Buffer));
}

template<typename T>
sf::Vector2<T> operator/(sf::Vector2<T> vec, int num)
{
	return sf::Vector2<T>(vec.x / num, vec.y / num);
}

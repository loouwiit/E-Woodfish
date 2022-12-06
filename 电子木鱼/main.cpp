#include <iostream>
#include <fstream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

/*
* 1.0.0.1 初号版本
* 1.0.0.2 优化木鱼的缩放逻辑
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

sf::SoundBuffer sound_Buffer;
constexpr char sound_Number = 4; //虽说两个足矣，但是为了不浪费这个char我要浪费更多的Sound
sf::Sound sound[sound_Number];
char now_Sound_Number;

sf::Texture texture;
sf::Sprite fish;
float fish_Scale = 1;

sf::Font font;
constexpr char text_Buffer_Lenght = 30;
constexpr char text_Number = 80 + 1;
wchar_t text_Buffer[text_Buffer_Lenght] = L"";
sf::Text text[text_Number];
char now_Text_Number;

unsigned long long merits = 0; //功德
//constexpr int File_Version = 1;

template <typename T>
inline sf::Vector2<T> operator/(sf::Vector2<T> vec, int num);

void initlize();
void loop();
void enedlize();

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
	file.open("./电子木鱼/功德.功德本", std::ios::in | std::ios::binary);
	file.read((char*)&merits, sizeof(merits));
	file.close();

	sound_Buffer.loadFromFile("./电子木鱼/电子木鱼.ogg");
	for (char i = 0; i < sound_Number; i++) sound[i].setBuffer(sound_Buffer);

	texture.loadFromFile("./电子木鱼/电子木鱼.png");
	fish.setTexture(texture);
	fish.setOrigin((sf::Vector2f)texture.getSize() / 2);
	fish.setPosition(0, 0);

	if (!font.loadFromFile("C:/windows/fonts/msyh.ttc")) font.loadFromFile("C:/windos/fonts/msyh.ttf");
	swprintf_s(text_Buffer, text_Buffer_Lenght, L"当前功德:%llu", merits);

	for (char i = 1; i < text_Number; i++)
	{
		text[i].setString(sf::String(L"功德+1"));
		text[i].setFont(font);
		text[i].setFillColor(sf::Color(0xFFFFFF00));
		text[i].setOrigin(0, 20);
	}
	text[0].setString(sf::String(text_Buffer));
	text[0].setFont(font);
	text[0].setFillColor(sf::Color(0xFFFFFFFF));


	window.create(sf::VideoMode(origenal_Size[0], origenal_Size[1]), L"电子木鱼");
	window.setIcon(texture.getSize().x, texture.getSize().y, texture.copyToImage().getPixelsPtr());
	view.reset(sf::FloatRect(-(sf::Vector2f)window.getSize() / 2, (sf::Vector2f)window.getSize()));
	window.setView(view);
	text[0].setPosition(-(sf::Vector2f)window.getSize() / 2 + sf::Vector2f(20, 20));
}

void loop()
{
	//事件处理
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
			fish_Scale = min((float)event.size.width / (float)texture.getSize().x, (float)event.size.height / (float)texture.getSize().y); //1.0.0.2优化
			fish.setScale(fish_Scale,fish_Scale);
			text[0].setPosition(-(float)event.size.width / 2+20, -(float)event.size.height / 2+20);
			window.setView(view);
			break;
		case sf::Event::KeyPressed:
			if (event.key.code == sf::Keyboard::Escape)
			{
				//若是ESC则退出
				window.close();
				break;
			}
			//否则功德+1
		case sf::Event::MouseButtonPressed:
			merits++;
			fish.setScale(0.95f * fish_Scale, 0.95f * fish_Scale);
			now_Sound_Number++, now_Sound_Number %= sound_Number;
			sound[now_Sound_Number].play();

			now_Text_Number = (now_Text_Number + 1) % (text_Number - 1) + 1;
			if (event.type == sf::Event::MouseButtonPressed) text[now_Text_Number].setPosition(window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y }));
			else text[now_Text_Number].setPosition(fish.getPosition().x + fish.getOrigin().x * fish_Scale * 0.7f, fish.getPosition().y + fish.getOrigin().y * fish_Scale * -0.5f);
			text[now_Text_Number].setFillColor(sf::Color(0xFFFFFFFF));

			swprintf_s(text_Buffer, text_Buffer_Lenght, L"当前功德:%llu", merits);
			text[0].setString(sf::String(text_Buffer));
			break;
		default:
			break;
		}
	}

	//状态更新
	if (fish.getScale().x < fish_Scale)
		fish.setScale(fish.getScale().x + 0.01f, fish.getScale().x + 0.01f);
	for (char i = 1; i < text_Number; i++)
	{
		if (text[i].getFillColor().a > 0)
		{
			//显示中
			text[i].setFillColor(sf::Color(0xFF, 0xFF, 0xFF, max(text[i].getFillColor().a - 5, 0)));
			text[i].move(0, -3.f);
		}
	}

	//渲染
	window.clear(sf::Color(0x000000FF));
	window.draw(fish);
	for (char i = 0; i < text_Number; i++) window.draw(text[i]);
	window.display();

	sf::sleep(sf::milliseconds(1000 / 60)); //62.5 FPS
}

void enedlize()
{
	std::fstream file;
	unsigned long long file_Merits = 0;
	int file_Version = 0;
	file.open("./电子木鱼/功德.功德本", std::ios::in | std::ios::out | std::ios::binary);
	if (!file.is_open())
	{
		std::ofstream temp;
		temp.open("./电子木鱼/功德.功德本");
		for (char i = 0; i < 8; i++) temp.write("", 1); //防止后续读取出错
		temp.close();
		file.open("./电子木鱼/功德.功德本", std::ios::in | std::ios::out | std::ios::binary);
	}
	file.read((char*)&file_Merits, sizeof(file_Merits));
	if (merits > file_Merits) //读完了或功德比功德本的多
	{
		file.seekp(-file.gcount(), std::ios::cur);//倒退回去
		file.write((char*)&merits, sizeof(merits));
	}
	file.close();
	return;
}

template<typename T>
sf::Vector2<T> operator/(sf::Vector2<T> vec, int num)
{
	return sf::Vector2<T>(vec.x / num, vec.y / num);
}

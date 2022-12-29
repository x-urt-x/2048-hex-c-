#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <math.h>
#include <time.h>
#include "windows.h"


using namespace sf;


Font font;
const int w_x = 1000, w_y = 1000;
const int spawn_s_q =2;
const int spawn_q = 1;
const int spawn_r = 2;
const int a = 3;
const int base = 2;
const int max_degree = 11;
const int r = (w_x / 2 - w_x / 10) / (a * 2 - 1);
int score = 0;
const Color tile_col[] = 
{
	Color(97, 77, 60),
	Color(250,231,224),
	Color(249,225,206),
	Color(249,179,124),
	Color(246,149,96),
	Color(243,125,95),
	Color(254,92,65),
	Color(245,208,119),
	Color(249,208,103),
	Color(249,202,88),
	Color(239,196,42),
	Color(244,198,42)
};

RenderWindow window(VideoMode(w_x, w_y), "2048 game");

class Tile : public sf::CircleShape
{

public:
	Tile(){}
	Tile(int x, int y, int r)

		:CircleShape(r,6)
	{
			this->setPosition(x, y);
			text_num.setCharacterSize(r/2);
			text_num.setPosition(x+r*0.866-r/4, y+r/2);
			text_num.setFont(font);
			window.draw(*this);
			num = 0;
			upNum();
	}
	void setNum(int numToSet)
	{
		num = numToSet;
		upNum();
	}
	int getNum() { return num; }

	void upNum()
	{
		text_num.setString(std::to_string(num));
		if (num == 0)
			text_num.setFillColor(tile_col[0]);
		else if (num <= base*2)
			text_num.setFillColor(Color(121, 112, 100));
		else
			text_num.setFillColor(Color::White);
		this->setFillColor(tile_col[int(num == 0 ? 0 : (log(num) / log(2) > max_degree ? max_degree : log(num) / log(2)))]);
		window.draw(*this);
		window.draw(text_num);
	}
private:
	int num = 0;
	Text text_num;
};

void newTile(std::vector < std::vector<Tile*>*>& matr)
{
	int r_x, r_y;
	do
	{
		r_x = rand() % (2 * a - 1);
		r_y = rand() % (2 * a - abs(a - 1 - r_x) - 1);
	} while (matr.at(r_x)->at(r_y)->getNum() != 0);
	matr.at(r_x)->at(r_y)->setNum(base * pow(2, (rand()) % spawn_r));
}


int move(std::vector < std::vector<Tile*>*> &matr, int dir, bool v)
{
	int moved=0;
	std::vector<int> ind;
	for (int i = 0; i < 2 * a - 1;i++) { ind.push_back(i); }
	for (int i = 0 ; ind.size()>0; i++)
	{
		for (int j = 0;j < ind.size(); j++)
		{
			int i_w = dir == 1 ? i : matr.at(ind[j])->size() - i-1;
			bool flag_c = false;
			int num = matr.at(ind[j])->at(i_w)->getNum();
			int k_max = (2 * a - abs(a - 1 - ind[j]) - 1);
			if (num == 0)
			{
				for (int k = i_w + dir; dir == 1 ? k < k_max : k >= 0 ; k+=dir)  
				{
					if (matr.at(ind[j])->at(k)->getNum() !=0)
					{
						moved = 1;
						if (!v)
						{
							matr.at(ind[j])->at(i_w)->setNum(matr.at(ind[j])->at(k)->getNum());
							matr.at(ind[j])->at(k)->setNum(0);
							num = matr.at(ind[j])->at(i_w)->getNum();
							break;
						}
					}
					if (((k == k_max - 1) && (dir == 1)) || ((dir==-1)&&(k==0)))
					{ 
						ind.erase(ind.begin() + j);
						j--;
						flag_c=true;
						break;
					}
				}
			}
				for (int k = i_w+dir; (dir == 1 ? k < k_max : k >= 0) and !flag_c;k+=dir)
				{
					if (matr.at(ind[j])->at(k)->getNum() == num)
					{
						moved = 1;
						if (!v)
						{
							matr.at(ind[j])->at(k)->setNum(0);
							matr.at(ind[j])->at(i_w)->setNum(num * 2);
							score += num * 2;
							break;
						}		
					}
					else
					{
						if(matr.at(ind[j])->at(k)->getNum() != 0) break;
					}
				}
				if (((i_w == k_max - 1) && (dir == 1)) || ((dir == -1) && (i_w == 0)))
				{
					ind.erase(ind.begin() + j);
					j--;
				}
		}
	}
	return moved;
}



int main()
{
	bool lose = true,stop=false;
	int moved=0, num=0,won=0;
	srand(time(NULL));
	font.loadFromFile("arial.ttf");
	window.setVerticalSyncEnabled(true);
	Text scores,msg;
	scores.setFont(font);
	msg.setFont(font);
	scores.setPosition(w_x/2-w_x/10, w_y/20);
	msg.setPosition(w_x / 20, w_y / 2-w_y/10);
	scores.setCharacterSize(w_y/20);
	msg.setCharacterSize(w_y / 5);
	msg.setFillColor(Color::Red);


	std::vector < std::vector<Tile*>* > matrx, matry, matrz;
	for (int i = 0; i < 2 * a - 1; i++)
	{
		matrx.push_back(new std::vector<Tile*>);
		matry.push_back(new std::vector<Tile*>);
		matrz.push_back(new std::vector<Tile*>);
		matrx.at(i)->resize(2 * a - abs(a - 1 - i)-1);
		matry.at(i)->resize(2 * a - abs(a - 1 - i)-1);
		matrz.at(i)->resize(2 * a - abs(a - 1 - i)-1);
	}
	for (int i = 0; i < 2*a-1; i++)
	{                   
		for (int j = 0; j < 2 * a -1 - abs(a - 1 - i); j++)
		{
			int i1 = i - a + 1;
			int j1 = i < a ? j - i : j - a+1;
			matrx.at(i)->at(j) = new Tile(int(w_x/2-r + (i1 * 2 + j1) * r * 0.866 * 1.2), int(w_y/2-r + j1 * r * 2 * 0.75 * 1.2), r);
			matry.at(-j1 + a - 1)->at(-j1 < 0 ? i1 + a - 1 : i1 + j1 + a - 1) = matrx.at(i)->at(j);
			matrz.at(i1 + j1 + a - 1)->at(i1 + j1 < 0 ? i1 + a - 1 :- j1 + a - 1) = matrx.at(i)->at(j);
		}
	}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       

	for (int i = 0; i < spawn_s_q&&i<1+3*a*(a-1); i++)
	{
		newTile(matrx);
	}
	while (window.isOpen())
	{
		window.clear(Color(100, 100, 100));
		for (int i = 0; i < matrx.size(); i++)
		{
			for (int j = 0; j < matrx.at(i)->size(); j++)
			{
				num = matrx.at(i)->at(j)->getNum();
				if (num == 0)
				{
					lose = false;
					if (moved != 0 && moved <= spawn_q)
					{
						newTile(matrx);
						moved++;
					}
				}
				else if (log(num) / log(2) >= max_degree && won==0) won = 2;
				matrx.at(i)->at(j)->upNum();
			}
		}
		if (lose)
		{
			if (!(move(matrz, 1, 1) || move(matrx, -1, 1) || move(matry, 1, 1) || move(matry, -1, 1) || move(matrx, 1, 1) || 
				move(matrz, -1, 1))) { stop = true; msg.setString(static_cast<String>("you lose")); }
		}
		if (won == 2)
		{
			msg.setString(static_cast<String>("you win"));
			window.draw(msg);
			window.display();
			Sleep(5000);
			msg.setString(static_cast<String>(""));
			won = 1;

		}
		scores.setString(std::to_string(score));
		window.draw(msg);
		window.draw(scores);
		Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
				// закрытие окна
			case sf::Event::Closed:
				window.close();
				break;
				// нажатие клавиши

			case sf::Event::KeyPressed:
				if (!stop)
				{
					switch (event.key.code)
					{
					case 	76: moved = move(matrz, 1, 0); break;
					case 	77: moved = move(matrx, -1, 0); break;
					case 	79: moved = move(matry, 1, 0); break;
					case 	80: moved = move(matry, -1, 0); break;
					case 	82: moved = move(matrx, 1, 0); break;
					case 	83: moved = move(matrz, -1, 0); break;
					case 78: if (!lose)newTile(matrx); break;
					case 81:
					{
						for (int i = 0; i < matry.size(); i++)
						{
							for (int j = 0; j < matry.at(i)->size(); j++)
							{
								matry.at(i)->at(j)->setNum(pow(base, i + 1));
							}
						}
						break;
					}
					default: break;
					}
				}
					break;
			default:
				break;
			}
		}
		window.display();
		lose = true;
	}
	return 0;
}
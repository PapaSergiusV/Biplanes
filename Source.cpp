#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <sstream>
#include "camera.h"

using namespace sf;
using namespace std;

class Player {
public:
	float x, y, dx, dy, w, h, speed = 0;
	float health = 5;
	bool empty = false;
	double degrees;
	float bullets = 1000;
	String File;
	Image image; Image i_exp;
	Texture texture; Texture t_exp;
	Sprite sprite; Sprite s_exp;
	//parachute
	float pilot_x = 20, pilot_y = 20, pilot_speed = 0;
	bool parachute = false;
	Image pilot_image;
	Texture pilot_texture;
	Sprite pilot_sprite;
	int pMode = 0;
	enum { iRed, iBlue } indicator;

	Player(String F, float X, float Y, float W, float H, String I) {
		File = F; x = X; y = Y; w = W; h = H;
		image.loadFromFile("img/" + File);
		image.createMaskFromColor(Color(255, 0, 255));
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		if (I == "b") indicator = iBlue;
		if (I == "r") indicator = iRed;
		if (indicator == iBlue) sprite.setOrigin(w / 3 * 2, h / 2);
		if (indicator == iRed) sprite.setOrigin(w / 3, h / 2);
		//exp
		i_exp.loadFromFile("img/explosion.png");
		i_exp.createMaskFromColor(Color(255, 255, 255));
		t_exp.loadFromImage(i_exp);
		s_exp.setTexture(t_exp);
		//parachute
		pilot_image.loadFromFile("img/parachute.png"); pilot_image.createMaskFromColor(Color(255, 0, 255));
		pilot_texture.loadFromImage(pilot_image);
		pilot_sprite.setTexture(pilot_texture);
		if (indicator == iBlue) pilot_sprite.setOrigin(32, 80);
		if (indicator == iRed) pilot_sprite.setOrigin(32, 80);
		if (indicator == iRed) degrees = 10;
		else degrees = -10;
	}

	void moving(float time, float *countDamage) {
		if (empty == false) {
			//стрельба
			if (Keyboard::isKeyPressed(Keyboard::LShift) && bullets > 0) bullets -= 0.2 * time;
			if (Keyboard::isKeyPressed(Keyboard::F)) bullets = 1000;
			//управление газом и тангаж
			if (indicator == iBlue) {
				if (Keyboard::isKeyPressed(Keyboard::A) && speed > 0.15) speed -= 0.0001 * time;
				if (Keyboard::isKeyPressed(Keyboard::D) && speed < 0.28) speed += 0.00015 * time;
				if (Keyboard::isKeyPressed(Keyboard::S) && speed != 0) degrees = degrees - 0.1 * time;
				if (Keyboard::isKeyPressed(Keyboard::W) && speed != 0) degrees = degrees + 0.1 * time;
			}
			if (indicator == iRed) {
				if (Keyboard::isKeyPressed(Keyboard::Left) && speed > -0.28) speed -= 0.0001 * time;
				if (Keyboard::isKeyPressed(Keyboard::Right) && speed < -0.15) speed += 0.00015 * time;
				if (Keyboard::isKeyPressed(Keyboard::Up) && speed != 0) degrees = degrees - 0.1 * time;
				if (Keyboard::isKeyPressed(Keyboard::Down) && speed != 0) degrees = degrees + 0.1 * time;
			}
		}
		//штопор
		if (indicator == iBlue) {
			if (y < 600 && speed < 0.149 && ((int(degrees) % 360 >= 180 && int(degrees) % 360 <= 270) || (int(degrees) % 360 <= -90 && int(degrees) % 360 >= -180))) degrees -= 0.07 * time;
			if (y < 600 && speed < 0.149 && ((int(degrees) % 360 > 270 && int(degrees) % 360 <= 359) || (int(degrees) % 360 <= -1 && int(degrees) % 360 > -90))) degrees += 0.07 * time;
			//if (((int(degrees) % 360 >= 30 && int(degrees) % 360 <= 150) || (int(degrees) % 360 <= -210 && int(degrees) % 360 >= -330)) && speed < 0.03) degrees 

			//обвал скорости
			if (((int(degrees) % 360 >= 30 && int(degrees) % 360 <= 150) || (int(degrees) % 360 <= -210 && int(degrees) % 360 >= -330)) && speed <0.35) speed += 0.0001;
			if (((int(degrees) % 360 >= 210 && int(degrees) % 360 <= 330) || (int(degrees) % 360 <= -30 && int(degrees) % 360 >= -150)) && speed >0.01) speed -= 0.000025;
			if (((int(degrees) % 360 >= 250 && int(degrees) % 360 <= 290) || (int(degrees) % 360 <= -70 && int(degrees) % 360 >= -110)) && speed >0.149) speed -= 0.0002;
		}
		if (indicator == iRed) {
			if (y < 600 && speed > -0.149 && ((int(degrees) % 360 >= 0 && int(degrees) % 360 <= 90) || (int(degrees) % 360 <= -270 && int(degrees) % 360 >= -359))) degrees -= 0.07 * time;
			if (y < 600 && speed > -0.149 && ((int(degrees) % 360 > 90 && int(degrees) % 360 <= 180) || (int(degrees) % 360 <= -180 && int(degrees) % 360 > -270))) degrees += 0.07 * time;
			//if (((int(degrees) % 360 >= 30 && int(degrees) % 360 <= 150) || (int(degrees) % 360 <= -210 && int(degrees) % 360 >= -330)) && speed < 0.03) degrees 

			//обвал скорости
			if (((int(degrees) % 360 >= 210 && int(degrees) % 360 <= 330) || (int(degrees) % 360 <= -30 && int(degrees) % 360 >= -150)) && speed > -0.35) speed -= 0.0001;
			if (((int(degrees) % 360 >= 30 && int(degrees) % 360 <= 150) || (int(degrees) % 360 <= -210 && int(degrees) % 360 >= -330)) && speed < -0.01) speed += 0.000025;
			if (((int(degrees) % 360 >= 70 && int(degrees) % 360 <= 110) || (int(degrees) % 360 <= -250 && int(degrees) % 360 >= -290)) && speed < -0.149) speed += 0.0002;
		}

		//настройки скорости в зависимости от здоровья
		if (indicator == iBlue) {
			if (health > 2 && empty == false) {
				dx = cos(degrees * 3.1415 / 180) * speed;
				if (speed < 0.15) dy = sin(degrees * 3.1415 / 180) * speed + 0.05;
				if (speed >= 0.15) dy = sin(degrees * 3.1415 / 180) * speed + 0.02;
			}
		}
		if (indicator == iRed) {
			if (health > 2 && empty == false) {
				dx = cos(degrees * 3.1415 / 180) * speed;
				if (speed > -0.15) dy = sin(degrees * 3.1415 / 180) * speed + 0.05;
				if (speed <= -0.15) dy = sin(degrees * 3.1415 / 180) * speed + 0.02;
			}
		}

		if (health == 2 && empty == false) {
				dx = cos(degrees * 3.1415 / 180) * speed;
				dy = sin(degrees * 3.1415 / 180) * speed + 0.08;
			}
		if (health == 1 || empty == true) {
			dx = cos(degrees * 3.1415 / 180) * speed;
			dy += 0.0002;
			if (y < 672) {
				if ((int(degrees) % 360 > -90 && int(degrees) % 360 <= 0) || (int(degrees) % 360 > 270 && int(degrees) % 360 <= 360)) degrees += 0.035 * time;
				if ((int(degrees) % 360 > 0 && int(degrees) % 360 <= 80) || (int(degrees) % 360 >= -360 && int(degrees) % 360 <= -280)) degrees += 0.035 * time;
				if ((int(degrees) % 360 >= 100 && int(degrees) % 360 <= 270) || (int(degrees) % 360 <= -90 && int(degrees) % 360 >= -260)) degrees -= 0.035 * time;
			}
		}
		
		x += dx*time;
		y += dy*time;

		//границы экрана
		if (x >= 1350) x = 1; 
		if (x <= 0) x = 1349;
		if (y <= 20) { y = 20; }

		//урон от удара о землю и ангар
		if (y >= 672) {
			y = 672;
			*countDamage += 0.01; 
			if (dy > 0.11) health -= 1;
			if (*countDamage > 4) {
				if ((int(degrees) % 360 >= 30 && int(degrees) % 360 <= 150) || (int(degrees) % 360 <= -210 && int(degrees) % 360 >= -330)) health -= 1;
				if ((int(degrees) % 360 <= -30 && int(degrees) % 360 >= -150) || (int(degrees) % 360 >= 210 && int(degrees) % 360 <= 330)) health -= 1;
				*countDamage = 0;
			}
		}
		//if (((y >= 610 && x >= 519 && x <= 783) || (y >= 537 && x >= 586 && x <= 686)) && speed >= 0.11) health = 0;
		
		sprite.setPosition(x, y);
	}

	void animation(float *Cadr, float time) {
		*Cadr += 0.004 * time; if (*Cadr > 3) *Cadr = 0;
		if (health > 2) sprite.setTextureRect(IntRect(0, 0, w, h));
		//if (health == 4) sprite.setTextureRect(IntRect(w * int(*Cadr), h * (5 - health), w, h));
		//if (health == 3) sprite.setTextureRect(IntRect(w * int(*Cadr), h * (5 - health), w, h));
		if (health == 2 || health == 1) sprite.setTextureRect(IntRect(w * int(*Cadr), h * 3, w, h));

		sprite.setRotation(degrees);
	}

	void explosion(float *Cadr, float time) {
		if (health <= 0 && health >= -100) {
			*Cadr += 0.004 * time;
			if (*Cadr < 3) movingCamera(&cam_x, &cam_y, *Cadr);
			if (*Cadr > 3 && *Cadr < 3.3) {
				cam_x = 675; camera.setCenter(cam_x, cam_y);
			}
			s_exp.setPosition(x - 52, y - 171);
			
			//cout << *Cadr << " ";
			s_exp.setTextureRect(IntRect(192 * int(*Cadr), 0, 192, 192));
			if (*Cadr >= 15)	health = -1000;
		}
		if (health == -1000) {
			*Cadr = 0; y = 672; speed = 0; 
			if (indicator == iBlue) { x = 900; degrees = -10; }
			if (indicator == iRed) { x = 470; degrees = 10; }
			cout << "done ";
		}
	}

	void pilot_moving(float time) {
		if (pMode == 2 && health >= 1) {
			pMode = 1;
			empty = true;
			pilot_x = x;
			pilot_y = y;
		}
		if (pMode == 1) {
			if (pilot_y < 696) {
				if (Keyboard::isKeyPressed(Keyboard::P)) parachute = true;
				if (parachute == false) {
					pilot_speed += 0.0002; pilot_sprite.setTextureRect(IntRect(64, 0, 64, 82));
				}
				if (parachute == true) {
					pilot_sprite.setTextureRect(IntRect(0, 0, 64, 82));
					if (pilot_speed > 0.1) pilot_speed -= 0.001;
					else pilot_speed = 0.09;
				}
				pilot_y += pilot_speed * time;
			}
			if (pilot_y >= 696) {
				pilot_y = 696;
				if (pilot_x > 660) pilot_x -= 0.1 * time;
				if (pilot_x < 650) pilot_x += 0.1 * time;
				pilot_sprite.setTextureRect(IntRect(64, 0, 64, 82));
			}

			pilot_sprite.setPosition(pilot_x, pilot_y);
			if (pilot_x > 650 && pilot_x < 660) {
				pMode = 0; parachute = false; pilot_speed = 0;
			}
		}
		
	}
	
};

class Smoke {
public:
	float x, y = 0;
	float w, h = 32;
	String File;
	Texture texture;
	Image image;
	Sprite sprite;

	Smoke(String F) {
		File = F; 
		image.loadFromFile("img/" + File);
		image.createMaskFromColor(Color(255, 0, 255));
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		sprite.setOrigin(16, 16);
	}
	void update(float time, float X, float Y) {
		x = X; y = Y;
		sprite.setPosition(x, y);
	}
};

/*
class Pilot {
public:
	float pilot_x = 20, pilot_y = 20, speed = 0;
	bool parachute = false;
	String pilot_File;
	Image pilot_image;
	Texture pilot_texture;
	Sprite pilot_sprite;

	Pilot() {
		
		pilot_image.loadFromFile("img/parachute.png"); pilot_image.createMaskFromColor(Color(255, 0, 255));
		pilot_texture.loadFromImage(pilot_image);
		pilot_sprite.setTexture(pilot_texture);
		pilot_sprite.setOrigin(32, 80);
	}

	void moving(float time) {
		if (pilot_y < 696) {
			if (Keyboard::isKeyPressed(Keyboard::Space)) parachute = true;
			if (parachute == false) {
				speed += 0.0002; pilot_sprite.setTextureRect(IntRect(64, 0, 64, 82));
			}
			if (parachute == true) {
				pilot_sprite.setTextureRect(IntRect(0, 0, 64, 82));
				if (speed > 0.1) speed -= 0.001;
				else speed = 0.09;
			}
			pilot_y += speed * time;
		}
		if (pilot_y >= 696) {
			pilot_y = 696;
			if (pilot_x > 660) pilot_x -= 0.1 * time;
			if (pilot_x < 650) pilot_x += 0.1 * time;
			pilot_sprite.setTextureRect(IntRect(64, 0, 64, 82));
		}
		
		pilot_sprite.setPosition(pilot_x, pilot_y);
	}
}; */

int main()
{
	sf::RenderWindow window(sf::VideoMode(1350, 700), "Biplanes");
	//camera
	camera.reset(FloatRect(0, 0, 1350, 700));

	//текстуры
	Texture tex_wall; tex_wall.loadFromFile("img/wall-1370x700.png"); 
	Sprite spr_wall; spr_wall.setTexture(tex_wall); spr_wall.setPosition(-10, 0);

	Texture tex_clouds; tex_clouds.loadFromFile("img/Clouds.png"); 
	Sprite clouds; clouds.setTexture(tex_clouds);

	Image i_shade; i_shade.loadFromFile("img/shade.png"); i_shade.createMaskFromColor(Color(255, 0, 255));
	Texture t_shade; t_shade.loadFromImage(i_shade); Sprite shade; shade.setTexture(t_shade); shade.setOrigin(33, 3);
	
	//TIME и переменные
	Clock clock;
	float Cadr = 0;
	float CadrR = 0;
	float CadrB = 0;
	float exp_cadr = 0;
	float countDamage = 0;
	float countSmoke = 0;
	float rebirthTime = 0;
	float rebirthTimeR = 0;
	float cloudsX = 0;
	//текст на экране
	Font font;
	font.loadFromFile("fonts/Chava.ttf");
	Text text ("", font, 20); text.setColor(Color::Green);
	Text healthScore("", font, 20); healthScore.setColor(Color::Green);
	

	Player blue_plane("blue.png", 900, 672, 103, 46, "b"); //blue_plane.indicator = blue_plane.iBlue;
	Player red_plane("red.png", 470, 672, 103, 46, "r"); //red_plane.indicator = blue_plane.iRed;
	Smoke bsg1("sg.png"); Smoke bsg2("sg.png"); Smoke bsg3("sg.png");
	Smoke bsg4("sg.png"); Smoke bsg5("sg.png"); Smoke bsg6("sg.png");
	Smoke bsb1("sb.png"); Smoke bsb2("sb.png"); Smoke bsb3("sb.png");
	Smoke bsb4("sb.png"); Smoke bsb5("sb.png"); Smoke bsb6("sb.png");


	while (window.isOpen())
	{
		float time = clock.getElapsedTime().asMicroseconds(); //give time as microseconds
		clock.restart();		//reboot time
		time = time / 800;		//speed of game

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}



		//управление здоровьем (чит) ////////////////
		if (Keyboard::isKeyPressed(Keyboard::Num5)) blue_plane.health = 5;
		if (Keyboard::isKeyPressed(Keyboard::Num4)) blue_plane.health = 4;
		if (Keyboard::isKeyPressed(Keyboard::Num3)) blue_plane.health = 3;
		if (Keyboard::isKeyPressed(Keyboard::Num2)) blue_plane.health = 2;
		if (Keyboard::isKeyPressed(Keyboard::Num1)) blue_plane.health = 1;
		if (Keyboard::isKeyPressed(Keyboard::Num0)) red_plane.health = 5;
		if (Keyboard::isKeyPressed(Keyboard::Num9)) red_plane.health = 4;
		if (Keyboard::isKeyPressed(Keyboard::Num8)) red_plane.health = 3;
		if (Keyboard::isKeyPressed(Keyboard::Num7)) red_plane.health = 2;
		if (Keyboard::isKeyPressed(Keyboard::Num6)) red_plane.health = 1;


		if (blue_plane.health < 0) rebirthTime += 0.001;
		if (rebirthTime > 5) {
			blue_plane.health = 5;
			blue_plane.empty = false;
			rebirthTime = 0;
		}
		if (red_plane.health < 0) rebirthTimeR += 0.001;
		if (rebirthTimeR > 5) {
			red_plane.health = 5;
			red_plane.empty = false;
			rebirthTimeR = 0;
		}
		/////////////////////////////////////////////
		window.setView(camera);
		window.clear();
		window.draw(spr_wall);

		//облака
		clouds.setPosition(cloudsX, 152); cloudsX -= 0.01 * time; if (cloudsX <= -1350) cloudsX = 0;
		window.draw(clouds);
		//тень
		if (blue_plane.health > 0) { shade.setPosition(blue_plane.x, 697); window.draw(shade); }

		//функции полета и т.д.
		if (blue_plane.health > 0) {
			blue_plane.moving(time, &countDamage); blue_plane.animation(&CadrB, time);
			window.draw(blue_plane.sprite);
		}
		if (red_plane.health > 0) {
			red_plane.moving(time, &countDamage); red_plane.animation(&CadrR, time);
			window.draw(red_plane.sprite);
		}
		//катапультирование
		if (Keyboard::isKeyPressed(Keyboard::Space)) { blue_plane.pMode = 2; blue_plane.pilot_moving(time); }
		if (blue_plane.pMode == 1) { blue_plane.pilot_moving(time); window.draw(blue_plane.pilot_sprite); }

		if (Keyboard::isKeyPressed(Keyboard::LShift)) { red_plane.pMode = 2; red_plane.pilot_moving(time); }
		if (red_plane.pMode == 1) { red_plane.pilot_moving(time); window.draw(red_plane.pilot_sprite); }
		//cout << "pilot_x: " << blue_plane.pilot_x << ".  y: " << blue_plane.pilot_y << endl;
		//system("cls");

		//взрыв
		if (blue_plane.health <= 0 && blue_plane.health >= -100) {
			blue_plane.explosion(&CadrB, time);
			window.draw(blue_plane.s_exp);
			//cout << "blue ";
		}
		if (red_plane.health <= 0 && red_plane.health >= -100) {
			red_plane.explosion(&CadrR, time);
			window.draw(red_plane.s_exp);
			//cout << "red ";
		}

		///////////дым///////////
		{
			if (blue_plane.health == 4) {
				if (countSmoke == 0) {
					bsg1.update(time, blue_plane.x, blue_plane.y);
				}
				if (countSmoke > 4 && countSmoke < 5) {
					bsg2.update(time, blue_plane.x, blue_plane.y);
				}
				if (countSmoke > 9 && countSmoke < 10) {
					bsg3.update(time, blue_plane.x, blue_plane.y);
				}
				if (countSmoke > 14 && countSmoke < 15) {
					bsg4.update(time, blue_plane.x, blue_plane.y);
				}
				if (countSmoke > 19 && countSmoke < 20) {
					bsg5.update(time, blue_plane.x, blue_plane.y);
				}
				if (countSmoke > 24 && countSmoke < 25) {
					bsg6.update(time, blue_plane.x, blue_plane.y);
				}
				countSmoke += 0.08 * time; if (countSmoke > 25) countSmoke = 0;
				window.draw(bsg1.sprite); window.draw(bsg2.sprite); window.draw(bsg3.sprite);
				window.draw(bsg4.sprite); window.draw(bsg5.sprite); window.draw(bsg6.sprite);
			}
			if (blue_plane.health < 4 && blue_plane.health > 0) {
				if (countSmoke == 0) {
					bsb1.update(time, blue_plane.x, blue_plane.y);
				}
				if (countSmoke > 4 && countSmoke < 5) {
					bsb2.update(time, blue_plane.x, blue_plane.y);
				}
				if (countSmoke > 9 && countSmoke < 10) {
					bsb3.update(time, blue_plane.x, blue_plane.y);
				}
				if (countSmoke > 14 && countSmoke < 15) {
					bsb4.update(time, blue_plane.x, blue_plane.y);
				}
				if (countSmoke > 19 && countSmoke < 20) {
					bsb5.update(time, blue_plane.x, blue_plane.y);
				}
				if (countSmoke > 24 && countSmoke < 25) {
					bsb6.update(time, blue_plane.x, blue_plane.y);
				}
				countSmoke += 0.08 * time; if (countSmoke > 25) countSmoke = 0;
				window.draw(bsb1.sprite); window.draw(bsb2.sprite); window.draw(bsb3.sprite);
				window.draw(bsb4.sprite); window.draw(bsb5.sprite); window.draw(bsb6.sprite);
			}
		/*	if (blue_plane.health < 0) {
				bsg1.update(time, -100, 0);
				bsb1.update(time, -100, 0);
				bsg2.update(time, -100, 0);
				bsb2.update(time, -100, 0);
				bsg3.update(time, -100, 0);
				bsb3.update(time, -100, 0);
				bsg4.update(time, -100, 0);
				bsb4.update(time, -100, 0);
				bsg5.update(time, -100, 0);
				bsb5.update(time, -100, 0);
				bsg6.update(time, -100, 0);
				bsb6.update(time, -100, 0);
			}*/
		}

		{
			if (red_plane.health == 4) {
				if (countSmoke == 0) {
					bsg1.update(time, red_plane.x, red_plane.y);
				}
				if (countSmoke > 4 && countSmoke < 5) {
					bsg2.update(time, red_plane.x, red_plane.y);
				}
				if (countSmoke > 9 && countSmoke < 10) {
					bsg3.update(time, red_plane.x, red_plane.y);
				}
				if (countSmoke > 14 && countSmoke < 15) {
					bsg4.update(time, red_plane.x, red_plane.y);
				}
				if (countSmoke > 19 && countSmoke < 20) {
					bsg5.update(time, red_plane.x, red_plane.y);
				}
				if (countSmoke > 24 && countSmoke < 25) {
					bsg6.update(time, red_plane.x, red_plane.y);
				}
				countSmoke += 0.08 * time; if (countSmoke > 25) countSmoke = 0;
				window.draw(bsg1.sprite); window.draw(bsg2.sprite); window.draw(bsg3.sprite);
				window.draw(bsg4.sprite); window.draw(bsg5.sprite); window.draw(bsg6.sprite);
			}
			if (red_plane.health < 4 && red_plane.health > 0) {
				if (countSmoke == 0) {
					bsb1.update(time, red_plane.x, red_plane.y);
				}
				if (countSmoke > 4 && countSmoke < 5) {
					bsb2.update(time, red_plane.x, red_plane.y);
				}
				if (countSmoke > 9 && countSmoke < 10) {
					bsb3.update(time, red_plane.x, red_plane.y);
				}
				if (countSmoke > 14 && countSmoke < 15) {
					bsb4.update(time, red_plane.x, red_plane.y);
				}
				if (countSmoke > 19 && countSmoke < 20) {
					bsb5.update(time, red_plane.x, red_plane.y);
				}
				if (countSmoke > 24 && countSmoke < 25) {
					bsb6.update(time, red_plane.x, red_plane.y);
				}
				countSmoke += 0.08 * time; if (countSmoke > 25) countSmoke = 0;
				window.draw(bsb1.sprite); window.draw(bsb2.sprite); window.draw(bsb3.sprite);
				window.draw(bsb4.sprite); window.draw(bsb5.sprite); window.draw(bsb6.sprite);
			}
	/*		if (red_plane.health < 0) {
				bsg1.update(time, -100, 0);
				bsb1.update(time, -100, 0);
				bsg2.update(time, -100, 0);
				bsb2.update(time, -100, 0);
				bsg3.update(time, -100, 0);
				bsb3.update(time, -100, 0);
				bsg4.update(time, -100, 0);
				bsb4.update(time, -100, 0);
				bsg5.update(time, -100, 0);
				bsb5.update(time, -100, 0);
				bsg6.update(time, -100, 0);
				bsb6.update(time, -100, 0);
			}*/
		}
		//////////конец: дым////////

		//текст
		ostringstream blue_speed;
		ostringstream blueHealthScore;
		//blue_speed << int(red_plane.speed * 572);
		//blueHealthScore << int(1000000 / (time * 800));
		//blueHealthScore << int(red_plane.degrees) % 360;
		blueHealthScore << int(blue_plane.speed * 572);
		blue_speed << int(red_plane.speed * 572);
		text.setString("REDD:  " + blue_speed.str() + " "); text.setPosition(20, 20);
		healthScore.setString("BLUE: " + blueHealthScore.str() + " "); healthScore.setPosition(20, 40);
		window.draw(text); 
		window.draw(healthScore);
		
		window.display();

	}

	return 0;
}
﻿#include "engine.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>

Engine::Engine() : m_wireframe(false), m_player(WORLD_SIZE / 2 * CHUNK_SIZE_X, 0, WORLD_SIZE / 2 * CHUNK_SIZE_X, 0, 0), m_shader01(), m_textureAtlas(7), m_Chunks(WORLD_SIZE, WORLD_SIZE)
{
	//Initialisation des touches
	for (int i = 0; i < sf::Keyboard::KeyCount; i++)
	{
		m_keyboard[i] = false;
	}

	//Creation du tableau de block info
	m_bInfo = new BlockInfo[256];
}

Engine::~Engine()
{
	delete[] m_bInfo;
}

void Engine::Init()
{
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cerr << " Error while initializing glew .. abording (" << glewGetErrorString(err) << ")" << std::endl;
		abort();
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(75.0f, (float)Width() / (float)Height(), 0.001f, 1000.0f);
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LINE_SMOOTH);


	glEnable(GL_CULL_FACE);

	// Light
	GLfloat light0Pos[4] = { 0.0f, CHUNK_SIZE_Y, 0.0f, 1.0f };
	GLfloat light0Amb[4] = { 0.9f, 0.9f, 0.9f, 1.0f };
	GLfloat light0Diff[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat light0Spec[4] = { 0.2f, 0.2f, 0.2f, 1.0f };

	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0Amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0Spec);

	CenterMouse();
	HideCursor();

	for (int i = 0; i < WORLD_SIZE; i++)
	{
		for (int j = 0; j < WORLD_SIZE; j++)	//Parcours les chunks
		{
			m_Chunks.Get(i, j).SetPosition(CHUNK_SIZE_X * i , -CHUNK_SIZE_Y / 2, CHUNK_SIZE_Z * j );

			for (int x = 0; x < CHUNK_SIZE_X; ++x)
			{
				for (int z = 0; z < CHUNK_SIZE_Z; ++z)
				{
					for (int y = 0; y < CHUNK_SIZE_Y; ++y)	//parcours les blocks du chunk
					{
						if (y < 64 && y >= 63)
							m_Chunks.Get(i, j).SetBlock(x, y, z, BTYPE_GRASS);
						else if (y < 63 && y >= 57)
							m_Chunks.Get(i, j).SetBlock(x, y, z, BTYPE_DIRT);
						else if (y < 57 && y > 40)
							m_Chunks.Get(i, j).SetBlock(x, y, z, BTYPE_STONE);
						else if (y <= 40 && y > 38)
							m_Chunks.Get(i, j).SetBlock(x, y, z, BTYPE_BED_ROCK);
					}
				}
			}
		}
	}




	//Terrain de jeux

	//Arche
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(5, 2 + 64, 5, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(6, 2 + 64, 5, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(7, 2 + 64, 5, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(5, 1 + 64, 5, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(7, 1 + 64, 5, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(5, 0 + 64, 5, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(7, 0 + 64, 5, BTYPE_WOOD_PLANK);

	//Mur
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(10, 0 + 64, 14, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(11, 0 + 64, 14, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(12, 0 + 64, 14, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(13, 0 + 64, 14, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(10, 1 + 64, 14, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(11, 1 + 64, 14, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(12, 1 + 64, 14, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(13, 1 + 64, 14, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(10, 2 + 64, 14, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(11, 2 + 64, 14, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(12, 2 + 64, 14, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(13, 2 + 64, 14, BTYPE_WOOD_PLANK);

	//Mur 2
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(2, 0 + 64, 8, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(2, 0 + 64, 9, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(2, 0 + 64, 10, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(2, 0 + 64, 11, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(2, 1 + 64, 8, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(2, 1 + 64, 9, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(2, 1 + 64, 10, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(2, 1 + 64, 11, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(2, 2 + 64, 8, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(2, 2 + 64, 9, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(2, 2 + 64, 10, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(2, 2 + 64, 11, BTYPE_WOOD_PLANK);

	//Esclaier
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(13, 0 + 64, 6, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(13, 1 + 64, 7, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(13, 2 + 64, 8, BTYPE_WOOD_PLANK);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(13, 3 + 64, 9, BTYPE_WOOD_PLANK);



	m_Chunks.Get(0, 0).SetBlock(3, 64, 3, BTYPE_TEST);
	m_Chunks.Get(WORLD_SIZE / 2, WORLD_SIZE / 2).SetBlock(8, 64, 8, BTYPE_CHEST);

}

void Engine::DeInit()
{
}

void Engine::LoadResource()
{
	//Load texture qui ne sont pas dans l'atlas
	LoadTexture(m_textureSky, TEXTURE_PATH "sky.jpg");
	LoadTexture(m_textureCrosshair, TEXTURE_PATH "cross.bmp");
	LoadTexture(m_textureFont, TEXTURE_PATH "font.bmp");

	//Load texture dans l'atlas
	m_bInfo[BTYPE_GRASS].Init(BTYPE_GRASS, "Grass");
	m_texBlockIndex = m_textureAtlas.AddTexture(TEXTURE_PATH "block_grass.bmp");
	m_textureAtlas.TextureIndexToCoord(m_texBlockIndex, m_bInfo[BTYPE_GRASS].u, m_bInfo[BTYPE_GRASS].v, m_bInfo[BTYPE_GRASS].w, m_bInfo[BTYPE_GRASS].h);

	m_bInfo[BTYPE_TEST].Init(BTYPE_TEST, "Test");
	m_texBlockIndex = m_textureAtlas.AddTexture(TEXTURE_PATH "block_test.bmp");
	m_textureAtlas.TextureIndexToCoord(m_texBlockIndex, m_bInfo[BTYPE_TEST].u, m_bInfo[BTYPE_TEST].v, m_bInfo[BTYPE_TEST].w, m_bInfo[BTYPE_TEST].h);

	m_bInfo[BTYPE_STONE].Init(BTYPE_STONE, "Stone");
	m_texBlockIndex = m_textureAtlas.AddTexture(TEXTURE_PATH "block_stone.bmp");
	m_textureAtlas.TextureIndexToCoord(m_texBlockIndex, m_bInfo[BTYPE_STONE].u, m_bInfo[BTYPE_STONE].v, m_bInfo[BTYPE_STONE].w, m_bInfo[BTYPE_STONE].h);

	m_bInfo[BTYPE_WOOD_PLANK].Init(BTYPE_WOOD_PLANK, "Wood Plank");
	m_texBlockIndex = m_textureAtlas.AddTexture(TEXTURE_PATH "block_wood_plank.bmp");
	m_textureAtlas.TextureIndexToCoord(m_texBlockIndex, m_bInfo[BTYPE_WOOD_PLANK].u, m_bInfo[BTYPE_WOOD_PLANK].v, m_bInfo[BTYPE_WOOD_PLANK].w, m_bInfo[BTYPE_WOOD_PLANK].h);

	m_bInfo[BTYPE_CHEST].Init(BTYPE_CHEST, "Chest");
	m_texBlockIndex = m_textureAtlas.AddTexture(TEXTURE_PATH "block_chest.bmp");
	m_textureAtlas.TextureIndexToCoord(m_texBlockIndex, m_bInfo[BTYPE_CHEST].u, m_bInfo[BTYPE_CHEST].v, m_bInfo[BTYPE_CHEST].w, m_bInfo[BTYPE_CHEST].h);

	m_bInfo[BTYPE_BED_ROCK].Init(BTYPE_BED_ROCK, "Bed Rock");
	m_texBlockIndex = m_textureAtlas.AddTexture(TEXTURE_PATH "block_bed_rock.bmp");
	m_textureAtlas.TextureIndexToCoord(m_texBlockIndex, m_bInfo[BTYPE_BED_ROCK].u, m_bInfo[BTYPE_BED_ROCK].v, m_bInfo[BTYPE_BED_ROCK].w, m_bInfo[BTYPE_BED_ROCK].h);

	m_bInfo[BTYPE_DIRT].Init(BTYPE_DIRT, "Dirt");
	m_texBlockIndex = m_textureAtlas.AddTexture(TEXTURE_PATH "block_dirt.bmp");
	m_textureAtlas.TextureIndexToCoord(m_texBlockIndex, m_bInfo[BTYPE_DIRT].u, m_bInfo[BTYPE_DIRT].v, m_bInfo[BTYPE_DIRT].w, m_bInfo[BTYPE_DIRT].h);


	if (!m_textureAtlas.Generate(128, false))
	{
		std::cout << " Unable to generate texture atlas ..." << std::endl;
		abort();
	}

	std::cout << " Loading and compiling shaders ..." << std::endl;
	if (!m_shader01.Load(SHADER_PATH "shader01.vert", SHADER_PATH "shader01.frag", true))
	{
		std::cout << " Failed to load shader " << std::endl;
		exit(1);
	}
}

void Engine::UnloadResource()
{
}

void Engine::Render(float elapsedTime)
{


	static float gameTime = elapsedTime;

	gameTime += elapsedTime;

	//On met a jour le fps
	if ((int)(gameTime * 100) % 10 == 0)
		m_fps = round(1 / elapsedTime);
	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Transformations initiales
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Update le player
	m_player.Move(m_keyboard[sf::Keyboard::W], m_keyboard[sf::Keyboard::S], m_keyboard[sf::Keyboard::A], m_keyboard[sf::Keyboard::D], elapsedTime, m_Chunks);
	m_player.ApplyRotation();
	m_player.ApplyTranslation();


	//Ciel
	glPushMatrix();
	glRotatef(gameTime * 1.2, 0, 1, 0);
	m_textureSky.Bind();

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0.50);			glVertex3f(-512, -512, -512);
	glTexCoord2f(0.25, 0.50);		glVertex3f(512, -512, -512);
	glTexCoord2f(0.25, 0.25);		glVertex3f(512, 512, -512);
	glTexCoord2f(0, 0.25);			glVertex3f(-512, 512, -512);

	glTexCoord2f(0.25, 0.25);		glVertex3f(512, 512, -512);
	glTexCoord2f(0.25, 0.5);		glVertex3f(512, -512, -512);
	glTexCoord2f(0.5, 0.5);			glVertex3f(512, -512, 512);
	glTexCoord2f(0.5, 0.25);		glVertex3f(512, 512, 512);

	glTexCoord2f(0.75, 0.50);		glVertex3f(-512, -512, 512);
	glTexCoord2f(0.75, 0.25);		glVertex3f(-512, 512, 512);
	glTexCoord2f(0.5, 0.25);		glVertex3f(512, 512, 512);
	glTexCoord2f(0.5, 0.50);		glVertex3f(512, -512, 512);

	glTexCoord2f(0.75, 0.25);		glVertex3f(-512, 512, 512);
	glTexCoord2f(0.75, 0.50);		glVertex3f(-512, -512, 512);
	glTexCoord2f(1, 0.50);			glVertex3f(-512, -512, -512);
	glTexCoord2f(1, 0.25);			glVertex3f(-512, 512, -512);

	glTexCoord2f(0.5, 0);			glVertex3f(-512, 512, 512);
	glTexCoord2f(0.25, 0);			glVertex3f(-512, 512, -512);
	glTexCoord2f(0.25, 0.25);		glVertex3f(512, 512, -512);
	glTexCoord2f(0.5, 0.25);		glVertex3f(512, 512, 512);
	glEnd();


	glPopMatrix();

	////Render des chunk

	m_textureAtlas.Bind();

	for (int i = 0; i < WORLD_SIZE; i++)
	{
		for (int j = 0; j < WORLD_SIZE; j++)	//Parcour les chunk
		{
			if (m_Chunks.Get(i, j).IsDirty())
				m_Chunks.Get(i, j).Update(m_bInfo);
			m_shader01.Use();
			m_Chunks.Get(i, j).Render();
			Shader::Disable();
		}
	}

	//Render le hui
	if (m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	DrawHud();
	if (m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);




}

void Engine::KeyPressEvent(unsigned char key)
{
	//update le teableau
	m_keyboard[key] = true;

	//Esc -> Arrete le programme
	if (m_keyboard[36])
		Stop();

	//f10 -> toggle fulscreen mode
	else if (m_keyboard[94])
		SetFullscreen(!IsFullscreen());
	//V -> toogle noclip mode
	else if (m_keyboard[sf::Keyboard::V])
		m_player.ToggleNoClip();

	//Lctr -> Sneak
	else if (m_keyboard[sf::Keyboard::LControl])
		m_player.SetSneak(true);

	//LSHIFT -> RUN
	else if (m_keyboard[sf::Keyboard::LShift])
		m_player.SetRunning(true);

	//space -> jump
	if (m_keyboard[sf::Keyboard::Space])
		m_player.Jump();

	//y -> toggle wireframe mode
	else if (m_keyboard[24])
	{
		m_wireframe = !m_wireframe;
		if (m_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

}

void Engine::KeyReleaseEvent(unsigned char key)
{
	//update le teableau
	m_keyboard[key] = false;

	//end sneak
	if (!m_keyboard[sf::Keyboard::LControl])
		m_player.SetSneak(false);

	//end run
	if (!m_keyboard[sf::Keyboard::LShift])
		m_player.SetRunning(false);

}

void Engine::MouseMoveEvent(int x, int y)
{
	// Centrer la souris seulement si elle n'est pas déjà centrée
	// Il est nécessaire de faire la vérification pour éviter de tomber
	// dans une boucle infinie où l'appel à CenterMouse génère un
	// MouseMoveEvent, qui rapelle CenterMouse qui rapelle un autre 
	// MouseMoveEvent, etc
	if (x == (Width() / 2) && y == (Height() / 2))
		return;
	CenterMouse();

	float relativeX = x - Width() / 2;
	float relativeY = y - Height() / 2;

	m_player.TurnLeftRight(relativeX * MOUSE_SENSIBILITY);
	m_player.TurnTopBottom(relativeY * MOUSE_SENSIBILITY);

}

void Engine::MousePressEvent(const MOUSE_BUTTON &button, int x, int y)
{
}

void Engine::MouseReleaseEvent(const MOUSE_BUTTON &button, int x, int y)
{
}

bool Engine::LoadTexture(Texture& texture, const std::string& filename, bool stopOnError)
{
	texture.Load(filename);
	if (!texture.IsValid())
	{
		std::cerr << "Unable to load texture (" << filename << ")" << std::endl;
		if (stopOnError)
			Stop();

		return false;
	}

	return true;
}

void Engine::DrawHud()
{
	// Setter le blend function , tout ce qui sera noir sera transparent
	glDisable(GL_LIGHTING);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);

	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, Width(), 0, Height(), -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// Bind de la texture pour le font
	m_textureFont.Bind();

	std::ostringstream ss;

	ss << "Fps: " << m_fps;
	PrintText(10, Height() - 25, 16, ss.str());


	ss.str("");
	ss << m_player.Position();
	PrintText(10, 10, 16, ss.str());

	ss.str("");
	ss << "Health:";
	//Pour chaque 10 point de vie on met un carre sinon un espace
	for (int i = 0; i < m_player.GetHP() / 5; i++)
	{
		ss << (char)254; // Le carractere ■
	}
	for (int i = 0; i < 100 / 5 - m_player.GetHP() / 5; i++)
	{
		ss << " ";
	}
	PrintText((Width() - ss.str().length() * 12) - 10, Height() - 25, 16, ss.str());

	// Affichage du crosshair
	m_textureCrosshair.Bind();
	static const int crossSize = 32;
	glLoadIdentity();
	glTranslated(Width() / 2 - crossSize / 2, Height() / 2 - crossSize / 2, 0);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2i(0, 0);
	glTexCoord2f(1, 0);
	glVertex2i(crossSize, 0);
	glTexCoord2f(1, 1);
	glVertex2i(crossSize, crossSize);
	glTexCoord2f(0, 1);
	glVertex2i(0, crossSize);
	glEnd();

	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void Engine::PrintText(unsigned int x, unsigned int y, int size, const std::string & t)
{
	glLoadIdentity();
	glTranslated(x, y, 0);
	for (unsigned int i = 0; i < t.length(); ++i)
	{
		float left = (float)((t[i] - 32) % 16) / 16.0f;
		float top = (float)((t[i] - 175) / 16) / 16.0f;

		top += 0.5f;
		glBegin(GL_QUADS);
		glTexCoord2f(left, 1.0f - top - 0.0625f);
		glVertex2f(0, 0);
		glTexCoord2f(left + 0.0625f, 1.0f - top - 0.0625f);
		glVertex2f(size, 0);
		glTexCoord2f(left + 0.0625f, 1.0f - top);
		glVertex2f(size, size);
		glTexCoord2f(left, 1.0f - top);
		glVertex2f(0, size);
		glEnd();
		glTranslated(size - (size / 4), 0, 0);
	}
}

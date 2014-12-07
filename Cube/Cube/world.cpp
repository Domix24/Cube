#include "world.h"


World::World() : m_chunks(WORLD_SIZE, WORLD_SIZE)
{
	//Parcours les chunks et les positionne dans la map
	for (int i = 0; i < WORLD_SIZE; i++)
		for (int j = 0; j < WORLD_SIZE; j++)
		{
			m_chunks.Get(i, j).SetPosition(CHUNK_SIZE_X * i, 0, CHUNK_SIZE_Z * j);

			//Set les pointeur sur les chunk qui est coll�
			m_chunks.Get(i, j).m_positiveX = NULL;
			m_chunks.Get(i, j).m_negativeX = NULL;
			m_chunks.Get(i, j).m_positiveZ = NULL;
			m_chunks.Get(i, j).m_negativeZ = NULL;

			if (i < WORLD_SIZE - 1)
				m_chunks.Get(i, j).m_positiveX = &m_chunks.Get(i + 1, j);

			if (i > 0)
				m_chunks.Get(i, j).m_negativeX = &m_chunks.Get(i - 1, j);

			if (j < WORLD_SIZE - 1)
				m_chunks.Get(i, j).m_positiveZ = &m_chunks.Get(i, j + 1);

			if (j > 0)
				m_chunks.Get(i, j).m_negativeZ = &m_chunks.Get(i, j - 1);
		}


}


World::~World()
{
}


void World::InitMap(int octaves, float freq, float amp, int seed)
{
	std::srand(seed);
	//Erase map
	for (int i = 0; i < WORLD_SIZE; i++)
		for (int j = 0; j < WORLD_SIZE; j++)
			for (int x = 0; x < CHUNK_SIZE_X; ++x)
				for (int z = 0; z < CHUNK_SIZE_Z; ++z)
					for (int y = 0; y < CHUNK_SIZE_Y; ++y)
						m_chunks.Get(i, j).SetBlock(x, y, z, BTYPE_AIR);


	Perlin perlin(octaves, freq, amp, seed);

	int count = 0;

	//Map height
	for (int i = 0; i < WORLD_SIZE; i++)
		for (int j = 0; j < WORLD_SIZE; j++)
		{
			for (int x = 0; x < CHUNK_SIZE_X; ++x)
				for (int z = 0; z < CHUNK_SIZE_Z; ++z)
				{
					float val = perlin.Get((float)(i * CHUNK_SIZE_X + x) / 2000.f, (float)(j * CHUNK_SIZE_Z + z) / 2000.f);
					//Couche
					for (int y = 0; y <= CHUNK_SIZE_Y; y++)
					{
						if (y == 0)
							m_chunks.Get(i, j).SetBlock(x, val + 64 - y, z, BTYPE_GRASS);
						else if (y >= 1 && y < 4)
							m_chunks.Get(i, j).SetBlock(x, val + 64 - y, z, BTYPE_DIRT);
						else if (y >= 4)
							m_chunks.Get(i, j).SetBlock(x, val + 64 - y, z, BTYPE_STONE);
					}
					//Plancher de bedrock
					m_chunks.Get(i, j).SetBlock(x, 0, z, BTYPE_BED_ROCK);
					m_chunks.Get(i, j).SetBlock(x, 1, z, BTYPE_BED_ROCK);

					m_chunks.Get(i, j).SetBlock(x, 2, z, BTYPE_STONE);
					m_chunks.Get(i, j).SetBlock(x, 2, z, BTYPE_STONE);

				}
			if (count++ % 300 == 0)
				std::cout << "Chunk " << count << " / " << WORLD_SIZE * WORLD_SIZE << " Created" << std::endl;


		}

	//Minerals
	/*
	Coal:
	layer range: 1 - 128
	Percentage : 1.19%

	Iron:
	layer Range : 1 - 65
	Percentage : 0.68%

	Gold:
	layer range : 1 - 33
	Percentage : 0.13%

	Redstone:
	Layer Range : 1 - 15
	Percentages by layer : 0.98%

	Lapis Lazuli:
	Layer Range : 1 - 33
	Percentage : 0.13%

	Diamond:
	Layer Range : 1 - 15
	Percentage : 0.12%
	*/
	std::cout << "Adding minerals..." << std::endl;
	
	for (int i = 0; i < WORLD_SIZE; i++)
		for (int j = 0; j < WORLD_SIZE; j++)
			for (int x = 0; x < CHUNK_SIZE_X; ++x)
				for (int z = 0; z < CHUNK_SIZE_Z; ++z)
					for (int y = 3; y <= CHUNK_SIZE_Y; y++)
					{
						if (rand() % 10000 >= 9891 && m_chunks.Get(i, j).GetBlock(x, y, z) == BTYPE_STONE)
						{
							AddMineral(BTYPE_COAL, i, j, x, y, z);
						}
						else if (rand() % 10000 >= 9932 && m_chunks.Get(i, j).GetBlock(x, y, z) == BTYPE_STONE && y < 66)
						{
							AddMineral(BTYPE_IRON, i, j, x, y, z);
						}
						else if (rand() % 10000 >= 9987 && m_chunks.Get(i, j).GetBlock(x, y, z) == BTYPE_STONE && y < 34)
						{
							AddMineral(BTYPE_GOLD, i, j, x, y, z);
						}
						else if (rand() % 10000 >= 9907 && m_chunks.Get(i, j).GetBlock(x, y, z) == BTYPE_STONE && y < 16)
						{
							AddMineral(BTYPE_REDSTONE, i, j, x, y, z);
						}
						else if (rand() % 10000 >= 9987 && m_chunks.Get(i, j).GetBlock(x, y, z) == BTYPE_STONE && y < 34)
						{
							AddMineral(BTYPE_LAPIS_LAZULI, i, j, x, y, z);
						}
						else if (rand() % 10000 >= 9988 && m_chunks.Get(i, j).GetBlock(x, y, z) == BTYPE_STONE && y < 16)
						{
							AddMineral(BTYPE_DIAMOND, i, j, x, y, z);
						}

					}

	if (freq != 0)
		std::cout << "Map created with this seed: " << seed << std::endl;
	else
		std::cout << "Flat map created" << std::endl;
}

BlockType World::BlockAt(float x, float y, float z)
{
	Vector3<float>chunkPos(floor(x / CHUNK_SIZE_X), 0, floor(z / CHUNK_SIZE_Z));

	if (chunkPos.x >= 0 && chunkPos.z >= 0 && chunkPos.x < WORLD_SIZE && chunkPos.z < WORLD_SIZE)
	{

		Vector3<float>blockPos(x - (chunkPos.x * CHUNK_SIZE_X), y, z - (chunkPos.z * CHUNK_SIZE_X));

		return m_chunks.Get(chunkPos.x, chunkPos.z).GetBlock(blockPos.x, blockPos.y, blockPos.z);
	}
	else
		return BTYPE_AIR;
}

Chunk& World::ChunkAt(float x, float z)
{
	if (x >= 0 && z >= 0 && x < WORLD_SIZE  && z < WORLD_SIZE)
		return m_chunks.Get(x, z);
}

void World::LoadMap(std::string filename, BlockInfo *binfo)
{
	std::cout << "Loading " << filename << "..." << std::endl;

	//Erase map
	for (int i = 0; i < WORLD_SIZE; i++)
		for (int j = 0; j < WORLD_SIZE; j++)
			for (int x = 0; x < CHUNK_SIZE_X; ++x)
				for (int z = 0; z < CHUNK_SIZE_Z; ++z)
					for (int y = 0; y < CHUNK_SIZE_Y; ++y)
					{
						m_chunks.Get(i, j).SetBlock(x, y, z, BTYPE_AIR);
						m_chunks.Get(i, j).SetBlock(0, 0, 0, BTYPE_TEST);
					}

	//Open file
	std::ifstream file;
	file.open(filename);

	//Chunk pos, block pos, blocktype
	int i, j, x, y, z, b;

	//Read All file
	std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	std::stringstream ss(str);

	//Close it
	file.close();

	//Get Size
	ss.seekg(0, file.end);
	float length = ss.tellg() / 1024;
	ss.seekg(0, file.beg);

	std::string word;

	//Read
	while (ss)
	{
		//Get the value for a block
		ss >> i >> j >> x >> y >> z >> b;

		//Set block
		if (b >= 0 && b << NUMBER_OF_BLOCK)
			m_chunks.Get(i, j).SetBlock(x, y, z, binfo[b].GetType());

		//Tell Where we are
		if (ss.tellg() % 1024 == 0)
			std::cout << float(ss.tellg() / 1024) << " / " << length << " KB loaded" << std::endl;

	}

	std::cout << filename << " Loaded" << std::endl;
}

void World::SaveMap(std::string filename)
{
	std::ofstream file;
	file.open(filename);
	int count = 1;

	for (int i = 0; i < WORLD_SIZE; i++)
		for (int j = 0; j < WORLD_SIZE; j++)
		{
			for (int x = 0; x < CHUNK_SIZE_X; ++x)

				for (int z = 0; z < CHUNK_SIZE_Z; ++z)
					for (int y = 0; y <= CHUNK_SIZE_Y; y++)
					{
						if (m_chunks.Get(i, j).GetBlock(x, y, z) != BTYPE_AIR)
							file << i << " " << j << " " << x << " " << y << " " << z << " " << (int)m_chunks.Get(i, j).GetBlock(x, y, z) << " ";
					}

			std::cout << "Chunk " << count++ << " / " << WORLD_SIZE * WORLD_SIZE << " saved" << std::endl;
			file << std::endl;
		}
	file << "END " << std::endl;
	file.close();

	std::cout << "Map saved as " << filename << std::endl;
}

void  World::AddMineral(BlockType mineral, int i, int j, int x, int y, int z)
{

	m_chunks.Get(i, j).SetBlock(x, y, z, mineral);
	if (rand() % 100 >= 60 && m_chunks.Get(i, j).GetBlock(x + 1, y, z) == BTYPE_STONE)
	{
		m_chunks.Get(i, j).SetBlock(x + 1, y, z, mineral);
	}
	if (rand() % 100 >= 60 && m_chunks.Get(i, j).GetBlock(x - 1, y, z) == BTYPE_STONE)
	{
		m_chunks.Get(i, j).SetBlock(x - 1, y, z, mineral);
	}
	if (rand() % 100 >= 60 && m_chunks.Get(i, j).GetBlock(x, y + 1, z) == BTYPE_STONE)
	{
		m_chunks.Get(i, j).SetBlock(x, y + 1, z, mineral);
	}
	if (rand() % 100 >= 60 && m_chunks.Get(i, j).GetBlock(x, y - 1, z) == BTYPE_STONE)
	{
		m_chunks.Get(i, j).SetBlock(x, y - 1, z, mineral);
	}
	if (rand() % 100 >= 60 && m_chunks.Get(i, j).GetBlock(x, y, z + 1) == BTYPE_STONE)
	{
		m_chunks.Get(i, j).SetBlock(x, y, z + 1, mineral);
	}
	if (rand() % 100 >= 60 && m_chunks.Get(i, j).GetBlock(x, y, z - 1) == BTYPE_STONE)
	{
		m_chunks.Get(i, j).SetBlock(x, y, z - 1, mineral);
	}
}


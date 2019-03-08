
#include "Map.h"

uint8_t Map::getMapData(uint8_t x,uint8_t y){
	return mapData[y][x];
}

void Map::setMapData(uint8_t x,uint8_t y,uint8_t value){
	mapData[y][x] = value;
}

void Map::buildMap(uint8_t scenario){
	switch(scenario){
		case 0:
		for (uint8_t x = 0;x<11;x++)
		{
			for (uint8_t y = 0;y<9;y++)
			{
				setMapData(x,y,CRATE);
			}
		}
		for (uint8_t x = 0;x<5;x++)
		{
			for (uint8_t y = 0;y<4;y++)
			{
				setMapData((x*2)+1,(y*2)+1,WALL);
			}
		}
		setMapData(0,0,OPENSPACE);
		setMapData(0,1,OPENSPACE);
		setMapData(1,0,OPENSPACE);
		setMapData(10,8,OPENSPACE);
		setMapData(9,8,OPENSPACE);
		setMapData(10,7,OPENSPACE);
		break;
	}
}

void Map::changeBlock(uint8_t x,uint8_t y,uint8_t state){
	if (mapData[x][y] != state)
	{
		updates[updateIndex][0] = x;
		updates[updateIndex][1] = y;
		updates[updateIndex][2] = state;
		updateIndex++;
	}
	
}
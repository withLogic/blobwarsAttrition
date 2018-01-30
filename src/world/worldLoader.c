/*
Copyright (C) 2018 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "worldLoader.h"

static void loadEnemyTypes(char *enemyTypes);
static void loadTriggers(cJSON *root);
static void loadSprites(cJSON *root);
static void loadBob(cJSON *root);
static void loadEntities(cJSON *root);
static void loadObjectives(cJSON *root);

void loadWorld(char *filename)
{
	cJSON *root;
	char *text;
	
	memset(&world, 0, sizeof(World));
	
	world.entityTail = &world.entityHead;
	world.triggerTail = &world.triggerHead;
	world.objectiveTail = &world.objectiveHead;
	
	text = readFile(filename);

	root = cJSON_Parse(text);
	
	STRNCPY(world.id, cJSON_GetObjectItem(root, "id")->valuestring, MAX_NAME_LENGTH);
	STRNCPY(world.name, cJSON_GetObjectItem(root, "name")->valuestring, MAX_NAME_LENGTH);
	world.minEnemySpawnTime = cJSON_GetObjectItem(root, "minEnemySpawnTime")->valueint;
	world.maxEnemySpawnTime = cJSON_GetObjectItem(root, "maxEnemySpawnTime")->valueint;
	STRNCPY(world.music, cJSON_GetObjectItem(root, "music")->valuestring, MAX_FILENAME_LENGTH);
	STRNCPY(world.tileset, cJSON_GetObjectItem(root, "tileset")->valuestring, MAX_NAME_LENGTH);
	STRNCPY(world.background, cJSON_GetObjectItem(root, "background")->valuestring, MAX_FILENAME_LENGTH);
	world.entityCounter = cJSON_GetObjectItem(root, "entityCounter")->valueint;
	
	loadEnemyTypes(cJSON_GetObjectItem(root, "enemyTypes")->valuestring);
	
	loadTriggers(cJSON_GetObjectItem(root, "triggers"));
	
	loadSprites(cJSON_GetObjectItem(root, "sprites"));
	
	loadBob(cJSON_GetObjectItem(root, "bob"));
	
	loadEntities(cJSON_GetObjectItem(root, "entities"));
	
	loadObjectives(cJSON_GetObjectItem(root, "objectives"));
}

static void loadEnemyTypes(char *enemyTypes)
{
	int i;
	char *enemyType;

	for (i = 0 ; i < strlen(enemyTypes) ; i++)
	{
		if (enemyTypes[i] == '|')
		{
			world.numEnemyTypes++;
		}
	}

	world.enemyTypes = malloc(world.numEnemyTypes * sizeof(char*));

	i = 0;
	enemyType = strtok(enemyTypes, "|");
	while (enemyType)
	{
		world.enemyTypes[i] = malloc(strlen(enemyType) + 1);
		strcpy(world.enemyTypes[i], enemyType);

		enemyType = strtok(NULL, "|");

		i++;
	}
}

static void loadTriggers(cJSON *root)
{
	cJSON *node;
	Trigger *t;
	
	for (node = root->child ; node != NULL ; node = node->next)
	{
		t = malloc(sizeof(Trigger));
		memset(t, 0, sizeof(Trigger));
		world.triggerTail->next = t;
		world.triggerTail = t;
		
		STRNCPY(t->name, cJSON_GetObjectItem(node, "name")->valuestring, MAX_NAME_LENGTH);
		STRNCPY(t->targetNames, cJSON_GetObjectItem(node, "targetNames")->valuestring, MAX_DESCRIPTION_LENGTH);
		STRNCPY(t->message, cJSON_GetObjectItem(node, "message")->valuestring, MAX_DESCRIPTION_LENGTH);
		t->x = cJSON_GetObjectItem(node, "x")->valueint;
		t->y = cJSON_GetObjectItem(node, "y")->valueint;
		t->w = cJSON_GetObjectItem(node, "w")->valueint;
		t->h = cJSON_GetObjectItem(node, "h")->valueint;
	}
}

static void loadSprites(cJSON *root)
{
	cJSON *node;
	
	for (node = root->child ; node != NULL ; node = node->next)
	{
		loadSprite(node);
	}
}

static void loadBob(cJSON *root)
{
	Bob *b;
	
	b = (Bob*)createEntity("Bob");
	b->x = cJSON_GetObjectItem(root, "x")->valueint;
	b->y = cJSON_GetObjectItem(root, "y")->valueint;
	b->facing = lookup(cJSON_GetObjectItem(root, "facing")->valuestring);
	
	initBob(b);
}

static void loadEntities(cJSON *root)
{
	cJSON *node;
	
	for (node = root->child ; node != NULL ; node = node->next)
	{
		self = createEntity(cJSON_GetObjectItem(node, "type")->valuestring);
		
		if (cJSON_GetObjectItem(node, "name"))
		{
			STRNCPY(self->name, cJSON_GetObjectItem(node, "name")->valuestring, MAX_NAME_LENGTH);
		}
		
		self->x = cJSON_GetObjectItem(node, "x")->valueint;
		self->y = cJSON_GetObjectItem(node, "y")->valueint;
		
		self->load(node);
	}
}

static void loadObjectives(cJSON *root)
{
	Objective *o;
	cJSON *node;
	
	for (node = root->child ; node != NULL ; node = node->next)
	{
		o = malloc(sizeof(Objective));
		memset(o, 0, sizeof(Objective));
		world.objectiveTail->next = o;
		world.objectiveTail = o;
		
		STRNCPY(o->id, cJSON_GetObjectItem(node, "id")->valuestring, MAX_NAME_LENGTH);
		STRNCPY(o->targetName, cJSON_GetObjectItem(node, "targetName")->valuestring, MAX_NAME_LENGTH);
		STRNCPY(o->description, cJSON_GetObjectItem(node, "description")->valuestring, MAX_DESCRIPTION_LENGTH);
		o->totalValue = cJSON_GetObjectItem(node, "totalValue")->valueint;
		o->targetValue = cJSON_GetObjectItem(node, "totalValue")->valueint;
		o->currentValue = cJSON_GetObjectItem(node, "currentValue")->valueint;
		o->required = cJSON_GetObjectItem(node, "required")->valueint;
	}
}
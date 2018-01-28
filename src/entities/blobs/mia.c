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

#include "mia.h"

static void reset(void);
static void tick(void);
static void touch(Entity *other);
static void preTeleport(void);
static void teleport(void);

void initMIA(Entity *e)
{
	MIA *m;
	
	initUnit(e);
	
	m = (MIA*)e;
	
	m->type = ET_MIA;
	
	m->tx = m->ty = -1;

	m->sprite[FACING_LEFT] = getSpriteIndex("MIA");
	m->sprite[FACING_RIGHT] = getSpriteIndex("MIA");
	m->sprite[FACING_DIE] = getSpriteIndex("MIA");

	m->flags |= EF_IGNORE_BULLETS;

	/* blink at random intervals */
	m->spriteFrame = 0;
	m->spriteTime = rand() % 180;

	m->action = nothing;
	m->reset = reset;
	m->tick = tick;
	m->touch = touch;

	m->isMissionTarget = 1;
}

void reinitMIA(Entity *e)
{
	if (e->tx == -1 && e->ty == -1)
	{
		e->tx = e->x;
		e->ty = e->y;
	}
}

static void reset(void)
{
	self->tx = self->ty = -1;
}

static void tick(void)
{
	MIA *m;
	
	m = (MIA*)self;
	
	m->shudderTimer--;
	if (m->shudderTimer <= 0)
	{
		m->x = (m->tx + rand() % 4);
		m->shudderTimer = 2;
	}

	if (m->action != nothing)
	{
		m->starTimer--;
		if (m->starTimer <= 0)
		{
			addMIATeleportStars(m->x + rand() % m->w, m->y + rand() % m->h);
			m->starTimer = 1;
		}
	}
}

static void touch(Entity *other)
{
	MIA *m;
	
	m = (MIA*)self;
	
	if (m->action == nothing && other == (Entity*)world.bob)
	{
		m->action = preTeleport;
		m->teleportTimer = FPS * 3;
		setGameplayMessage(MSG_OBJECTIVE, _("Rescued %s"), m->name);
		m->isMissionTarget = 0;
		m->flags |= EF_ALWAYS_PROCESS;
		playSound(SND_MIA, CH_ANY);
	}
}

static void preTeleport(void)
{
	MIA *m;
	
	m = (MIA*)self;
	
	m->teleportTimer--;
	if (m->teleportTimer <= FPS)
	{
		m->action = teleport;
		m->flags |= (EF_NO_CLIP | EF_WEIGHTLESS);
		m->dy = -5;
	}
}

static void teleport(void)
{
	MIA *m;
	
	m = (MIA*)self;
	
	m->teleportTimer--;
	if (m->teleportTimer <= 0)
	{
		addTeleportStars(self);
		addRescuedMIA(m->name);
		updateObjective("MIA");
		m->alive = ALIVE_DEAD;
	}
}
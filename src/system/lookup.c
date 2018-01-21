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

#include "lookup.h"

static Lookup head;
static Lookup *tail;

static void addLookup(const char *name, long value); 

void initLookups(void)
{
	memset(&head, 0, sizeof(Lookup));
	tail = &head;
	
	addLookup("DUMMY", 0);
}

static void addLookup(const char *name, long value)
{
	Lookup *lookup = malloc(sizeof(Lookup));
	memset(lookup, 0, sizeof(Lookup));

	STRNCPY(lookup->name, name, MAX_NAME_LENGTH);
	lookup->value = value;

	tail->next = lookup;
	tail = lookup;
}

long lookup(const char *name)
{
	Lookup *l;

	for (l = head.next ; l != NULL ; l = l->next)
	{
		if (strcmp(l->name, name) == 0)
		{
			return l->value;
		}
	}

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "No such lookup value '%s'", name);
	exit(1);

	return 0;
}

char *getLookupName(const char *prefix, long num)
{
	Lookup *l;

	for (l = head.next ; l != NULL ; l = l->next)
	{
		if (l->value == num && strncmp(prefix, l->name, strlen(prefix)) == 0)
		{
			return l->name;
		}
	}

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "No such lookup value %ld, prefix=%s", num, prefix);
	exit(1);

	return "";
}

const char *getFlagValues(const char *prefix, long flags)
{
	static char flagStr[MAX_DESCRIPTION_LENGTH];
	int requirePlus;
	Lookup *l;

	memset(flagStr, '\0', MAX_DESCRIPTION_LENGTH);

	requirePlus = 0;

	for (l = head.next ; l != NULL ; l = l->next)
	{
		if (flags & l->value && strncmp(prefix, l->name, strlen(prefix)) == 0)
		{
			if (requirePlus)
			{
				strcat(flagStr, "+");
			}

			strcat(flagStr, l->name);

			requirePlus = 1;
		}
	}

	return flagStr;
}

long flagsToLong(const char *in)
{
	char *flag, *flags;
	long total;
	
	total = 0;
	
	if (strlen(in) > 0)
	{
		flags = malloc(strlen(in) + 1);
		STRNCPY(flags, in, strlen(in) + 1);

		flag = strtok(flags, "+");
		while (flag)
		{
			total += lookup(flag);
			flag = strtok(NULL, "+");
		}
		
		free(flags);
	}

	return total;
}

void destroyLookups(void)
{
	Lookup *l;

	while (head.next)
	{
		l = head.next;
		head.next = l->next;
		free(l);
	}
}

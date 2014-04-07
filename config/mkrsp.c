/* mkrsp.c -- response file generator for MSDOS compilers

   This file is part of the LZO real-time data compression library.

   Copyright (C) 1996 Markus Franz Xaver Johannes Oberhumer

   The LZO library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   The LZO library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the LZO library; see the file COPYING.
   If not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   Markus F.X.J. Oberhumer
   markus.oberhumer@jk.uni-linz.ac.at
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>
#include <assert.h>


/*************************************************************************
//
**************************************************************************/

enum {
	M_BCC,
	M_WCC,
};

int mode = M_BCC;


int main(int argc, char *argv[])
{
	int i;
	FILE *f = NULL;
	char *p;

	for (i = 1; i < argc && argv[i][0] == '-'; i++)
	{
		p = &argv[i][1];
		if (stricmp(p,"bcc") == 0)
			mode = M_BCC;
		else if (stricmp(p,"wcc") == 0)
			mode = M_WCC;
		else if (p[0] == 'o' && p[1] == 0)
		{
			if (f != NULL)
				fclose(f);
			p = argv[++i];
			if (p != NULL)
				f = fopen(p,"w");
		}
	}

	if (f == NULL)
		f = stdout;

	while (i < argc)
	{
		switch (mode)
		{
		case M_BCC:
			fprintf(f,"+%s%s\n",argv[i], i+1 < argc ? " &" : "");
			break;
		case M_WCC:
			fprintf(f,"+'%s'%s\n",argv[i], "");
			break;
		}
		i++;
	}

	fflush(f);
	if (f != stdout)
		fclose(f);

	return 0;
}

/*
vi:ts=4
*/


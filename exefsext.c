/* 
 * ExeFS Extractor
 * Copyright (C) 2016 FIX94
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <malloc.h>
#include "polarssl/sha2.h"
#define u64 unsigned long long
#define u32 unsigned int
#define u16 unsigned short
#define u8 unsigned char

#define S64 __builtin_bswap64
#define S32 __builtin_bswap32
#define S16 __builtin_bswap16

#define A64(x) ((x+0x3F)&(~0x3F))

int main()
{
	u8 *buf = NULL;
	FILE *f = fopen("exefs.bin","rb");
	if(!f)
	{
		puts("Could not open exefs.bin!");
		goto end;
	}
	fseek(f,0,SEEK_END);
	size_t fsize = ftell(f);
	buf = malloc(fsize);
	if(!buf)
	{
		printf("Unable to allocate %i bytes for exefs!\n",fsize);
		goto end;
	}
	fseek(f,0,SEEK_SET);
	fread(buf,fsize,1,f);
	fclose(f);
	mkdir("exefs");
	int i;
	u8 verSha[0x20];
	for(i = 0; i < 0x10; i++)
	{
		if(*(buf+(i*0x10)) == 0)
			break;
		u32 thisdatapos = *(u32*)(buf+(i*0x10)+0x8)+0x200;
		u32 thisdatasize = *(u32*)(buf+(i*0x10)+0xC);
		sha2(buf+thisdatapos,thisdatasize,verSha,0);
		if(memcmp(verSha,buf+0x1E0-(i*0x20), 0x20) == 0)
			printf("SHA2 of %s is valid!\n",buf+(i*0x10));
		else
			printf("WARNING:SHA2 of %s is invalid!\n",buf+(i*0x10));
		char name[32];
		sprintf(name,"exefs/%s",buf+(i*0x10));
		f = fopen(name,"wb");
		if(!f)
		{
			printf("Unable to write %s!\n",name);
			goto end;
		}
		fwrite(buf+thisdatapos,thisdatasize,1,f);
		fclose(f);
		printf("Wrote %s\n",name);
	}
end:
	if(buf) free(buf);
	if(f) fclose(f);
	return 0;
}
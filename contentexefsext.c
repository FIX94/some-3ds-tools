/* 
 * Content ExeFS Extractor
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
int main()
{
	u8 *buf = NULL;
	FILE *f = fopen("content.bin","rb");
	if(!f)
	{
		puts("Could not open content.bin!");
		goto end;
	}
	fseek(f,0,SEEK_END);
	size_t fsize = ftell(f);
	fseek(f,0,SEEK_SET);
	buf = malloc(fsize);
	if(!buf)
	{
		printf("Unable to allocate %i bytes for content!\n",fsize);
		goto end;
	}
	fread(buf,fsize,1,f);
	fclose(f);
	if(memcmp(buf+0x100,"NCCH",4) != 0)
	{
		puts("Content is not a NCCH!");
		goto end;
	}
	else if((*(buf+0x18F) & 4) == 0)
	{
		puts("Content is encrypted, please decrypt it with Decrypt9WIP!");
		goto end;
	}
	size_t exefspos = *(u32*)(buf+0x1A0)*0x200;
	size_t exefssize = *(u32*)(buf+0x1A4)*0x200;
	size_t exefsHashSize = *(u32*)(buf+0x1A8)*0x200;
	printf("ExeFS Pos:%x,Size:%x,Hash Size:%x\n",exefspos,exefssize, exefsHashSize);
	u8 verSha[0x20];
	sha2(buf+exefspos,exefsHashSize,verSha,0);
	if(memcmp(verSha,buf+0x1C0, 0x20) == 0)
		puts("SHA2 of ExeFS is valid!");
	else
		puts("WARNING:Invalid ExeFS SHA2!");
	f = fopen("exefs.bin","wb");
	if(!f)
	{
		puts("Could not open exefs.bin!");
		goto end;
	}
	fwrite(buf+exefspos,exefssize,1,f);
	fclose(f);
	puts("Wrote exefs.bin");
end:
	if(f) fclose(f);
	if(buf) free(buf);
	return 0;
}
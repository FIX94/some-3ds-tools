/* 
 * CIA Content Extractor
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

typedef enum {
	RSA_2048_SHA256	= 0x00010004,
	RSA_4096_SHA256	= 0x00010003,
} sigtype;

int main()
{
	u8 *buf = NULL;
	FILE *f = fopen("title.cia","rb");
	if(!f)
	{
		puts("Unable to open title.cia!");
		goto end;
	}
	fseek(f,0,SEEK_END);
	size_t fsize = ftell(f);
	fseek(f,0,SEEK_SET);
	buf = malloc(fsize);
	if(!buf)
	{
		printf("Unable to allocate %i bytes!\n",fsize);
		goto end;
	}
	fread(buf,1,fsize,f);
	fclose(f);
	f = NULL;
	u32 certsize = A64(*(u32*)(buf+0x8)), tiksize=A64(*(u32*)(buf+0xC)), tmdsize=A64(*(u32*)(buf+0x10)),
		metasize=A64(*(u32*)(buf+0x14)), contentsize=A64(*(u32*)(buf+0x18));
	printf("Cert Size:%x, Tik Size: %x, TMD Size: %x\nMeta Size: %x, Total Content Size:%x\n",
		certsize,tiksize,tmdsize,metasize,contentsize);
	u32 tmdstart = 0x2040+certsize+tiksize;
	printf("TMD Start: %x\n",tmdstart);
	u32 sigtype = S32(*(u32*)(buf+tmdstart));
	u32 tmdsigsize = 0x100;

	if(sigtype != RSA_2048_SHA256)
	{
		printf("Signature Type not supported:0x%08x\n",sigtype);
		goto end;
	}

	u32 tmdhdr = A64(tmdstart+4+tmdsigsize);
	u32 tmdverpos = tmdhdr+0x40;
	int tmdver = *(buf+tmdverpos);
	if(tmdver != 1)
	{
		printf("Unknown tmd ver:%i\n",tmdver);
		goto end;
	}
	printf("Title ID: %016I64x\n",S64(*(u64*)(buf+tmdhdr+0x4C)));
	u32 numcontentpos = tmdhdr+0x9E;
	u16 numcontents = S16(*(u16*)(buf+numcontentpos));
	printf("Num Contents: %i\n",numcontents);
	u32 infoshapos = numcontentpos+6; //has sha1 of 0x900 inforecords
	u32 inforecords = infoshapos+0x20; //has sha1 of each chunkrecord
	u8 verSha[0x20];
	sha2(buf+inforecords,(0x40*0x24),verSha,0);
	if(memcmp(verSha,buf+infoshapos, 0x20) == 0)
		puts("SHA2 of Content Info Records is valid!");
	else
		puts("WARNING:Invalid Content Info Records SHA2!");
	u32 chunkrecords = inforecords+(0x40*0x24); //has sha1 of each content
	sha2(buf+chunkrecords,0x30,verSha,0);
	if(memcmp(verSha,buf+inforecords+4, 0x20) == 0)
		puts("SHA2 of Content Chunk Records is valid!");
	else
		puts("WARNING:Invalid Content Chunk Records SHA2!");
	u32 chunkrecordsSize = chunkrecords+(numcontents*0x30);
	u32 content = A64(chunkrecordsSize);
	printf("Content starts at 0x%x\n",content);
	int i;
	for(i = 0; i < numcontents; i++)
	{
		u64 thisContentSize = S64(*(u64*)(buf+(chunkrecords+(i*0x30))+0x8));
		printf("Content %x Size: %I64x\n", 
			S16(*(u16*)(buf+(chunkrecords+(i*0x30))+0x4)),
			thisContentSize);
		sha2(buf+content,(u32)thisContentSize,verSha,0);
		if(memcmp(verSha,buf+(chunkrecords+(i*0x30))+0x10, 0x20) == 0)
			puts("SHA2 of this Content is valid!");
		else
			puts("WARNING:SHA2 of this Content is invalid!");
	}
	f = fopen("content.bin","wb");
	fwrite(buf+content,contentsize,1,f);
	fclose(f);
	puts("Wrote content.bin");
end:
	if(buf) free(buf);
	if(f) fclose(f);
	return 0;
}
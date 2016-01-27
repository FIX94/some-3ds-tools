/* 
 * ExeFS Packer
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
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <malloc.h>
#include "polarssl/sha2.h"
#define u64 unsigned long long
#define u32 unsigned int
#define u16 unsigned short
#define u8 unsigned char

#define ALIGN256(x) ((x+0x1FF)&(~0x1FF))

static void write_align(u32 size, FILE *f)
{
	size_t align_missing = (ALIGN256(size) - size);
	if(align_missing == 0)
		return;
	char nothin[1];
	nothin[0] = 0x00;

	size_t i;
	for(i = 0; i < align_missing; ++i)
		fwrite(nothin, 1, 1, f);
}

int isfile(const char *path)
{
	struct stat path_stat;
	stat(path, &path_stat);
	return S_ISREG(path_stat.st_mode);
}

int main()
{
	u8 *buf = NULL;
	FILE *f = NULL;
	char fnames[8][10];
	memset((void*)fnames,0,8*10);
	size_t foff[10], fsize[10];
	memset(foff,0,sizeof(size_t)*10);
	memset(fsize,0,sizeof(size_t)*10);
	unsigned char fhashes[0x20][10];
	memset(fhashes,0,0x20*10);
	int fentries = 0;
	DIR *dp = NULL;
	struct dirent *ep;

	dp = opendir("exefs");
	if (!dp)
	{
		puts("No exefs dir!");
		goto end;
	}
	while((ep = readdir (dp)))
	{
		//security check 1
		if(memcmp(ep->d_name,".",2) == 0 ||
			memcmp(ep->d_name,"..",3) == 0 ||
			strlen(ep->d_name) > 7) continue;
		//security check 2
		char path[32];
		sprintf(path,"exefs/%s",ep->d_name);
		if(!isfile(path)) continue;
		//all good, lets add entry
		memcpy(fnames[fentries],ep->d_name,strlen(ep->d_name));
		fentries++;
		if(fentries > 9) break;
	}
	closedir(dp);

	char exefsHdr[0x200];
	memset(exefsHdr,0,0x200);
	f = fopen("exefs.bin","wb+");
	if(!f)
	{
		puts("Unable to write exefs.bin!");
		goto end;
	}
	fwrite(exefsHdr,0x200,1,f);
	size_t curoffset = 0;
	int i;
	for(i = 0; i < fentries; i++)
	{
		char name[32];
		sprintf(name,"exefs/%s",fnames[i]);
		FILE *fr = fopen(name,"rb");
		if(!fr)
		{
			printf("Unable to open %s!\n",name);
			continue;
		}
		fseek(fr,0,SEEK_END);
		fsize[i] = ftell(fr);
		fseek(fr,0,SEEK_SET);
		buf = malloc(fsize[i]);
		if(!buf)
		{
			printf("Unable to allocated %i bytes!\n",fsize[i]);
			fclose(fr);
			goto end;
		}
		printf("Adding %s with %i bytes\n",name,fsize[i]);
		fread(buf,fsize[i],1,fr);
		fclose(fr);
		sha2(buf,fsize[i],fhashes[i],0);
		fwrite(buf,fsize[i],1,f);
		write_align(fsize[i],f);
		foff[i] = curoffset;
		curoffset += ALIGN256(fsize[i]);
		memcpy(exefsHdr+(i*16),fnames[i],strlen(fnames[i]));
		memcpy(exefsHdr+(i*16)+0x8,&(foff[i]),4);
		memcpy(exefsHdr+(i*16)+0xC,&(fsize[i]),4);
		memcpy(exefsHdr+0x1E0-(i*0x20),fhashes[i],0x20);
	}
	fseek(f,0,SEEK_SET);
	puts("Writing updated header");
	fwrite(exefsHdr,0x200,1,f);
	puts("Wrote exefs.bin");
end:
	if(f) fclose(f);
	if(buf) free(buf);
	return 0;
}

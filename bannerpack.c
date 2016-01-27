/* 
 * Banner Packer
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
#define u64 unsigned long long
#define u32 unsigned int
#define u16 unsigned short
#define u8 unsigned char

extern size_t lz11_compress(u8 *indata, size_t inLength, u8 **out);

#define A16(x) ((x+0xF)&(~0xF))
static void write_align(u32 size, FILE *f)
{
	size_t align_missing = (A16(size) - size);
	if(align_missing == 0)
		return;
	char nothin[1];
	nothin[0] = 0x00;

	size_t i;
	for(i = 0; i < align_missing; ++i)
		fwrite(nothin, 1, 1, f);
}

int main()
{
	u8 *cbmd = NULL, *cgfx = NULL, *cmpcgfx = NULL, *cwav = NULL;
	FILE *f = NULL;
	FILE *banner = fopen("exefs/banner","wb");
	if(!banner)
	{
		puts("Unable to write banner!");
		goto end;
	}
	f = fopen("exefs/bannerext/banner.cbmd","rb");
	if(!f)
	{
		puts("Unable to open banner.cbmd!");
		goto end;
	}
	fseek(f,0,SEEK_END);
	size_t cbmdsize = ftell(f);
	fseek(f,0,SEEK_SET);
	cbmd = malloc(cbmdsize);
	if(!cbmd)
	{
		printf("Unable to allocated %i bytes!\n",cbmdsize);
		goto end;
	}
	fread(cbmd,cbmdsize,1,f);
	fclose(f);
	if(cbmdsize != 0x88 || memcmp(cbmd,"CBMD",4) != 0)
	{
		puts("Not a CBMD!");
		goto end;
	}
	f = fopen("exefs/bannerext/banner.cgfx","rb");
	if(!f)
	{
		puts("Unable to open banner.cgfx!");
		goto end;
	}
	fseek(f,0,SEEK_END);
	size_t cgfxsize = ftell(f);
	fseek(f,0,SEEK_SET);
	cgfx = malloc(cgfxsize);
	if(!cgfx)
	{
		printf("Unable to allocated %i bytes!\n",cgfxsize);
		goto end;
	}
	fread(cgfx,cgfxsize,1,f);
	fclose(f);
	if(memcmp(cgfx,"CGFX",4) != 0)
	{
		puts("Not a CGFX!");
		goto end;
	}
	size_t cmpcgfxsize = lz11_compress(cgfx,cgfxsize,&cmpcgfx);
	if(cmpcgfxsize == 0 || !cmpcgfx)
	{
		puts("Failed to compress CGFX!");
		goto end;
	}
	f = fopen("exefs/bannerext/banner.bcwav","rb");
	if(!f)
	{
		puts("Unable to open banner.bcwav!");
		goto end;
	}
	fseek(f,0,SEEK_END);
	size_t cwavsize = ftell(f);
	fseek(f,0,SEEK_SET);
	cwav = malloc(cwavsize);
	if(!cwav)
	{
		printf("Unable to allocated %i bytes!\n",cgfxsize);
		goto end;
	}
	fread(cwav,cwavsize,1,f);
	fclose(f);
	size_t raw_cwav_offset = 0x88+cmpcgfxsize;
	//update cwav offset
	*(u32*)(cbmd+0x84) = A16(raw_cwav_offset);
	printf("New CWAV Offset:%x\n",*(u32*)(cbmd+0x84));
	fwrite(cbmd,0x88,1,banner);
	printf("Wrote CBMD with %i bytes\n",0x88);
	fwrite(cmpcgfx,cmpcgfxsize,1,banner);
	printf("Wrote Compressed CGFX with %i bytes\n",cmpcgfxsize);
	write_align(raw_cwav_offset,banner);
	fwrite(cwav,cwavsize,1,banner);
	printf("Wrote CWAV with %i bytes\n",cwavsize);
end:
	if(cbmd) free(cbmd);
	if(cgfx) free(cgfx);
	if(cmpcgfx) free(cmpcgfx);
	if(cwav) free(cwav);
	if(banner) fclose(banner);
	if(f) fclose(f);
	return 0;
}

/* 
 * Banner Extractor
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

extern size_t lz11_decompress(u8 *indata, size_t inLength, u8 **out);

int main()
{
	u8 *buf = NULL, *out = NULL;
	FILE *f = fopen("exefs/banner","rb");
	if(!f)
	{
		puts("Unable to open banner!");
		goto end;
	}
	fseek(f,0,SEEK_END);
	size_t fsize = ftell(f);
	fseek(f,0,SEEK_SET);
	buf = malloc(fsize);
	if(!buf)
	{
		printf("Unable to allocated %i bytes!\n",fsize);
		goto end;
	}
	fread(buf,fsize,1,f);
	fclose(f);
	if(memcmp(buf,"CBMD",4) != 0)
	{
		puts("Not a CBMD!");
		goto end;
	}
	size_t cwavOffset = (*(u32*)(buf+0x84));
	if((cwavOffset+4 > fsize) || (memcmp(buf+cwavOffset,"CWAV",4) != 0))
	{
		puts("Not a CWAV!");
		goto end;
	}
	size_t decmpSize = lz11_decompress(buf+0x88,cwavOffset-0x88,&out);
	if(!out || decmpSize == 0 || memcmp(out,"CGFX",4) != 0)
	{
		puts("Not a CGFX!");
		goto end;
	}
	mkdir("exefs/bannerext");
	f = fopen("exefs/bannerext/banner.cbmd","wb");
	if(!f)
	{
		puts("Unable to write banner.cbmd!");
		goto end;
	}
	fwrite(buf,0x88,1,f);
	fclose(f);
	printf("Wrote banner.cbmd with %i bytes\n",0x88);
	f = fopen("exefs/bannerext/banner.cgfx","wb");
	if(!f)
	{
		puts("Unable to write banner.cgfx!");
		goto end;
	}
	fwrite(out,decmpSize,1,f);
	fclose(f);
	printf("Wrote banner.cgfx with %i bytes\n",decmpSize);
	f = fopen("exefs/bannerext/banner.bcwav","wb");
	if(!f)
	{
		puts("Unable to write banner.bcwav!");
		goto end;
	}
	fwrite(buf+cwavOffset,fsize-cwavOffset,1,f);
	fclose(f);
	printf("Wrote banner.bcwav with %i bytes\n",fsize-cwavOffset);
end:
	if(f) fclose(f);
	if(buf) free(buf);
	if(out) free(out);
	return 0;
}

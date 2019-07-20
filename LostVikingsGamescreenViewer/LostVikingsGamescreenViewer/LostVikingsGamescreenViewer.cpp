// LostVikingsGamescreenViewer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "LostVikingsGamescreenViewer.h"


#define METATILE_WIDTH 16
#define METATILE_HEIGHT 16
#define TILEMAP_MAX_WIDTH 256
#define TILEMAP_MAX_HEIGHT 256

//Make space for 256 x 256 metatiles, each 16x16 (16 MB).
#define MAX_PIXELS (TILEMAP_MAX_WIDTH * METATILE_WIDTH * TILEMAP_MAX_HEIGHT * METATILE_HEIGHT)

int bitmapWidth = 512;
int bitmapHeight = 512;
uint8_t redData[MAX_PIXELS];
uint8_t greenData[MAX_PIXELS];
uint8_t blueData[MAX_PIXELS];

//uint8_t palette[256][3];
uint8_t palette[256*3];

void setPixel(int x, int y, int color)
{
	redData[x + y * bitmapWidth] = palette[color*3] * 4;
	greenData[x + y * bitmapWidth] = palette[color*3 + 1] * 4;
	blueData[x + y * bitmapWidth] = palette[color*3 + 2] * 4;
}

//https://en.wikipedia.org/wiki/User:Evercat/Buddhabrot.c
void drawbmp(const char* filename) {
	unsigned int headers[13];
	FILE* outfile;
	int extrabytes;
	int paddedsize;
	int x; int y; int n;
	int red, green, blue;

	extrabytes = 4 - ((bitmapWidth * 3) % 4);                 // How many bytes of padding to add to each
														// horizontal line - the size of which must
														// be a multiple of 4 bytes.
	if (extrabytes == 4)
		extrabytes = 0;

	paddedsize = ((bitmapWidth * 3) + extrabytes) * bitmapHeight;

	// Headers...
	// Note that the "BM" identifier in bytes 0 and 1 is NOT included in these "headers".

	headers[0] = paddedsize + 54;      // bfSize (whole file size)
	headers[1] = 0;                    // bfReserved (both)
	headers[2] = 54;                   // bfOffbits
	headers[3] = 40;                   // biSize
	headers[4] = bitmapWidth;  // biWidth
	headers[5] = bitmapHeight; // biHeight

	// Would have biPlanes and biBitCount in position 6, but they're shorts.
	// It's easier to write them out separately (see below) than pretend
	// they're a single int, especially with endian issues...

	headers[7] = 0;                    // biCompression
	headers[8] = paddedsize;           // biSizeImage
	headers[9] = 0;                    // biXPelsPerMeter
	headers[10] = 0;                    // biYPelsPerMeter
	headers[11] = 0;                    // biClrUsed
	headers[12] = 0;                    // biClrImportant

	outfile = fopen(filename, "wb");

	//
	// Headers begin...
	// When printing ints and shorts, we write out 1 character at a time to avoid endian issues.
	//

	fprintf(outfile, "BM");

	for (n = 0; n <= 5; n++)
	{
		fprintf(outfile, "%c", headers[n] & 0x000000FF);
		fprintf(outfile, "%c", (headers[n] & 0x0000FF00) >> 8);
		fprintf(outfile, "%c", (headers[n] & 0x00FF0000) >> 16);
		fprintf(outfile, "%c", (headers[n] & (unsigned int)0xFF000000) >> 24);
	}

	// These next 4 characters are for the biPlanes and biBitCount fields.

	fprintf(outfile, "%c", 1);
	fprintf(outfile, "%c", 0);
	fprintf(outfile, "%c", 24);
	fprintf(outfile, "%c", 0);

	for (n = 7; n <= 12; n++)
	{
		fprintf(outfile, "%c", headers[n] & 0x000000FF);
		fprintf(outfile, "%c", (headers[n] & 0x0000FF00) >> 8);
		fprintf(outfile, "%c", (headers[n] & 0x00FF0000) >> 16);
		fprintf(outfile, "%c", (headers[n] & (unsigned int)0xFF000000) >> 24);
	}

	//
	// Headers done, now write the data...
	//

	for (y = bitmapHeight - 1; y >= 0; y--)     // BMP image format is written from bottom to top...
	{
		for (x = 0; x <= bitmapWidth - 1; x++)
		{

			fprintf(outfile, "%c", blueData[x + y * bitmapWidth]);
			fprintf(outfile, "%c", greenData[x + y * bitmapWidth]);
			fprintf(outfile, "%c", redData[x + y * bitmapWidth]);
		}
		if (extrabytes)      // See above - BMP lines must be of lengths divisible by 4.
		{
			for (n = 1; n <= extrabytes; n++)
			{
fprintf(outfile, "%c", 0);
			}
		}
	}

	fclose(outfile);
	return;
}

const int GAMESCREEN_COUNT = 16;

const int gamescreenChunkIndexRaw[] = { 0xC6, 0x00, 0xC8, 0x00, 0xCA, 0x00, 0xCC, 0x00, 0x28, 0x00, 0x2A, 0x00, 0x2C, 0x00, 0x2E, 0x00,
0x30, 0x00, 0x32, 0x00, 0x34, 0x00, 0x4B, 0x00, 0x4D, 0x00, 0x4F, 0x00, 0x51, 0x00, 0x53, 0x00,
0x55, 0x00, 0x72, 0x00, 0x74, 0x00, 0x76, 0x00, 0x78, 0x00, 0x7A, 0x00, 0x7C, 0x00, 0x7E, 0x00,
0x80, 0x00, 0x9C, 0x00, 0x9E, 0x00, 0xA0, 0x00, 0xA2, 0x00, 0xA4, 0x00, 0xA6, 0x00, 0xA8, 0x00,
0xAA, 0x00, 0xCE, 0x00, 0xD0, 0x00, 0xD2, 0x00, 0xD4, 0x00, 0x71, 0x01, 0x7D, 0x01, 0x86, 0x01,
0x8C, 0x01, 0x92, 0x01, 0x7E, 0x01, 0xAF, 0x01, 0xB0, 0x01, 0xB1, 0x01, 0xB2, 0x01, 0xDA, 0x00 };

int getGamescreenChunkIndex(int index)
{
	return (gamescreenChunkIndexRaw[index * 2 + 1] << 8) + gamescreenChunkIndexRaw[index * 2];
}

int loadChunk(int index, uint8_t* out)
{
	uint8_t* p = out;
	char filename[300];
	sprintf(filename, "C:\\dosbox\\lostvik\\RAW%03d.BIN", index);
	FILE* fData = fopen(filename, "rb");
	while (!feof(fData))
	{
		fread(p++, 1, 1, fData);
	}
	fclose(fData);
	return p - out - 1;
}


void drawTile(uint8_t* data, int x, int y, bool horizontalFlip, bool verticalFlip)
{
	for (int j = 0; j < 64; ++j)
	{
		int tileX = (j / 16) + ((j & 1) ? 4 : 0);
		if (horizontalFlip) tileX = 7 - tileX;
		int tileY = (j / 2) % 8;
		if (verticalFlip) tileY = 7 - tileY;
		setPixel(x + tileX, y + tileY, *data++);
	}
}

void drawMetatile(uint8_t* metatileData, uint8_t* tilePixels, int tileX, int tileY)
{
	int i = 0;
	for (int y = 0; y < 2; ++y)
	{
		for (int x = 0; x < 2; ++x)
		{
			int tile = ((int)metatileData[i + 1] << 8) + metatileData[i];
			bool horizontalFlip = tile & 0x0010;
			bool verticalFlip = tile & 0x0020;
			//0-2: Unknown. 3: Solid (?). 4: HorizontalFlip. 5: VerticalFlip
			tile >>= 6; //Unsure what all the bits are
			drawTile(tilePixels + tile * 64, tileX + x * 8, tileY + y * 8, horizontalFlip, verticalFlip);
			i += 2;
		}
	}
}

void clearImage()
{
	int i;
	for (i = 0; i < bitmapWidth; ++i)
	{
		for (int j = 0; j < bitmapHeight; ++j)
		{
			redData[i + j * bitmapWidth] = 0;
			greenData[i + j * bitmapWidth] = 0;
			blueData[i + j * bitmapWidth] = 0;
		}
	}
}

void dumpTilesToFile(uint8_t* data, int len, char* filename)
{
	int i;

	bitmapWidth = 512;
	bitmapHeight = 512;
	clearImage();

	int tileOffsetX = 0;
	int tileOffsetY = 0;
	i = 0;
	while (i < len)
	{
		//Read one tile, 8 x 8
		drawTile(data + i, tileOffsetX, tileOffsetY, false, false);
		i += 64;

		tileOffsetX += 8;
		if (tileOffsetX >= bitmapWidth)
		{
			tileOffsetX = 0;
			tileOffsetY += 8;
		}

	}
	drawbmp(filename);
}

void dumpMetaTilesToFile(uint8_t* metatileData, int metatileDataLen, uint8_t* tilePixels, int tilePixelsLen, char* filename)
{
	int i = 0;

	bitmapWidth = 512;
	bitmapHeight = 512;
	clearImage();

	int tileOffsetX = 0;
	int tileOffsetY = 0;

	while (i < metatileDataLen)
	{
		drawMetatile(metatileData + i, tilePixels, tileOffsetX, tileOffsetY);
		i += 8;

		tileOffsetX += 16;
		if (tileOffsetX >= bitmapWidth)
		{
			tileOffsetX = 0;
			tileOffsetY += 16;
		}

	}
	drawbmp(filename);
}


void dumpTilemapToFile(uint8_t* tilemapData, int tilemapDataLen, int tilemapWidth, uint8_t* metatileData, int metatileDataLen, uint8_t* tilePixels, int tilePixelsLen, char* filename)
{
	int i = 0;

	int tilemapHeight = tilemapDataLen / 2 / tilemapWidth;

	bitmapWidth = tilemapWidth * METATILE_WIDTH;
	bitmapHeight = tilemapHeight * METATILE_HEIGHT;
	clearImage();

	int metatileOffsetX = 0;
	int metatileOffsetY = 0;

	while (i < tilemapDataLen)
	{
		int metatile = ((int)tilemapData[i + 1] << 8) + tilemapData[i];
		metatile &= 0x01FF; //Unsure what all the bits are
		drawMetatile(metatileData + metatile * 8, tilePixels, metatileOffsetX * 16, metatileOffsetY * 16);
		i += 2;

		metatileOffsetX++;
		if (metatileOffsetX >= tilemapWidth)
		{
			metatileOffsetX = 0;
			metatileOffsetY++;
		}
	}
	drawbmp(filename);
}

void dumpGamescreenByIndex(int index)
{
	int chunkId;
	uint8_t headerData[5000];
	int headerLen;
	uint8_t buf[5000];
	int bufLen;

	uint8_t tilePixels[50000];
	uint8_t metatileData[10000];
	uint8_t tilemapData[20000];

	printf("\nExtracting gamescreen %03d", index);

	chunkId = getGamescreenChunkIndex(index);
	printf(" datachunk %03d", chunkId);

	headerLen = loadChunk(chunkId, headerData);

	//1: Load palette
	int i = 67;
	while (i < headerLen)
	{
		if (headerData[i] == 0xFF && headerData[i + 1] == 0xFF) break;
		i += 14;
	}
	i += 2;
	while (i < headerLen)
	{
		if (headerData[i] == 0xFF && headerData[i + 1] == 0xFF) break;
		int paletteChunkIndex = ((int)headerData[i + 1] << 8) + headerData[i];
		int offset = headerData[i + 2];
		loadChunk(paletteChunkIndex, palette + offset * 3);
		i += 3;
	}

	//2: Load tiles
	int tileChunkIndex = ((int)headerData[49] << 8) + headerData[48];
	printf(" tilechunk %03d", tileChunkIndex);

	if (tileChunkIndex == 0xFFFF) return; //screen 38
	int tilePixelsLen = loadChunk(tileChunkIndex, tilePixels);

	//3: Dump tiles to file
	char filename[300];
	sprintf(filename, "C:\\dosbox\\lostvik\\GAMESCREEN%03d_TILES.bmp", index);
	dumpTilesToFile(tilePixels, tilePixelsLen, filename);

	//FILE* fData = fopen("C:\\dosbox\\lostvik\\RAW388.BIN", "rb");
	
	//4: Load metatiles
	int metatileChunkIndex = ((int)headerData[51] << 8) + headerData[50];
	printf(" metatiletilechunk %03d", metatileChunkIndex);

	if (tileChunkIndex == 0xFFFF) return; //screen 38
	int metatileDataLen = loadChunk(metatileChunkIndex, metatileData);

	//5: Dump metatiles to file
	//char filename[300];
	sprintf(filename, "C:\\dosbox\\lostvik\\GAMESCREEN%03d_METATILES.bmp", index);
	dumpMetaTilesToFile(metatileData, metatileDataLen, tilePixels, tilePixelsLen, filename);
	
	//6: Load tilemap
	int tilemapChunkIndex = ((int)headerData[47] << 8) + headerData[46];
	printf(" tilemapchunk %03d", tilemapChunkIndex);

	if (tilemapChunkIndex == 0xFFFF) return;
	int tilemapDataLen = loadChunk(tilemapChunkIndex, tilemapData);
	printf(" len %03d", tilemapDataLen);

	//7: Dump full tilemap to file
	//char filename[300];
	sprintf(filename, "C:\\dosbox\\lostvik\\GAMESCREEN%03d_TILEMAP.bmp", index);

	//int tilemapWidth = 256; //20 for intro images
	int tilemapWidth = headerData[41];

	dumpTilemapToFile(tilemapData, tilemapDataLen, tilemapWidth, metatileData, metatileDataLen, tilePixels, tilePixelsLen, filename);

	//printf("Extracting gamescreen %03d\n", i);
}


int main()
{
	//std::cout << "Hello World!\n";
	for (int i = 0; i < 48; ++i)
	{
		dumpGamescreenByIndex(i);
	}
}




// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file


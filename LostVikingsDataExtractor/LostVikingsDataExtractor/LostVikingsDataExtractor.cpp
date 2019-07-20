// LostVikingsDataExtractor.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

void decompressData(FILE* fIn, FILE* fOut, int uncompressedLen)
{
	uint8_t dictionary[0x1000] = { 0 }; //4096 bytes
	int dictPos = 0;

	uint8_t buf[10];
	int bitsLeftInMap = 0;
	uint8_t compressBitmap = 0;
	while (uncompressedLen >= 0)
	{
		if (bitsLeftInMap <= 0)
		{
			fread(buf, 1, 1, fIn);
			compressBitmap = buf[0];
			bitsLeftInMap = 8;
			//printf("Fetching bitmap %#04x\n", compressBitmap);
		}

		int type = compressBitmap & 1;
		compressBitmap >>= 1;
		bitsLeftInMap--;

		if (type == 1) //raw
		{
			fread(buf, 1, 1, fIn);
			fwrite(buf, 1, 1, fOut);
			dictionary[dictPos++] = buf[0];
			dictPos &= 0x0FFF;
			uncompressedLen--;
			//printf("Raw %#04x\n", buf[0]);
		}
		else
		{
			//Dictionary lookup
			fread(buf, 1, 2, fIn);
			uint16_t dictLookup = buf[0] + (buf[1] << 8);

			int offset = dictLookup & 0x0FFF;
			int len = (dictLookup >> 12) + 3;

			//printf("Lookup %#06x offset %#06x len %d\n", dictLookup, offset, len);


			while (len--)
			{
				buf[0] = dictionary[offset++];
				offset &= 0x0FFF;
				fwrite(buf, 1, 1, fOut);
				dictionary[dictPos++] = buf[0];
				dictPos &= 0x0FFF;
				uncompressedLen--;
				//printf("Dict %#04x\n", buf[0]);

			}

		}
	}
}


int main()
{
    std::cout << "Hello World!\n";

	const int DATA_COUNT = 535; //Number of elements

	FILE* f = fopen("C:\\dosbox\\lostvik\\DATA.DAT", "rb");

	for (int i = 0; i < DATA_COUNT; ++i)
	{
		fseek(f, 4 * i, SEEK_SET);

		uint8_t buf[10];
		fread(buf, 1, 4, f);
		uint32_t offset = buf[0] + (buf[1] << 8) + (buf[2] << 16) + (buf[3] << 24);

		fseek(f, offset, SEEK_SET);
		fread(buf, 1, 2, f);
		uint16_t len = buf[0] + (buf[1] << 8);

		char filename[300];
		sprintf(filename, "C:\\dosbox\\lostvik\\RAW%03d.BIN", i);

		printf("Element %03d offset %#010x len %05d file %s\n", i, offset, len, filename);

		FILE* fOut = fopen(filename, "wb");

		decompressData(f, fOut, len);

		fclose(fOut);
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

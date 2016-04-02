#pragma once
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

class ByteReader;
class ByteWriter;

struct BMP_file_header
{
	//As struct will auto fill in the byte to 4 bytes, we need 2 rubbish chars.
	unsigned char bitfillrubbish[2] = { 0 ,0 };
	unsigned char fileidentifier[2];
	unsigned int filesize;
	unsigned int reserved;
	unsigned int offset;
	unsigned int headerlength;
	unsigned int width;
	unsigned int height;
	unsigned short frames;
	unsigned short colordigit;
	unsigned int compressed;
	unsigned int bmpsize;
	unsigned int wres;
	unsigned int hres;
	unsigned int colorindex;
	unsigned int importantindex;
	void ShowInfo()
	{
		cout << "FileSize : " << dec << filesize << endl;
		cout << "Offset : " << offset << endl;
		cout << "HeaderLength : " << headerlength << endl;
		cout << "Width : " << width << endl;
		cout << "Height : " << height << endl;
		cout << "Frames : " << frames << endl;
		cout << "DigitPerColor : " << colordigit << endl;
		cout << "Compressed : " << compressed << endl;
		cout << "BMP size : " << bmpsize << endl;
		cout << "Width Resolution : " << wres << endl;
		cout << "Height Resolution : " << hres << endl;
		cout << "Color Index : " << colorindex << endl;
		cout << "ImportantIndex : " << importantindex << endl;
	}
};
typedef struct BMP_file_header BMPheader;
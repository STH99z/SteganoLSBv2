#include <iostream>
#include <iomanip>
#include <fstream>
#include "main.h"

using namespace std;

class ByteReader
{
public:
	const bool OpenFile(const char* filepath)
	{
		file = new ifstream(filepath, ifstream::binary);
		return file->is_open();
	}
	const bool IsOpened()
	{
		return file->is_open();
	}
	const bool isEOF()
	{
		return file->eof();
	}
	const unsigned char GetByte();
	const unsigned int GetBit2Int();
	const unsigned char GetBit2Char();
	const unsigned char GetMovedBit(const unsigned short);
	const bool GetBit(const unsigned short);
	const unsigned long GetFilePos()
	{
		return file->tellg();
	}
	const unsigned long GetFileLength()
	{
		unsigned int p = file->tellg();
		file->seekg(0, ifstream::end);
		unsigned int l = file->tellg();
		file->seekg(p);
		return l;
	}
	void PrintByte_Bin();
	void PrintByte(unsigned char delim = ' ');
	void ReadBMPheader(BMPheader*);

	ByteReader()
	{
		curbyte = 0;
		file = NULL;
	}
	ByteReader(const char* filepath)
	{
		curbyte = 0;
		OpenFile(filepath);
	}
	~ByteReader()
	{
		if (file&&IsOpened())
		{
			file->close();
		}
		delete file;
	}

private:
	ifstream* file;
	unsigned char curbyte;
};
class ByteWriter
{
public:
	const bool isOpened()
	{
		return file->is_open();
	}
	const bool PushMovedBit(const unsigned char movedbyte);
	const bool PushByte(const unsigned char byte)
	{
		*file << byte;
		return true;
	}
	const bool PushSteganoData(ByteReader& byteprovider,unsigned char* dataaddress,unsigned short datalength);
	const bool writeBMPheader(const BMPheader* p);
	const bool OpenFile(const char* filepath)
	{
		file = new ofstream(filepath, ofstream::binary);
		return file->is_open();
	}
	static const unsigned char WriteBitIntoByte(
		unsigned char& ch, 
		const unsigned char bit,
		const unsigned short digit);
	ByteWriter(const char* filepath)
	{
		bitcount = 0;
		curbyte = 0;
		OpenFile(filepath);
	}
	ByteWriter()
	{
		file = NULL;
		bitcount = 0;
		curbyte = 0;
	}
	~ByteWriter()
	{
		if (file&&isOpened())
		{
			file->flush();
			file->close();
		}
		delete file;
	}
private:
	ofstream* file;
	unsigned short bitcount;
	unsigned char curbyte;
};
const bool ByteWriter::writeBMPheader(const BMPheader* p)
{
	unsigned char *c = (unsigned char*)p + 2;
	for (auto i = 0; i < 54; i++)
		*file << *(c + i);
	return true;
}
const bool ByteWriter::PushMovedBit(const unsigned char byte)
{
	curbyte += byte << bitcount;
	bitcount++;
	if (bitcount == 8)
	{
		*file << curbyte;
		bitcount = 0;
		curbyte = 0;
		return true;
	}
	return false;
}
const unsigned char ByteWriter::WriteBitIntoByte(
	unsigned char& ch, 
	const unsigned char bit,
	const unsigned short digit)
{
	//if ((ch&(1<<digit)>0)&bit)
	if (((ch&(1 << digit)) > 0)^bit)
	{
		if (bit)
			ch += bit << digit;
		else
			ch -= 1 << digit;
	}
	return ch;
}
const bool ByteWriter::PushSteganoData(
	ByteReader& byteprovider, 
	unsigned char* dataaddress, 
	unsigned short datalength)
{
	unsigned char writing;
	for (auto i = 0; i < datalength; i++)
	{
		for (auto j = 0; j < 8; j++)
		{
			writing = byteprovider.GetByte();
			ByteWriter::WriteBitIntoByte(
				writing,
				(dataaddress[i]&(1 << j))>0,
				0);
			PushByte(writing);
		}
	}
	return true;
}

const unsigned char ByteReader::GetBit2Char()
{
	unsigned char c = 0;
	for (auto j = 0; j < 8; j++)
	{
		GetByte();
		c += GetBit(0) << j;
	}
	return c;
}
const unsigned int ByteReader::GetBit2Int()
{
	unsigned int r=0;
	unsigned char b=0;
	for (auto i = 0; i < 4; i++)
	{
		for (auto j = 0; j < 8; j++)
		{
			GetByte();
			b += GetBit(0) << j;
		}
		r += b << (i * 8);
		b = 0;
	}
	return r;
}
const unsigned char ByteReader::GetByte()
{
	curbyte = file->get();
	return curbyte;
}
const unsigned char ByteReader::GetMovedBit(const unsigned short digit)
{
	return curbyte&(1 << digit);
}
const bool ByteReader::GetBit(const unsigned short digit)
{
	return GetMovedBit(digit)>0;
}
void ByteReader::PrintByte_Bin()
{
	for (auto i = 7; i >= 0; i--)
	{
		cout << (unsigned short)GetBit(i);
	}
	cout << endl;
}
void ByteReader::PrintByte(unsigned char delim)
{
	cout << hex << setw(2) << setfill('0') << (unsigned short)GetByte() << delim;
}
void ByteReader::ReadBMPheader(BMPheader* headeraddress)
{
	unsigned char* p = (unsigned char*)headeraddress;
	for (auto i = 2; i < sizeof(BMPheader); i++)
	{
		*(p + i) = file->get();
	}	
}

int main()
{
	//Set a UpperCase Hex output format.
	cout.setf(ios::hex, ios::basefield);
	cout.setf(ios::uppercase);

	ByteReader br(R"(in.bmp)");
	ByteReader br2;
	ByteWriter bw;
	BMPheader bmph;
	unsigned int outFileLength;
	unsigned int outFileNameLength;
	short sel = 0;
	char* outFileName = NULL;

	cout << "0.Write  1.Extract  Choice:";
	cin >> sel;
	
	cout << "Input BMP opened : " << br.IsOpened() << endl;
	if (!br.IsOpened())
		return 0;

	br.ReadBMPheader(&bmph);
	bmph.ShowInfo();

	switch (sel)
	{
	case 1://Extract
		cin.get();
		outFileLength = br.GetBit2Int();
		outFileNameLength = br.GetBit2Int();
		outFileName = new char[outFileNameLength + 1];
		for (unsigned int i = 0; i < outFileNameLength; i++)
		{
			*(outFileName + i) = br.GetBit2Char();
		}
		outFileName[outFileNameLength] = 0;
		bw.OpenFile(outFileName);

		cout << "Output File Size : " << outFileLength << endl;
		cout << "Output File Name : " << outFileName << endl;
		for (unsigned int i = 0; i < outFileLength * 8; i++)
		{
			br.GetByte();
			bw.PushMovedBit(br.GetBit(0));
		}
		cout << "finish" << endl;
		break;
	default://Write
		char InputFileName[256];
		unsigned char writing = 0;
		unsigned int InputFileLength;
		unsigned int InputFileNameLength=0;
		cout << endl << "Name of the file to write : ";
		cin.get();
		cin.getline(InputFileName, 256);
		br2.OpenFile(InputFileName);
		InputFileLength = br2.GetFileLength();
		bw.OpenFile("out.bmp");
		bw.writeBMPheader(&bmph);

		cout << "br2opened : " << br2.IsOpened();
		bw.PushSteganoData(br, (unsigned char*)&InputFileLength, 4);
		for (; InputFileName[InputFileNameLength]; InputFileNameLength++);
		bw.PushSteganoData(br, (unsigned char*)&InputFileNameLength, 4);
		bw.PushSteganoData(br, (unsigned char*)InputFileName, InputFileNameLength);
		unsigned char datwrite=0;
		for (unsigned int i = 0; i < InputFileLength; i++)
		{
			datwrite = br2.GetByte();
			bw.PushSteganoData(br, &datwrite, 1);
		}
		while (!br.isEOF())
		{
			bw.PushByte(br.GetByte());
		}
		break;
	}
	cout << endl <<	"Finished" << endl;
	cin.get();
	return 1;
}
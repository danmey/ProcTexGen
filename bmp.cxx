#include "bmp.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
using namespace std;

//the writing method:
template <class T>
void writeBin(fstream &Out, T data)
{
    Out.write(reinterpret_cast<char*> (&data), sizeof(T));
}

void bmp_write(const char* filename, int* pixels, int height, int width) {

  fstream data(filename, ios::out | ios::binary | ios::trunc);
 
  if(!data.is_open() || data.bad())
    {
      cout << "error: cannot open file" << endl;
      exit(-1);
    }
 
  writeBin(data, 'B');
  writeBin(data, 'M');
  writeBin(data, (long) height*width*4+52);
  writeBin(data, (long) 0);
  writeBin(data, (long) 52);
 
  writeBin(data, (long) 40);
  writeBin(data, width);
  writeBin(data, height);
  writeBin(data, (short) 1);
  writeBin(data, (short) 32);
  writeBin(data, (long) 0);
  writeBin(data, (long) 0);
  writeBin(data, (long) 0);
  writeBin(data, (long) 0);
  writeBin(data, (long) 0);
  writeBin(data, (long) 0); 
 
  for (int y=0; y<height;y++)
  for (int x=0; x<height;x++)
      writeBin(data, *pixels++);
  data.close();
}

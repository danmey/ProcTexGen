#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

using namespace std;


#include "tga.h"
#include "perlin.h"

 //Perlin http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
 //TGA http://local.wasp.uwa.edu.au/~pbourke/dataformats/tga/
extern void start();
extern void test_parsing();
void shape_generate_print();

#include <iostream>

int parse_main(const char*,const char*);
int main(int argc, char** argv)
{
  if ( argc != 3 ) {
    cout << "Wrong number of command line parameters. Usage: texgen texdef_filename bmp_output_filename\n";
    return 0;
  }
  parse_main(argv[1], argv[2]);
  return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tga.h"
typedef unsigned char byte;
char*	loadTGA()
{
  /* FILE *fp;
	nlTGAHeader hdr;
   fp=fopen(filename,"wb");
   fread(&hdr,1, sizeof(nlTGAHeader),fp);
	for (i=0; i<h*w*4; i+=4)
       {
               *d++ = texture[i+2];
               *d++ = texture[i+1];
               *d++ = texture[i+0];
       }*/
  //	return NULL;
  return 0;
}

void	saveTGA(char *filename, char *bits, int h, int w)
{
       FILE *fp;
       int i;
       unsigned char *data;

       fp=fopen(filename,"wb");
       data=(unsigned char*)malloc(h*w*3);
       byte *d=data;

       for (i=0; i<h*w*4; i+=4)
       {
               *d++ = bits[i+2];
               *d++ = bits[i+1];
               *d++ = bits[i+0];
       }

       nlTGAHeader header;

       memset(&header,0,sizeof(header));
       header.data_type=2;
       header.width=w;
       header.height=h;
       header.data_bits=32;

       fwrite((void*)&header,sizeof(nlTGAHeader),1,fp);                
       fwrite(data,w*h*3,1,fp);
       fclose(fp);
       free(data);
}

void save_tga(char *filename, int w, int h, char* bits)
{
  /* Write the result as a uncompressed TGA */
   if ((fptr = fopen(filename,"w")) == NULL) {
      fprintf(stderr,"Failed to open outputfile\n");
      exit(-1);
   }
   putc(0,fptr);
   putc(0,fptr);
   putc(2,fptr);                         /* uncompressed RGB */
   putc(0,fptr); putc(0,fptr);
   putc(0,fptr); putc(0,fptr);
   putc(0,fptr);
   putc(0,fptr); putc(0,fptr);           /* X origin */
   putc(0,fptr); putc(0,fptr);           /* y origin */
   putc((width) & 0x00FF),fptr);
   putc((width & 0xFF00) / 256,fptr);
   putc((height & 0x00FF),fptr);
   putc((height & 0xFF00) / 256,fptr);
   putc(32,fptr);                        /* 24 bit bitmap */
   putc(0,fptr);
   for (i=0;i<header.height*header.width;i++) {
      putc(pixels[i],fptr);
      putc(pixels[i+1],fptr);
      putc(pixels[i+2],fptr);
      putc(pixels[i+3],fptr);
   }
   fclose(fptr);
}

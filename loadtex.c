/*
 *  Load texture from BMP or PNG file
 */
#include "CSCIx229.h"
#include "lodepng.h"

/*
 *  Reverse n bytes
 */
static void Reverse(void* x,const int n)
{
   int k;
   char* ch = (char*)x;
   for (k=0;k<n/2;k++)
   {
      char tmp = ch[k];
      ch[k] = ch[n-1-k];
      ch[n-1-k] = tmp;
   }
}

/*
 *  Load texture from BMP file
 */
unsigned int LoadTexBMP(const char* file)
{
   unsigned int   texture;    // Texture name
   FILE*          f;          // File pointer
   unsigned short magic;      // Image magic
   unsigned int   dx,dy,size; // Image dimensions
   unsigned short nbp,bpp;    // Planes and bits per pixel
   unsigned char* image;      // Image data
   unsigned int   k;          // Counter
   int            max;        // Maximum texture dimensions

   //  Open file
   f = fopen(file,"rb");
   if (!f) Fatal("Cannot open file %s\n",file);
   //  Check image magic
   if (fread(&magic,2,1,f)!=1) Fatal("Cannot read magic from %s\n",file);
   if (magic!=0x4D42 && magic!=0x424D) Fatal("Image magic not BMP in %s\n",file);
   //  Seek to and read header
   if (fseek(f,16,SEEK_CUR) || fread(&dx ,4,1,f)!=1 || fread(&dy ,4,1,f)!=1 ||
       fread(&nbp,2,1,f)!=1 || fread(&bpp,2,1,f)!=1 || fread(&k,4,1,f)!=1)
     Fatal("Cannot read header from %s\n",file);
   //  Reverse bytes on big endian hardware (detected by backwards magic)
   if (magic==0x424D)
   {
      Reverse(&dx,4);
      Reverse(&dy,4);
      Reverse(&nbp,2);
      Reverse(&bpp,2);
      Reverse(&k,4);
   }
   //  Check image parameters
   glGetIntegerv(GL_MAX_TEXTURE_SIZE,&max);
   if (dx<1 || dx>max) Fatal("%s image width %d out of range 1-%d\n",file,dx,max);
   if (dy<1 || dy>max) Fatal("%s image height %d out of range 1-%d\n",file,dy,max);
   if (nbp!=1)  Fatal("%s bit planes is not 1: %d\n",file,nbp);
   if (bpp!=24) Fatal("%s bits per pixel is not 24: %d\n",file,bpp);
   if (k!=0)    Fatal("%s compressed files not supported\n",file);
#ifndef GL_VERSION_2_0
   //  OpenGL 2.0 lifts the restriction that texture size must be a power of two
   for (k=1;k<dx;k*=2);
   if (k!=dx) Fatal("%s image width not a power of two: %d\n",file,dx);
   for (k=1;k<dy;k*=2);
   if (k!=dy) Fatal("%s image height not a power of two: %d\n",file,dy);
#endif

   //  Allocate image memory
   size = 3*dx*dy;
   image = (unsigned char*) malloc(size);
   if (!image) Fatal("Cannot allocate %d bytes of memory for image %s\n",size,file);
   //  Seek to and read image
   if (fseek(f,20,SEEK_CUR) || fread(image,size,1,f)!=1) Fatal("Error reading data from image %s\n",file);
   fclose(f);
   //  Reverse colors (BGR -> RGB)
   for (k=0;k<size;k+=3)
   {
      unsigned char temp = image[k];
      image[k]   = image[k+2];
      image[k+2] = temp;
   }

   //  Sanity check
   ErrCheck("LoadTexBMP");
   //  Generate 2D texture
   glGenTextures(1,&texture);
   glBindTexture(GL_TEXTURE_2D,texture);
   //  Copy image
   glTexImage2D(GL_TEXTURE_2D,0,3,dx,dy,0,GL_RGB,GL_UNSIGNED_BYTE,image);
   if (glGetError()) Fatal("Error in glTexImage2D %s %dx%d\n",file,dx,dy);
   //  Scale linearly when image size doesn't match
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

   //  Free image memory
   free(image);
   //  Return texture name
   return texture;
}

/*
 * Load texture from PNG file.
 * 
 */
 
unsigned int loadPng(char* filename[], int n, unsigned int textures[])
{
    
  glGenTextures(n, textures);
  int i;
  
  for(i = 0; i < n; i++)
  {
      unsigned error;
      unsigned char* image;
      unsigned width, height;

      error = lodepng_decode32_file(&image, &width, &height, filename[i]);
      if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

      // Texture size must be power of two for the primitive OpenGL version this is written for. Find next power of two.
      size_t u2 = 1; while(u2 < width) u2 *= 2;
      size_t v2 = 1; while(v2 < height) v2 *= 2;

      // Make power of two version of the image.
      unsigned char *image2 = (unsigned char*) malloc(sizeof(unsigned char) * u2 * v2 * 4);
      size_t y, x, c;
      for(y = 0; y < height; y++)
      for(x = 0; x < width; x++)
      for(c = 0; c < 4; c++)
      {
        image2[4 * u2 * y + 4 * x + c] = image[4 * width * y + 4 * x + c];
      }
      
      glEnable(GL_TEXTURE_2D);
       
      glBindTexture(GL_TEXTURE_2D, textures[i]);
     
      glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); 
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
      
      glTexImage2D(GL_TEXTURE_2D, 0, 4, u2, v2, 0, GL_RGBA, GL_UNSIGNED_BYTE, image2);

      free(image);
      free(image2);
  }
  
  return 0;
}

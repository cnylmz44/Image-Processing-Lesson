#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#pragma pack(1)
typedef unsigned char BYTE;//1 bytes of memory
typedef unsigned short WORD;//2 bytes of memory
typedef unsigned int DWORD;//4 bytes of memory
typedef int LONG;//4 bytes of memory
typedef struct _BMPFH
{
	BYTE bftype1;
	BYTE bftype2;
	DWORD bfsize;
	WORD bfreserved1;
	WORD bfreserved2;
	DWORD bfOffbits;
}  __attribute__ ((__packed__)) BMPFH;
typedef struct _BMPIH
{
	DWORD bisize;
	LONG  biw;
	LONG bih;
	WORD biplane;
	WORD bibitcount;
	DWORD biComp;
	DWORD bisizeimage;
	LONG bix;
	LONG biy;
	DWORD biclused;
 	DWORD biclimp;
} __attribute__ ((__packed__)) BMPIH;
typedef struct _PALET
{
	BYTE rgbblue;
	BYTE rgbgreen;
	BYTE rgbred;
	BYTE rgbreserved;
}  __attribute__ ((__packed__)) PALET;
typedef struct _IMAGE
{
	BMPFH   bmpfh;
	BMPIH   bmpih;
	PALET   *palet;
	BYTE    *data;
}  __attribute__ ((__packed__)) IMAGE;
IMAGE *ImageRead(IMAGE *image,char *filename)
{
	BMPFH bmpfh;
	BMPIH bmpih;
	FILE *fp;
	DWORD r,rowsize,size;

	fp=fopen(filename,"rb");//tries to open the filename

	if(fp==NULL) {
		printf("File is not found..");
		exit(1);
	}

	fread(&bmpfh,sizeof(BMPFH),1,fp);//reads bitmap file header from the file and set to bmph
    fread(&bmpih,sizeof(BMPIH),1,fp);//reads bitmap info header from the file and set to bmpih

	image=(IMAGE *) malloc(bmpfh.bfsize);//allocates memory for image

	if(image==NULL) {
		printf("There is no enough memory for this operation");
		exit(1);
	}

	image->bmpfh=bmpfh;//sets bmpfh to image 
	image->bmpih=bmpih;//sets bmpih to image
	
	r=0;//r is set to 0 in case 24 bits per pixel or more (this kind of images does not have color palette)

    if(bmpih.bibitcount==1) r=2;//if the image 1 bit per pixel (the number of clor is 2)
	if(bmpih.bibitcount==4) r=16;//if the image 4 bits per pixel (the number of clor is 16)
	if(bmpih.bibitcount==8) r=256;////if the image 8 bits per pixel (the number of clor is 256)

	if(r!=0)
	{
		image->palet=(PALET *) malloc(4*r);//allocate memort for color palette
		fread(image->palet,4*r,1,fp);//read color palette from image to the memory
	}

	rowsize=(image->bmpih.biw*image->bmpih.bibitcount+31)/32*4;//calculates 1 row of image size 
	size=rowsize*image->bmpih.bih;//all size of image is calculated and set to size 
	image->data=(BYTE *)malloc(size);//allocates enough memory for iamage data
	fread(image->data,size,1,fp);//reads image data from file and sets to image->data
	fclose(fp); //closes the file

	return image;//returns the adress of image on the memory
}       
void ImageWrite(IMAGE *image,char *filename)
{
	FILE *fp;
	int r,rowsize,size;

	fp=fopen(filename,"wb");//opens the file in binary mode

	if(fp==NULL) {
		printf("Fie opening error!");
		exit(1);
	}

	fwrite(&image->bmpfh,sizeof(BMPFH),1,fp);//writes the bitmap file header to the file
	fwrite(&image->bmpih,sizeof(BMPIH),1,fp);//writes he bitmep info header to the file		

	r=0;

    if(image->bmpih.bibitcount==1) r=2;//if the image has 2 colors
	if(image->bmpih.bibitcount==4) r=16;//if the image has 16 colors
	if(image->bmpih.bibitcount==8) r=256;//if the image has 256 colors

	if(r!=0) fwrite(image->palet,4*r,1,fp);//if the image has color palette the palette is written to the file

	rowsize=(image->bmpih.biw*image->bmpih.bibitcount+31)/32*4;//a row size of image is calculated 
	size=rowsize*image->bmpih.bih;//all size of image is calculated

	fwrite(image->data,size,1,fp);//write image data to the file
	fclose(fp);//closes the file 
}
void writeInfo(IMAGE *image,char *fname)
{
	printf("--------Info about %s  image file\n",fname);
	printf("bfType value		:%c%c\n",image->bmpfh.bftype1,image->bmpfh.bftype2);
	printf("bfsize			:%d\n",image->bmpfh.bfsize);
	printf("bfreserved1		:%d\n",image->bmpfh.bfreserved1);
	printf("bfreserved2		:%d\n",image->bmpfh.bfreserved2);
	printf("bfOffbits		:%d\n",image->bmpfh.bfOffbits);
	printf("bisize			:%d\n",image->bmpih.bisize);
	printf("Width			:%d\n",image->bmpih.biw);
	printf("Height			:%d\n",image->bmpih.bih);
	printf("biplane			:%d\n",image->bmpih.biplane);
	printf("bibitcount		:%d\n",image->bmpih.bibitcount);
	printf("Compression		:%d\n",image->bmpih.biComp);
	printf("bisizeimage		:%d\n",image->bmpih.bisizeimage);
	printf("bix			:%d\n",image->bmpih.bix);
	printf("biy			:%d\n",image->bmpih.biy);
	printf("bi color used		:%d\n",image->bmpih.biclused);
	printf("bi color imp.		:%d\n",image->bmpih.biclimp);
	
}

void setImageData2Zero(IMAGE* image){
	int i,j;

	for (i = 0; i < image->bmpih.bih ; i++)
	{
		for ( j = 0; j < image->bmpih.biw; j++)
		{
			image->data[i*image->bmpih.biw+j] = 0;
		}
		
	}
}

void changePalette(IMAGE *image)
{
	int i;
	if(image->bmpih.bibitcount==8)
	{
		for(i=0;i<256;i++)
		{
			image->palet[i].rgbblue=i;
			image->palet[i].rgbgreen=i;
			image->palet[i].rgbred=i;
			image->palet[i].rgbreserved=0;
		}
	}
}

void negativeImage(IMAGE *image)
{
	int w,h,rowsize,i,j;
	w=image->bmpih.biw;
	h=image->bmpih.bih;
	rowsize=(image->bmpih.bibitcount*w+31)/32*4;
	for(i=0;i<h;i++)
	   for(j=0;j<rowsize;j++)
	       image->data[i*rowsize+j]=255-image->data[i*rowsize+j];
    return;
}
void thresholdImage(IMAGE *image,int threshold)
{
   	int w,h,rowsize,i,j;
	w=image->bmpih.biw;
	h=image->bmpih.bih;
	rowsize=(image->bmpih.bibitcount*w+31)/32*4;
	for(i=0;i<h;i++)
	   for(j=0;j<rowsize;j++)
	       if(image->data[i*rowsize+j]>threshold) image->data[i*rowsize+j]=255;
	       else image->data[i*rowsize+j]=0;
    return;	
}

void histogram(IMAGE *image,char *fname)
{
	int hist[256]={0};
	int h,w,rowsize,i,j;
	w=image->bmpih.biw;
	h=image->bmpih.bih;
	rowsize=(image->bmpih.bibitcount*w+31)/32*4;
	for(i=0;i<h;i++)
	   for(j=0;j<rowsize;j++)
	        hist[image->data[i*rowsize+j]]++;
	FILE *fp;
	fp=fopen(fname,"w");
	fprintf(fp,"Gray Level\tFrequency\n");
	for(i=0;i<256;i++) fprintf(fp,"%d\t\t%d\n",i,hist[i]);
	fclose(fp);
	
}

void allBlack(IMAGE *image)
{
	int h,w,rowsize,i,j;
	w=image->bmpih.biw;
	h=image->bmpih.bih;
	rowsize=(image->bmpih.bibitcount*w+31)/32*4;
	for(i=0;i<h;i++)
	   for(j=0;j<rowsize;j++)
	        image->data[i*rowsize+j]=0;
	return;
}
void brightImage(IMAGE *image,int level)
{
	int h,w,rowsize,i,j,k;
	w=image->bmpih.biw;
	h=image->bmpih.bih;
	rowsize=(image->bmpih.bibitcount*w+31)/32*4;
	for(i=0;i<h;i++)
	   for(j=0;j<rowsize;j++)
	   {
	   	 k=image->data[i*rowsize+j]+level;
	   	 if(k>255) k=255;
	   	 if(k<0) k=0;
	   	 image->data[i*rowsize+j]=(BYTE) k;
	   }
	return;
	      
}
void maxAndMinIntensity(IMAGE*image,LONG &max,LONG &min)
{
	int h,w,rowsize,i,j,k;
	w=image->bmpih.biw;
	h=image->bmpih.bih;
	rowsize=(image->bmpih.bibitcount*w+31)/32*4;
	max=0;min=255;
	for(i=0;i<h;i++)
	   for(j=0;j<rowsize;j++)
	   {
	   	 k=image->data[i*rowsize+j];
	   	 if(k>max) max=k;
	   	 if(k<min) min=k;
	   }
	   return;
}

void stretchImage(IMAGE *image,BYTE MAX,BYTE MIN)
{
    LONG h,w,rowsize,i,j,k,max,min;
	w=image->bmpih.biw;
	h=image->bmpih.bih;
	rowsize=(image->bmpih.bibitcount*w+31)/32*4;
	maxAndMinIntensity(image,max,min);
	for(i=0;i<h;i++)
	 for(j=0;j<rowsize;j++)
	    image->data[i*rowsize+j]=(MAX-MIN)*(image->data[i*rowsize+j]-min)/(max-min)+MIN;
	return;
}

void histogram(IMAGE *image,double CDF[])
{
	int hist[256]={0};
	int h,w,rowsize,i,j;
	double sum = 0;
	w=image->bmpih.biw;
	h=image->bmpih.bih;
	rowsize=(image->bmpih.bibitcount*w+31)/32*4;
	for(i=0;i<h;i++)
	   for(j=0;j<rowsize;j++)
	        hist[image->data[i*rowsize+j]]++;
	for(i=0;i<256;i++){
		sum += (double)hist[i]/(h*rowsize);
		CDF[i] = sum;
	}

	return;
	
}


void histogramEqualization(IMAGE *image){
	double CDF[256]={0.0};
	int Normalization[256];
	int h,w,rowsize,i,j,k;

	w=image->bmpih.biw;
	h=image->bmpih.bih;
	rowsize=(image->bmpih.bibitcount*w+31)/32*4;

	histogram(image,CDF);

	for(i=0;i<256;i++) Normalization[i] = 255*CDF[i];

	for ( i = 0; i < h; i++)
	{
		for ( j = 0; j < rowsize; j++)
		{
			image->data[i*rowsize+j] = Normalization[image->data[i*rowsize+j]];
		}
		
	}
	
	return;

}


main()
{
	IMAGE *image;
	image=ImageRead(image,"kelebek.bmp");//reads the image from file and sets 1all info to the ram
	//insert your transformation code here
	//image
	//setImageData2Zero(image);
    //writeInfo(image,"ucak.bmp");
	//changePalette(image);
	//negativeImage(image);
	//thresholdImage(image,200);
	//histogram(image,"histogram.txt");
	//allBlack(image);
	//brightImage(image,20);
	//maxAndMinIntensity(image,100.0,50);
	histogramEqualization(image);
	//histogram(image,"equalization.txt");	
	//----------------------------
	ImageWrite(image,"test2.bmp");//writes this image to file named "test3.bmp"
    free(image);	
    system("pause");
	return 0;
}

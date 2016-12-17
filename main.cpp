/* pbmtoSnes v1.0
A Simple C++ Program that converts pbm image of 256x256px
into pallete, tiledata and tilemap bin files that can be
loaded into snes.
*/

#include <iostream>
#include <iomanip>
#include <math.h>
#include <fstream>
using namespace std;

#define BYTE sizeof(char)
#define IMGNAME "img256.pbm"

class pixel{
public:
	int R, G, B;
};
class palArray{

int top;

class pxlstruct{
public:
	pixel p;
	pxlstruct* next;
};
//public:
pxlstruct *head, *end;


public:
unsigned long int maxpix;
	palArray(){
		head=nullptr;
		end=nullptr;
		top=0;
		maxpix=0;
	}

	int getTop(){return top;}

	unsigned int getConvColor(int icolor=0){
		pxlstruct *pxl = head;

		for(int i=0; i<icolor && i<top; i++){
			pxl=pxl->next;
		}
		//cout<<"Color "<<icolor<<"|R="<<pxl->p.R
			//	<<" G="<<pxl->p.G<<" B="<<pxl->p.B<<endl;

		unsigned int FinalColor = 0;
		pixel p =pxl->p;
		pxl=nullptr;

		p.B = p.B/8;
		p.G = p.G/8;
		p.R = p.R/8; //Reducing to 5bit/channel

		//cout<<"Conv Color "<<icolor<<"|R="<<p.R
			//			<<" G="<<p.G<<" B="<<p.B<<endl;

		FinalColor=p.B<<10; //inserting R to 1st 5 bit
		FinalColor|=p.G<<5; //inserting g's Next 5 bits
		FinalColor|=p.R; //inserting p to Next 5

		unsigned int FinalData = 0;

		//Shifting digits so that low 8 bytes of the Final Color
		//Comes first.
		/*for(int i=0; i<16; i++){
			if(8-i>0)
			FinalData |= (FinalColor&(1<<i))<<(15-(i*2));
			else
			FinalData |= (FinalColor&(1<<i))>>((i*2)-15);
		}*/

		return FinalColor;
	}

	int AddColor(pixel &obj){
		maxpix++;
		pxlstruct *pxl=head;
		for(int i=0; i<top; i++){
			if( pxl->p.R==obj.R && pxl->p.G==obj.G && pxl->p.B==obj.B){

				//cout<<maxpix<<". Already Added "<<i<<"|R="<<pxl->p.R
				//				<<" G="<<pxl->p.G<<" B="<<pxl->p.B<<endl;
				pxl=nullptr;
				return i;	// already inserted
			}
			pxl=pxl->next;
		}

		//insert color
		pxl=nullptr;
		pxl=new pxlstruct();
		pxl->p = obj;
		if(top==0){
			head=end=pxl;
		}else{
			end->next=pxl;
			end=pxl;
		}

		//cout<<dec<<maxpix<<hex<<". Color Added "<<top<<"|R="<<end->p.R
		//		<<" G="<<end->p.G<<" B="<<end->p.B<<endl;

		return top++;

	}

	~palArray(){
		pxlstruct *pxl=head;
		for(;top!=0;){
			pxl=head;
			for(int i=0; i<(top-1); i++){
				pxl=pxl->next;
			}
			delete pxl;
			top--;
		}
		head=nullptr;
		pxl=nullptr;
		}
};

class tile{
public:
	char bitP[8][8];
};



int main(){
	//Opening file
	fstream file(IMGNAME, ios::binary|ios::in);
	if(file)
		cout<<"File Size:"<<sizeof(file)<<endl;
	else
		cout<<"Error: can't open File";

	//Checking File
	char fileFormat;
	int wid(0), hig(0), val(0);
	int dataSize;

	file.seekg(BYTE, ios::beg);	//First Letter is P
	file.read( (char*)&fileFormat, BYTE); //Second is the format number
	cout<<"FileFormat: P"<<fileFormat<<endl;	//Display the format

	file.seekg(BYTE, ios::cur); //the third char is newline, 0A
	//until next line(0A), is width. same way next is height and max value
	//getting size of width, height and value
	int wsize(0),hsize(0),vsize(0);
	char c('0') ;
	do{
		file.read( (char*)&c, BYTE);
		wsize++;
	}while(c!=(char)0x70A);
	cout<<"width-digits:"<<--wsize;	//adjusting for last extra loop

	c='0';
	do{
		file.read( (char*)&c, BYTE);
		hsize++;
	}while(c!=(char)0x70A);
	cout<<"height-digits:"<<--hsize;

	c='0';
	do{
		file.read( (char*)&c, BYTE);
		vsize++;
	}while(c!=(char)0x70A);
	cout<<"value-digits:"<<--vsize;
	cout<<endl;

	//loading values
	file.seekg(BYTE*3, ios::beg);
	for(int i=wsize-1; i>=0; i--){
		file.read( (char*)&c, BYTE);
		wid += ((int)c-48)*pow(10,i);
	}
	cout<<"width:"<<wid<<endl;

	file.seekg(BYTE, ios::cur);
	for(int i=hsize-1; i>=0; i--){
		file.read( (char*)&c, BYTE);
		hig += ((int)c-48)*pow(10,i);
	}
	cout<<"height:"<<hig<<endl;

	file.seekg(BYTE, ios::cur);
	for(int i=vsize-1; i>=0; i--){
		file.read( (char*)&c, BYTE);
		val += ((int)c-48)*pow(10,i);
	}
	cout<<"max-value:"<<val<<endl;
	file.seekg(BYTE, ios::cur); //next is pure image data

	dataSize = wid*hig*3;
	cout<<"Data Size: "<<dataSize<<endl;



	//Add all the colors and tiles
	palArray pal;
	pixel black;
	black.R=0; black.B=0; black.G=0;
	pal.AddColor(black);
	tile tmap[32][32];

	for(int i=0; i<hig/8; i++)
	for(int f=0; f<8; f++){

		pixel color;
		char a('0');
		int x(0);


		for(int g=0; g<wid/8; g++){
			//clearing junk data from tile
			/*for(int y=0; y<8; y++)
				tmap[i][f].bitP[y][g]=0;
				*/

			//getting color, adding to pal, and setting tile
		for(int h=0; h<8; h++){
			a='0';x=0;color.R=color.G=color.B=0;
			//R
			file.read((char*)&a, BYTE);

			/*
			 * since a is a char(1b) and x is int(2byte)
			 * problem will arise if MSB is 1 in a.
			 * that'll make x negative
			 */
			x=a;
			if(x < 0){
					a=a&127;	// removing msb
					x=a;		// assinging w/o MSB
					x=x|128;	// adding back MSB
			}
			color.R=x;

			//G
			file.read((char*)&a, BYTE);
			x=a;
			if(x < 0){
					a=a&127;
					x=a;
					x=x|128;
			}
			color.G=x;

			//B
			file.read((char*)&a, BYTE);
			x=a;
			if(x < 0){
					a=a&127;
					x=a;
					x=x|128;
			}
			color.B=x;

			//Add to Pal
			int palpos = pal.AddColor(color);

			char cpalpos = palpos;
			if(palpos>127){
				palpos = palpos & 127; 		// removing MSB
				cpalpos = palpos;
				cpalpos = cpalpos | 128; 	// insert  MSB
			}


			// Think g, h as pixel coord. g is the length, h is the width
			// and y as the bit plane no.
			/*
			 * What's happening here is a `bit` complicated :P
			 * for the g-th line's, h-th bit, we have to put the
			 * corresponding color bit of cpalpos along the same
			 * g,h coords of all 8 bitplanes.
			 */
			for(int y=0; y<8 && cpalpos!=0; y++){
				if(7-h-y>0)
				tmap[i][g].bitP[y][f] |=(cpalpos & (1<<y)) << (7-h-y);
				else
				tmap[i][g].bitP[y][f] |=(cpalpos & (1<<y)) >> (h+y-7);
			}
			/*
			 * What i did is:
			 * 1. isolated the bit that is to be stored in the
			 * 	  current bitplane by `calpos & (1<<y)`
			 * 2. since 1st bit of g-th line(h=0) stores the first
			 * 	  pixel color no, I had to shift all the bytes to
			 * 	  to the left by `<< (7-h-y)`. now for the first color in
			 * 	  a g-th line(h=0, `<< 7`), bits in bit plane will be
			 * 	  stored in the first bit of every gth line.
			 * 3. `|=` doesn't delets the previous data, kinda obvious.
			 */

		}}

	}


	cout <<dec<<(int)tmap[0][0].bitP[0][0] <<endl;

	file.close();

	//All the data we need is in ram
	//lets write it to the pal and tile files.


	// Writing to pal File
	fstream palFile("palData.inc",ios::out|ios::trunc);

	int ncolors = pal.getTop();

	for(int i=0; i<ncolors; i++){
		if(i==0 || i%4==0){
			palFile<<endl<<".db	";
		}
		unsigned int D1 = pal.getConvColor(i);
		unsigned int HigByte = (D1&0xFF00)>>8; //Seperating High And Low
		unsigned int LowByte = (D1&0x00FF);
		palFile<<"$"<<setfill('0')<<setw(2)<<hex<<LowByte<<",$"<<setw(2)<<HigByte<<",";
		//Putting in text File as hex
	}
	palFile.close();


	//Writing to TileFile



	fstream tileFile;

	for(int i=0; i<hig/8; i++){
		if(i==0)
			tileFile.open("tileData1.inc", ios::out|ios::trunc);
		/*if(i==16){
			tileFile.close();
			tileFile.open("tileData2.inc", ios::out|ios::trunc);
		}*/

	for(int f=0; f<wid/8; f++){

		for(int y=0; y<4; y+=2){

			for(int g=0; g<8; g++){
				if(g==4 || g==0){
					tileFile<<endl<<".db	";
				}
			int pData0 = (int)tmap[i][f].bitP[y][g];
			int pData1 = (int)tmap[i][f].bitP[y+1][g];

			if(pData0 < 0){
				pData0 &= 0xFF; // remove MSB
				pData0 |= 0x80; //Adding it back
			}

			if(pData1 < 0){
							pData1 &= 0xFF; // remove MSB
							pData1 |= 0x80; //Adding it back
			}

			tileFile<<"$"<<hex<<setfill('0')<<setw(2)<<pData0<<",";
			tileFile<<"$"<<hex<<setfill('0')<<setw(2)<<pData1<<",";
			}
		}


	}}


	tileFile.close();





	return 0;
}

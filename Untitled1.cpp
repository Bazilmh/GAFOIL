#include <windows.h>
#include <iostream>
#include <fstream>
#include <SDL.h>
#include <math.h>

#define NUM_MIN 0.0000000000000000001
Uint32 color=0x000000;
Uint32 * pixels = new Uint32[640 * 480];

using namespace std;
fstream file,file2,file3;
char ch;
int GL_POP=20;
float randx,randy,randspx,randspy,base=0,range_high=300,range_low=0,chrom_cnt=4,GEN=0;




struct CHROM
{
       float gene[120];
       float val;
       
      
};
struct CELL
{
bool fit;       
CHROM chrom[60];  
float fitness_val;   
int generation; 
};


struct AIRFOIL
{
       
double pntx[200];
double pnty[200];
int cnt;
      
}seed_af,mod_af;


CELL human[1000],temp;
CELL solutions[200];
int sol_cnt;




//------------------------------------------------------------------------------
void del(int millis)
{
long int time,elapsed;
     
time = timeGetTime();

start:
elapsed = timeGetTime()-time;
if(elapsed >= millis)
goto end;
else
goto start;

end:;
//cout<<endl;

}
//------------------------------------------------------------------------------
Uint32 rgbcol(Uint32 r,Uint32 g,Uint32 b)
{

return (r*0x10000+g*0x100+b);

}


//------------------------------------------------------------------------------
Uint32 colscale(float val)
{
if(val>255)
return rgbcol(255,0,0);
else
if(val<0)
return rgbcol(0,255,0);
else
return rgbcol(val,255-val,0);

}


//------------------------------------------------------------------------------
float dist2d(float x1,float y1,float x2,float y2)               
{
       
 return sqrt( ((x2-x1)*(x2-x1)) + ((y2-y1)*(y2-y1))  );


}



//------------------------------------------------------------------------------
void putpixel(int x,int y,int resx,int resy,Uint32 col)
{

 pixels[resx*y+x] = col;

}


//------------------------------------------------------------------------------
void putpixels(int x,int y,int resx,int resy,int len,Uint32 col)
{
len = x+len;
for(int i=x;i<len;i++)
pixels[resx*y+i] = col;

}

//------------------------------------------------------------------------------
void write_file(AIRFOIL af,char*str)
{
file.open(str,ios::out);

file<<"NEW AIRFOIL"<<endl;
for(int i=0;i<af.cnt;i++)
{
file<<"    "<<af.pntx[i]<<"      "<<af.pnty[i]<<endl;
}
file.close();
}
//------------------------------------------------------------------------------
int random(int a,int b,int d)  //argument d is provided as relaxation time for getting random values in a loop. Generally d >= 700000 for loop.
{                              //for generating random number out of a loop argument d can be zero.
int c = b-a;

for(int i=0;i<d;i++)
continue;       

      
       
return ( (timeGetTime()%c)+a );   

}


//------------------------------------------------------------------------------
float random_place(float a,float b)
{
float mult =0;
for(int i=0;i<80;i++)
{
float diff;


if( randx < 0 || randx > 100 )
randspx*=-1;

if( randy < 0 || randy > 100 )
randspy*=-1;





float unitx,unity,mag;




mag = dist2d(100,100,randx,randy);

unitx = (randx-50)/mag;
unity = (randy-50)/mag;


             
if(dist2d(50,50,randx,randy) < ((timeGetTime()%4000)*0.01))
{
randspx=(unitx*0.7); 
randspy=(unity*0.7); 
}

//randx+=((timeGetTime()%1000)*0.0000001);
//randy+=((timeGetTime()%1000)*0.0000001);

randx+=randspx;
randy+=randspy;



putpixel(randx,randy,640,480,0x00ff00);

diff = b-a;
mult = diff/100;

//putpixel(base,( ((randx+randy)*0.5*mult)+a),640,480,0x00ff00);

}

//cout<<( ((randx+randy)*0.5*mult)+ a )<<endl;
base++;
return ( ((randx+randy)*0.5*mult)+ a );

}



//------------------------------------------------------------------------------
void encode_to_gene(int index1,int index2)
{

for(int i=0;i<100;i++)
human[index1].chrom[index2].gene[i] = (human[index1].chrom[index2].val/5050)*(i+1);

}

//------------------------------------------------------------------------------
void decode_from_gene(int index1,int index2)
{

human[index1].chrom[index2].val=0;   
for(int i=0;i<100;i++)
{human[index1].chrom[index2].val+= human[index1].chrom[index2].gene[i]; } 
     
     
}

//------------------------------------------------------------------------------
CHROM one_point_crossover(int human1,int human2, int chrom_no,int cross_point)
{
CHROM offspring;  int i;

for(i=0;i<cross_point;i++)
{ 
                                                  
offspring.gene[i] = human[human1].chrom[chrom_no].gene[i];
                  
}

for(i=cross_point;i<100;i++)
{ 
                                                  
offspring.gene[i] = human[human2].chrom[chrom_no].gene[i];
                  
}

return offspring;

}
//------------------------------------------------------------------------------
void mutation(int pop_no,int chrom_no)
{

for(int i=0;i<pop_no;i++)
{

for(int j=0;j<100;j++)
{
human[i].chrom[chrom_no].gene[j]+=(random(10,20,0)*(0.1));
             
}
}

}


//------------------------------------------------------------------------------
void gen_population(int pop_no,int chrom_no,float a,float b,int start)
{
for(int i=start;i<pop_no;i++)
{
human[i].chrom[chrom_no].val = random_place(a,b);
human[i].fit = 1; 
}

}


//------------------------------------------------------------------------------
void fusion(int pop_no,int chrom_no)
{


for(int i=0;i<(pop_no/2);i++)
{

for(int j=0;j<chrom_no;j++)
{              
human[i+(pop_no/2)].chrom[j] = one_point_crossover(i+(pop_no/2),i,j,random_place(20,80));    
}

}




}
//------------------------------------------------------------------------------
void modify_shape(float amp,float omg,float phi,float t)
{
mod_af.cnt = seed_af.cnt;     
for(int i=0;i<seed_af.cnt;i++)
{
mod_af.pntx[i]=seed_af.pntx[i];
mod_af.pnty[i]=seed_af.pnty[i];


}

for(int i=0;i<mod_af.cnt;i++)
{
mod_af.pnty[i]*=t;

mod_af.pnty[i] += sin((mod_af.pntx[i]*omg)+phi)*amp;     // range amp(-0.05,0.05) omg(0,10) phi(0,2) t(0,3)

}




}

//------------------------------------------------------------------------------
void fitness_function(int pop_no,float factor,float fit_val)
{

for(int i=0;i<pop_no;i++)
{

int flag =0;

             
if(human[i].chrom[0].val > 0.05 || human[i].chrom[0].val < -0.05)
flag = 1; 
else
if(human[i].chrom[1].val > 10 || human[i].chrom[1].val < 0)
flag = 1; 
else
if(human[i].chrom[2].val > 2 || human[i].chrom[2].val < 0)
flag = 1; 
else
if(human[i].chrom[3].val > 3 || human[i].chrom[3].val < 0)
flag = 1;     


if(flag==1)
human[i].fitness_val =0;
else
{
modify_shape(human[i].chrom[0].val,human[i].chrom[1].val,human[i].chrom[2].val,human[i].chrom[3].val); 
write_file(mod_af,"input.dat");
system("flow_calc.py");

//del(50);

file2.open("output.txt");
file2>>human[i].fitness_val;
file2.close();

//system("del input.dat");

}


}



//system("del output.txt");





}

//------------------------------------------------------------------------------
void sort(int pop_no)
{
int a = 0,big_index;
float big; 
for(int i=0;i<pop_no;i++)
{

big =0; 
for(int j=a;j<pop_no;j++)
{
if( big < human[j].fitness_val)
{big_index = j; big = human[j].fitness_val;}      
        
       
}

temp = human[i];
human[i] = human[big_index];
human[big_index] = temp;


a++;
}




}
//------------------------------------------------------------------------------
void modified_generation(int a)
{
float div = GL_POP/20,diff=0;  
CELL mod_gen[50];
int mod_gen_cnt=0;   
     
for(int i=a;i<(4+a);i++)
{
                           

   
for(float j=1;j<(div+3);j++)
{

for(int k=0;k<chrom_cnt;k++)
{
diff = human[i].chrom[k].val-human[i+1].chrom[k].val;     
mod_gen[mod_gen_cnt].chrom[k].val = human[i+1].chrom[k].val+((diff/(div+1))*j);
//mod_gen[mod_gen_cnt].chrom[k].val = diff;
}

mod_gen_cnt++;      
      
}


}


for(int i=0,j=(GL_POP-1);i<mod_gen_cnt;i++,j--)
{
        
human[j] = mod_gen[i];

}


}
//------------------------------------------------------------------------------
void line_draw(int x1,int y1,int x2,int y2,Uint32 col)
{

float dist = dist2d(x1,y1,x2,y2);
float unitx = (x2-x1)/dist;
float unity = (y2-y1)/dist;
float len=0;
for(float i=x1,j=y1;len<=dist;)
{
          
putpixel( i,j,640,480,col);     
i+=unitx;
j+=unity;
len++;

}


}

//------------------------------------------------------------------------------
void test_vel_dist()
{




}


//------------------------------------------------------------------------------
void disp_af(AIRFOIL af)
{
     
for(int i=0;i<af.cnt;i++)
{
af.pntx[i]*=500;
af.pnty[i]*=500;  
}
 
for(int i=0;i<(af.cnt-1);i++)
{
line_draw(af.pntx[i]+20,af.pnty[i]+200,af.pntx[i+1]+20,af.pnty[i+1]+200,0xff0000);
}


}


//------------------------------------------------------------------------------
void load_airfoil()
{

file.open("naca0012.txt",ios::in);

file>>seed_af.cnt;
for(int i=0;i<seed_af.cnt;i++)
{
file>>seed_af.pntx[i];
file>>seed_af.pnty[i];

}
file.close();


}

//------------------------------------------------------------------------------
void setup()
{
     
randx = 10;
randy = 10;

randspx = (timeGetTime()%10)*0.1;
randspy = (timeGetTime()%10)*0.1+0.1;
     
 
  
gen_population(GL_POP,0,-0.05,0.05,0);
gen_population(GL_POP,1,0,10,0);
gen_population(GL_POP,2,0,2,0);
gen_population(GL_POP,3,0,2,0);

load_airfoil();


fitness_function(GL_POP,0,0);


sort(GL_POP);


/*
for(int i=0;i<GL_POP;i++)
{
for(int j=0;j<10;j++)
{
putpixels(10,j*5,640,480,human[i].chrom[j].val,0x00ff00);

}

}
*/

}


void render()
{
     
//In this program the fitness function will be 7A-2B where the value of A and B should be between 100 and 200 (both values inclusive).   
//so the maximum value for this function will be (7x200-2x100) = 1200. 
//here we'll be using genetic algorithhm to find the optimized value of A and B. 

for(int i=0;i<GL_POP;i++)
{
        
        
for(int j=0;j<chrom_cnt;j++)
{
        
float mult = 15/(range_high-range_low);        
putpixels(i*30,j*2+300,640,480,human[i].chrom[j].val*5,0x00ff00);

}



}

for(int i=0;i<GL_POP;i++)
{
for(int j=0;j<chrom_cnt;j++)
{        
encode_to_gene(i,j);
}
}

fusion(GL_POP,4);

for(int i=0;i<GL_POP;i++)
{
for(int j=0;j<chrom_cnt;j++)
{        
decode_from_gene(i,j);

}
}

fitness_function(GL_POP,0,0);

sort(GL_POP);




//putpixels(200,200,640,480,human[0].fitness_val*100,0x00ffff);
//cout<<human[0].fitness_val<<endl;

gen_population(GL_POP,0,-0.05,0.05,GL_POP/4);
gen_population(GL_POP,1,0,10,GL_POP/4);
gen_population(GL_POP,2,0,2,GL_POP/4);
gen_population(GL_POP,3,0,2,GL_POP/4);


modified_generation(0);

//line_draw(200,200,100,300,0x00ff00);



}



int main(int argc, char ** argv)
{
    bool leftMouseButtonDown = false;
    bool quit = false;
    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window = SDL_CreateWindow("SDL2 Pixel Drawing",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture * texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 640, 480);
    


     
    //memset(pixels, 125, 640 * 480 * sizeof(Uint32));
    
     
    setup(); 
    
    while (!quit)
    {
        SDL_UpdateTexture(texture, NULL, pixels, 640 * sizeof(Uint32));

      
         while( SDL_PollEvent( &event ) != 0 )
                {
                    //User requests quit
                    if( event.type == SDL_QUIT )
                    {
                        quit = true;
                    }
                }
 
                    memset(pixels, 0, 640 * 480 * sizeof(Uint32));
                    
                    
                 
                 
                 
                 modify_shape(human[0].chrom[0].val,human[0].chrom[1].val,human[0].chrom[2].val,human[0].chrom[3].val); 
                // disp_af(mod_af);
                 render();
                  
                 // for(int i=0;i<20;i++)
                 
                  cout<<"popno : "<<GEN<<"  "<<human[0].fitness_val<<endl;
                  write_file(mod_af,"optimized.txt");
                  cout<<"-----------------------------------------"<<endl;
                  file3.open("result.txt",ios::out|ios::app);
                  file3<<GEN<<"\t"<<human[0].fitness_val<<endl;
                  file3.close();
                  
                  GEN++;         
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    delete[] pixels;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

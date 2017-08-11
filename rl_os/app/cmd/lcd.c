#define LCD_CMD_ADDR 0x7fdc
#define LCD_DATA_ADDR 0x7fdd

#define LCD_CLEAR 0x02
#define LCD_SETADDR 0x01


#define WIDTH 320
#define WIDTH_WORDS 20
#define HEIGHT 240

#include <string.h>

unsigned int fb[20*240*2];


void drawFB() {
  int cur = 0;
  int prev = 20*240;
  int end = 20*240;
  int needSetAddr = 1;  


  for(cur = 0; cur < end; cur++) {
    if(fb[cur] != fb[prev]) {
      fb[prev] = fb[cur];
      if(needSetAddr) {
        *(unsigned int *)(LCD_CMD_ADDR) = LCD_SETADDR;
        *(unsigned int *)(LCD_DATA_ADDR) = cur;
        needSetAddr = 0;
      }
      *(unsigned int *)(LCD_DATA_ADDR) = fb[cur];
    } else {
      needSetAddr = 1;
    }
    prev++;
  }
  
}

void resetFB() {
  memset(fb, 0, 20*240);
  memset(fb+20*240, 0xffff, 20*240);
  drawFB();
}
void GLCD_SetPixel(unsigned int x, unsigned int y, unsigned int color) {
  unsigned int  addr = y * WIDTH_WORDS + (x >> 4);
  unsigned int  bitPos = (15-(x&0x0f));

  unsigned int tmp = fb[addr];
  if(color) {
    tmp = tmp|(1<<bitPos);
  } else {
    tmp = tmp&(~(1<<bitPos));
  }
  fb[addr] = tmp;
}



void GLCD_Rectangle(unsigned int x, unsigned int y, unsigned int b, unsigned int a, unsigned int color)
{
  unsigned int j; // zmienna pomocnicza
  // rysowanie linii pionowych (boki)
  for (j = 0; j < a; j++) {
		GLCD_SetPixel(x, y + j, color);
		GLCD_SetPixel(x + b - 1, y + j, color);
	}
  // rysowanie linii poziomych (podstawy)
  for (j = 0; j < b; j++)	{
		GLCD_SetPixel(x + j, y, color);
		GLCD_SetPixel(x + j, y + a - 1, color);
	}
}
//

void GLCD_Circle(unsigned int cx, unsigned int cy ,unsigned int radius, unsigned int color)
{
int x, y, xchange, ychange, radiusError;
x = radius;
y = 0;
xchange = 1 - 2 * radius;
ychange = 1;
radiusError = 0;
while(x >= y)
  {
  GLCD_SetPixel(cx+x, cy+y, color); 
  GLCD_SetPixel(cx-x, cy+y, color); 
  GLCD_SetPixel(cx-x, cy-y, color);
  GLCD_SetPixel(cx+x, cy-y, color); 
  GLCD_SetPixel(cx+y, cy+x, color); 
  GLCD_SetPixel(cx-y, cy+x, color); 
  GLCD_SetPixel(cx-y, cy-x, color); 
  GLCD_SetPixel(cx+y, cy-x, color); 
  y++;
  radiusError += ychange;
  ychange += 2;
  if ( 2*radiusError + xchange > 0 )
    {
    x--;
	radiusError += xchange;
	xchange += 2;
	}
  }
}
//

void GLCD_Line(unsigned int X1,unsigned int Y1,unsigned int X2,unsigned int Y2, unsigned int color)
{
int CurrentX, CurrentY, Xinc, Yinc, 
    Dx, Dy, TwoDx, TwoDy, 
	TwoDxAccumulatedError, TwoDyAccumulatedError;

Dx = (X2-X1); // obliczenie sk³adowej poziomej
Dy = (Y2-Y1); // obliczenie sk³adowej pionowej

TwoDx = Dx + Dx; // podwojona sk³adowa pozioma
TwoDy = Dy + Dy; // podwojona sk³adowa pionowa

CurrentX = X1; // zaczynamy od X1
CurrentY = Y1; // oraz Y1

Xinc = 1; // ustalamy krok zwiêkszania pozycji w poziomie 
Yinc = 1; // ustalamy krok zwiêkszania pozycji w pionie

if(Dx < 0) // jesli sk³adowa pozioma jest ujemna 
  {
  Xinc = -1; // to bêdziemy siê "cofaæ" (krok ujemny)
  Dx = -Dx;  // zmieniamy znak sk³adowej na dodatni
  TwoDx = -TwoDx; // jak równie¿ podwojonej sk³adowej
  }

if (Dy < 0) // jeœli sk³adowa pionowa jest ujemna
  {
  Yinc = -1; // to bêdziemy siê "cofaæ" (krok ujemny)
  Dy = -Dy; // zmieniamy znak sk³adowej na dodatki
  TwoDy = -TwoDy; // jak równiez podwojonej sk³adowej
  }

GLCD_SetPixel(X1,Y1, color); // stawiamy pierwszy krok (zapalamy pierwszy piksel)

if ((Dx != 0) || (Dy != 0)) // sprawdzamy czy linia sk³ada siê z wiêcej ni¿ jednego punktu ;)
  {
  // sprawdzamy czy sk³adowa pionowa jest mniejsza lub równa sk³adowej poziomej
  if (Dy <= Dx) // jeœli tak, to idziemy "po iksach"
    { 
    TwoDxAccumulatedError = 0; // zerujemy zmienn¹ 
    do // ruszamy w drogê
	  {
      CurrentX += Xinc; // do aktualnej pozycji dodajemy krok 
      TwoDxAccumulatedError += TwoDy; // a tu dodajemy podwojon¹ sk³adow¹ pionow¹
      if(TwoDxAccumulatedError > Dx)  // jeœli TwoDxAccumulatedError jest wiêkszy od Dx
        {
        CurrentY += Yinc; // zwiêkszamy aktualn¹ pozycjê w pionie
        TwoDxAccumulatedError -= TwoDx; // i odejmujemy TwoDx
        }
       GLCD_SetPixel(CurrentX,CurrentY, color);// stawiamy nastêpny krok (zapalamy piksel)
       }while (CurrentX != X2); // idziemy tak d³ugo, a¿ osi¹gniemy punkt docelowy
     }
   else // w przeciwnym razie idziemy "po igrekach" 
      {
      TwoDyAccumulatedError = 0; 
      do 
	    {
        CurrentY += Yinc; 
        TwoDyAccumulatedError += TwoDx;
        if(TwoDyAccumulatedError>Dy) 
          {
          CurrentX += Xinc;
          TwoDyAccumulatedError -= TwoDy;
          }
         GLCD_SetPixel(CurrentX,CurrentY, color); 
         }while (CurrentY != Y2);
    }
  }
}
//




int main() {
  int i =0 ;
  resetFB();
  GLCD_Line(10,10,310,230,1);
  GLCD_Line(47,125,54,82,1);
  GLCD_Circle(50,180,45,1);

  GLCD_Rectangle(200, 150, 100, 70, 1);
/*
  for(i = 0; i<200;i++ ) {
    GLCD_SetPixel(i,i,1);
    GLCD_SetPixel(200-i,i,1);
  }

*/
  drawFB();

/*
while(1) {
  for(i = 0; i<20*240; i++) {
    *(unsigned int *)(LCD_DATA_ADDR) = 0x55;
  }
  for(i = 0; i<20*240; i++) {
    *(unsigned int *)(LCD_DATA_ADDR) = 0xaa00;
  }
  for(i = 0; i<20*240; i++) {
    *(unsigned int *)(LCD_DATA_ADDR) = 0xffff;
  }
  for(i = 0; i<20*240; i++) {
    *(unsigned int *)(LCD_DATA_ADDR) = 0x0000;
  }
}
*/


  return 0;
}

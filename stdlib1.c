#define SCR_CHARS 80
//#define OS_VIDEO_BUFFER 0xC10B8000
#define OS_VIDEO_BUFFER 0x00000000000B8000
//#define SCR_TEXT_BUF ((u16*)0xB8000)
#define SCR_TEXT_BUF ((u16*)OS_VIDEO_BUFFER)
#define SCR_ENTRY(ch,col) ((col<<8)|ch)
#define DEFAULT_COLOR BG_BLACK|7
#define BG_BLACK  (0<<4) // сдвинуть 0 на 4 разр€да влево.

static void draw_char(u16 sym,u32 color);

u32 s_iX;
u32 s_iY;
static u8 s_cCurAttr = DEFAULT_COLOR;


static int s_video_init = 0;
static u32 vga_palette[] = {
	0x000000,
	0x0000AA,
	0x00AA00,
	0x00AAAA,
	0xAA0000,
	0xAA00AA,
	0xAA5500,
	0xAAAAAA,
	
	0x555555,
	0x5555FF,
	0x55FF55,
	0x55FFFF,
	0xFF5555,
	0xFF55FF,
	0xFFFF55,
	0xFFFFFF,
};

void fprint(u64 number)
{
	u64 val; // тип имеющий одинаковую разр€дность на всех машинах
	char num[64];
	int i;
	
	//decimal
	i = 0;
	val = number>>16;
	while(val != 0)
	{
		num[i++] = '0'+(val%10);
		val /= 10;
	}
	num[i++] = '0'+(val%10);
	while(i != 0) printchar(num[--i]);
	
	//fraction
	i = 0;
	val = number&0xFFFF;
	printchar('.');
	while(val != 0)
	{
		val *= 10;
		printchar('0'+(val>>16));
		val &= 0xFFFF;
	}
}
void printchar(u16 c)
{	
	if(!s_video_init) return;
	if(c == '\n' || c == '\r' || c == '\t' || c == '\b') // Control characters
	{
		if(c == '\r') s_iX = 0;
		else if(c == '\n') s_iX = SCR_CHARS;
		else if(c == '\t') s_iX = (s_iX + 8) & ~7; // ~битова€ инверси€
		else if(c == '\b')
		{
			if(s_iX) s_iX--;
			else if(s_iY) 
			{
				s_iY--;
				s_iX = SCR_CHARS-1;
			}
			//Remove character
			//SCR_TEXT_BUF[s_iY*SCR_CHARS+s_iX] = SCR_ENTRY(0,s_cCurAttr);
			draw_char(0,0);
		}
	}
	else // Text characters
	{
		//SCR_TEXT_BUF[s_iY*SCR_CHARS+s_iX++] = SCR_ENTRY(c,s_cCurAttr);
		draw_char(c,vga_palette[s_cCurAttr]);
		s_iX++;
	}
	
	if(s_iX > SCR_CHARS-1)
	{
		s_iX = 0;
		if(s_iY < SCR_LINES-1)
			s_iY++;
		else shiftscreen();
	}
	
	update_cursor();
}
static void draw_char(u16 sym,u32 color)
{
	if(!sym && !color)
	{
		video_rect_zero(s_iX*lmem->vt_charw,s_iY*lmem->vt_charh,
			lmem->vt_charw,lmem->vt_charh);
	}		
	video_char(s_iX*lmem->vt_charw,s_iY*lmem->vt_charh,
		lmem->vt_charw,lmem->vt_charh,color,sym);
}
void update_cursor()
{
	/*u16 pos;
	pos = s_iY * 80 + s_iX;
 
	outb(0x3D4, 0x0F);
	outb(0x3D5, (u8) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (u8) ((pos >> 8) & 0xFF));*/
}
/*
===========================
  MIcro Hex EDitor v1.0.0

   Copyright (C) By CEPA
	2003.11.30
	 GNU GPL
===========================
*/

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define KEY_UP 18432
#define KEY_DOWN 20480
#define KEY_RIGHT 19712
#define KEY_LEFT 19200
#define KEY_ALT_X 11520
#define KEY_ALT_O 6144
#define KEY_ALT_C 11776
#define KEY_ALT_S 7936
#define KEY_ALT_F 8448
#define KEY_ALT_J 9216
#define KEY_PAGE_UP 18688
#define KEY_PAGE_DOWN 20736

int done = 0;
int key = 0;
int update = 0;
FILE *file = NULL;
long offset = 0,pointer = 0;
unsigned char buf[256];
char filename[74];
unsigned long size = 0;

int getkey()
{
  int key = 0;
  asm mov ax,0
  asm int 0x16
  asm mov key,ax
  return key;
}

void vputc(char c,int x,int y)
{
  char attr = 14;
  attr |= 1 << 4;
  asm mov ax,0xB800
  asm mov es,ax
  asm mov ax,y
  asm mov bx,80
  asm mul bx
  asm add ax,x
  asm mov bx,2
  asm mul bx
  asm mov di,ax
  asm mov ah,c
  asm mov es:[di],ah
  asm mov ah,attr
  asm add di,1
  asm mov es:[di],ah
}

void _vputc(char c,int x,int y)
{
  char attr = 15;
  attr |= 4 << 4;
  asm mov ax,0xB800
  asm mov es,ax
  asm mov ax,y
  asm mov bx,80
  asm mul bx
  asm add ax,x
  asm mov bx,2
  asm mul bx
  asm mov di,ax
  asm mov ah,c
  asm mov es:[di],ah
  asm mov ah,attr
  asm add di,1
  asm mov es:[di],ah
}

void print()
{
  int i = 0,x,y;

  gotoxy(1,1);
  textcolor(15);
  textbackground(1);
  clrscr();
  cprintf(
    "ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ[MIHED v1.0.0]อป"
    "บ    Size:                           Current Offset:                           บ"
    "บ Sectors:                          Current Pointer:                           บ"
    "บ Hex:       Int:      Bin:            Current Byte:                           บ"
    "บ----00-01-02-03-04-05-06-07-08-09-0A-0B-0C-0D-0E-0F------0123456789ABCDEF-----บ"
    "บ00-                                                 -00-                  -00 บ"
    "บ01-                                                 -01-                  -01 บ"
    "บ02-                                                 -02-                  -02 บ"
    "บ03-                                                 -03-                  -03 บ"
    "บ04-                                                 -04-                  -04 บ"
    "บ05-                                                 -05-                  -05 บ"
    "บ06-                                                 -06-                  -06 บ"
    "บ07-                                                 -07-                  -07 บ"
    "บ08-                                                 -08-                  -08 บ"
    "บ09-                                                 -09-                  -09 บ"
    "บ0A-                                                 -0A-                  -0A บ"
    "บ0B-                                                 -0B-                  -0B บ"
    "บ0C-                                                 -0C-                  -0C บ"
    "บ0D-                                                 -0D-                  -0D บ"
    "บ0E-                                                 -0E-                  -0E บ"
    "บ0F-                                                 -0F-                  -0F บ"
    "บ----00-01-02-03-04-05-06-07-08-09-0A-0B-0C-0D-0E-0F------0123456789ABCDEF-----บ"
    "บ    Alt+O:Open  Alt+C:Close  Alt+S:Set  Alt+F:Find  Alt+J:Jump  Alt+X:Exit    บ"
    "ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ"
    );

  gotoxy(3,1);
  cprintf("[Editing: %s]",filename);

  memset(buf,0,256);

  if(file)
  {
    textcolor(14);
    textbackground(1);
    gotoxy(12,2);
    cprintf("%d Bytes",size);
    gotoxy(12,3);
    cprintf("%d Sectors",((((size / 512) * 512) == size) ? size / 512 : size / 512 + 1));
    gotoxy(54,2);
    cprintf("%d",offset);
    gotoxy(54,3);
    cprintf("%d",pointer);
    gotoxy(54,4);
    cprintf("%d",offset + pointer);

    fseek(file,offset,SEEK_SET);
    fread(buf,1,256,file);
  }
  i = 0;
  for(y = 5; y < 21; y++)
  {
    for(x = 58; x < 74;x++)
    {
      if(i != pointer)
	vputc(buf[i],x,y);
      else
	_vputc(buf[i],x,y);
      i++;
    }
  }
  i = 0;
  for(y = 6; y < 22; y++)
  {
    for(x = 6; x < 52; x += 3)
    {
      if(pointer != i)
      {
	gotoxy(x,y);
	textcolor(14);
	textbackground(1);
	cprintf("%0.2x",buf[i]);
      }
      else
      {
	gotoxy(x,y);
	textcolor(15);
	textbackground(4);
	cprintf("%0.2x",buf[i]);
	gotoxy(8,4);
	textcolor(14);
	textbackground(1);
	cprintf("%xh",buf[i]);
	gotoxy(19,4);
	cprintf("%d",buf[i]);
	{
	  int temp = 0;
	  for(temp = 7; temp >= 0; temp--)
	  {
	    gotoxy(36 + -temp,4);
	    if(buf[i] & (unsigned char)pow(2,temp))
	      cprintf("1");
	    else
	      cprintf("0");
	  }
	}
      }
      i++;
    }
  }
  gotoxy(80,25);
}

unsigned long getlen()
{
  unsigned long len = 0;
  if(file)
  {
    fseek(file,0,SEEK_SET);
    while(fgetc(file) != EOF)
      len++;
  }
  return len;
}

int openfile(char *fname)
{
  sprintf(filename,"");
  if(!(file = fopen(fname,"rb+")))
    return 0;
  sprintf(filename,"%s",fname);
  size = getlen();
  return 1;
}

void closefile()
{
  fclose(file);
  sprintf(filename,"nofile");
  file = NULL;
}

int main(int argc,char **argv)
{
  update = 1;
  offset = pointer = 0;
  size = 0;
  sprintf(filename,"nofile");
  print();
  if(argv[1])
  {
    if(!openfile(argv[1]))
    {
      textcolor(4);
      textbackground(1);
      gotoxy(1,25);
      cprintf("Couldn't open file !");
      getkey();
    }
    size = getlen();
  }
  while(!done)
  {
    if(update == 1)
    {
      update = 0;
      print();
    }
    key = getkey();
    if(key == KEY_ALT_X)
    {
      done = 1;
    }
    if(key == KEY_UP)
    {
      if(file)
      {
	if(pointer - 16 >= 0)
	  pointer -= 16;
	else
	{
	  if(offset - 16 >= 0)
	    offset -= 16;
	  else
	    offset = 0;
	}
	update = 1;
      }
    }
    if(key == KEY_DOWN)
    {
      if(file)
      {
	if(pointer + 16 < 256)
	  pointer += 16;
	else
	{
	  offset += 16;
	}
	update = 1;
      }
    }
    if(key == KEY_LEFT)
    {
      if(file)
      {
	if(pointer - 1 >= 0)
	  pointer--;
	else
	{
	  if(file)
	  {
	    if(offset - 16 >= 0)
	    {
	      offset -= 16;
	      pointer = 16;
	    }
	    else
	      offset = pointer = 0;
	  }
	}
	update = 1;
      }
    }
    if(key == KEY_RIGHT)
    {
      if(file)
      {
	if(pointer + 1 < 256)
	  pointer++;
	else
	{
	  pointer -= 16;
	  offset += 16;
	}
	update = 1;
      }
    }
    if(key == KEY_ALT_O)
    {
      char fname[256];
      closefile();
      offset = pointer = 0;
      textcolor(14);
      textbackground(1);
      gotoxy(1,25);
      cprintf("Open file > ");
      cscanf("%s",fname);
      openfile(fname);
      if(file)
	print();
      else
      {
	print();
	textcolor(4);
	textbackground(1);
	gotoxy(1,25);
	cprintf("Couldn't open file !");
	getkey();
	print();
      }
    }
    if(key == KEY_ALT_C)
    {
      offset = pointer = 0;
      closefile();
      print();
    }
    if(key == KEY_ALT_S)
    {
      if(file)
      {
	int byte = 0;
	textcolor(14);
	textbackground(1);
	gotoxy(1,25);
	cprintf("Set Byte > ");
	cscanf("%d",&byte);
	fseek(file,offset + pointer,SEEK_SET);
	fputc(byte,file);
	print();
      }
    }
    if(key == KEY_ALT_F)
    {
      if(file && (offset + pointer < size))
      {
	char string[256];
	char temp[256];
	unsigned long i;
	int slen = 0;
	int tempdone = 0;
	textcolor(14);
	textbackground(1);
	gotoxy(1,25);
	cprintf("Find String > ");
	cscanf("%s",string);
	slen = strlen(string);
	i = offset + pointer;
	while(!tempdone)
	{
	  fseek(file,i,SEEK_SET);
	  fread(temp,1,slen,file);
	  if(strncmp(string,temp,slen) == 0)
	  {
	    offset = i;
	    pointer = 0;
	    tempdone = 1;
	  }
	  i++;
	  if(i > size)
	    tempdone = 1;
	}
	print();
      }
    }
    if(key == KEY_ALT_J)
    {
      unsigned long pos = 0;
      textcolor(14);
      textbackground(1);
      gotoxy(1,25);
      cprintf("Jump > ");
      cscanf("%d",&pos);
      offset = pos;
      pointer = 0;
      print();
    }
    if(key == KEY_PAGE_UP)
    {
      if(file)
      {
	if(offset - 256 > 0)
	  offset -= 256;
	else
	  offset = 0;
	print();
      }
    }
    if(key == KEY_PAGE_DOWN)
    {
      if(file)
      {
	offset += 256;
	print();
      }
    }
    /*
    textcolor(14);
    textbackground(1);
    gotoxy(1,25);
    cprintf("%d",key);
    */
  }
  closefile();
  textcolor(15);
  textbackground(0);
  clrscr();
  printf("MIcro Hex EDitor v1.0.0\nCopyright (C) By CEPA 2003\ncepa____@poczta.onet.pl\n");
  return 0;
}

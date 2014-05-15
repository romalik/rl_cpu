/*
 * This program is designed to be put onto an MSDOS filesystem floppy or 
 * a floppy image on a CD-ROM.
 *
 * The hexdump at the end is from Windows 95 boot sector and can be used
 * to start a Windows 9X io.sys.
 *
 * This program runs first and if the hard disk appears bootable will load
 * and start that unless you press return.
 *
 * If you press a key you have the option of Zapping the MBR!
 *
 */

#include <stdio.h>

#define sysboot_dosfs_stat 0x000B
#define sysboot_codestart 0x003E
#define sysboot_bootblock_magic 0x01FE

extern char win95_bb[512];
char bs_buf[512];
char buf2[512];
unsigned memseg, memlen;

fatal(str)
   char * str;
{
   cprintf(str);
   getch();
   exit(0);
}

main()
{
   int i, rv;
   int floppy_only = 0;
   int c,h,s,os;

   reset_screen();
   cprintf("...\n");
   relocate();
   if (__get_cs() != 0x80)
      fatal("ERROR - program incorrectly compiled.\n");

   for(i=0; i<6; i++)
     if (!(rv = _bios_disk_read(0x80, 0, 0, 1, 1, bs_buf))) break;

   if (rv != 0 || bs_buf[510] != 0x55 || bs_buf[511] != (char)0xAA) {
      cprintf("Hard disk not bootable.\n");
      floppy_only = 1;
   }

   if (!floppy_only) {
      for(rv=-1, i=0x1BE; i<0x1FE; i+= 16) {
	 if (bs_buf[i] == (char)0x80) {
	    rv = 0;
	    s = (bs_buf[i+2] & 63) ;
	    h = (bs_buf[i+1] & 255) ;
	    c = (bs_buf[i+3] & 255) + ((bs_buf[i+2] & 0xC0) << 2);

	    os = (bs_buf[i+4] & 255) ;
	    break;
	 }
      }

      if (rv) {
	 cprintf("Hard disk has no active partition.\n");
	 floppy_only = 1;
      }
   }

   if (!floppy_only && (os==4 || os==6 || os==11 || os==12 || os==13)) {
      for(i=0; i<6; i++)
	if (!(rv = _bios_disk_read(0x80, c, h, s, 1, bs_buf))) break;

      if (rv != 0 || bs_buf[510] != 0x55 || bs_buf[511] != (char)0xAA) {
	 cprintf("DOS Partition not bootable.\n");
	 floppy_only = 1;
      }
   }

   if (floppy_only)
      cprintf("Press return to wipe MBR: ");
   else
      cprintf("Press return to skip hard disk boot: ");

   __set_es(0x40);
   for(i=0; ; i++) {
      unsigned int tv = __deek_es(0x6c);
      while (tv == __deek_es(0x6c))
	 if (kbhit()) {
	    getch();
	    cprintf("\n");
	    goto break_break;
	 }
      if (i%10 == 0) cprintf(".");

      if (i>= 18*5) {
	 cprintf(" Booting.\n");
	 if(floppy_only)
	    boot_floppy();
	 else
	    boot_hd();
      }
   }
break_break:;

   cprintf("Do you want to leave the hard disk intact?\n");
   cprintf("(Y/n) ");
   i = (getch() & 0xFF);
   if (i == 'n' || i == 'N') {
      cprintf("No\n");

      cprintf("WARNING: This WILL delete everything on the hard disk!\n");
      cprintf("Do you want to clear the hard disk MBR?\n");
      cprintf("(N/y) ");

      i = (getch() & 0xFF);
      if (i == 'y' || i == 'Y') {
	 cprintf("Yes\n");
	 memset(bs_buf, 0, sizeof(bs_buf));

	 for(i=0; i<6; i++)
	   if (!(rv = _bios_disk_write(0x80, 0, 0, 1, 1, bs_buf))) break;

	 if (rv) {
	    cprintf("Disk error 0x%2x on disk write:", rv);
	    getch();
	    cprintf("\n");
	 } else {
	    cprintf("Hard disk MBR wiped!\n");
	 }
      } else 
	 cprintf("No -- Disk is still untouched\n");
   } else 
      cprintf("Ok -- Disk is untouched\n");

   boot_floppy();
}

reset_screen()
{
#asm
   mov	ah,#$0F
   int	$10
   cmp	al,#$07
   je	dont_touch
   mov	ax,#$0003
   int	$10
dont_touch:
#endasm
}

boot_floppy()
{
   _bios_disk_read(0, 0, 0, 1, 1, bs_buf);
   make_floppy_bb();

#asm
  mov	ax,#$00
  push	ax
  pop	ds
  mov	bx,#$7c00
  mov	dx,#$0000               ! Of the floppy drive
  jmpi	$7c00,0
#endasm
}

boot_hd()
{
   int i;
   /* If we're booting from a CD we want to turn off the floppy emulation */
   buf2[0] = 0x13;	/* Sizeof a 'Specification packet' */

#asm
  mov	ax,#$4B01
  mov	dl,#$7F
  mov	si,#_buf2
  int	$13
  ! Ignore the return value; it's meaningless if we aren't on a CD
#endasm

   /* Now boot the hard disk */
   __set_es(0x07c0);
   for(i=0; i<512; i++) __poke_es(i, bs_buf[i]);

#asm
  mov	ax,#$00
  push	ax
  pop	ds
  mov	bx,#$7c00
  mov	dx,#$0080               ! Of the hard drive
  jmpi	$7c00,0
#endasm
}

make_floppy_bb()
{
   int i;
   __set_es(0x07c0);
   for(i=0; i<sysboot_dosfs_stat; i++)
      __poke_es(i, win95_bb[i]);
   for(i=sysboot_dosfs_stat; i<sysboot_codestart; i++)
      __poke_es(i, bs_buf[i]);
   for(i=sysboot_codestart; i<512; i++)
      __poke_es(i, win95_bb[i]);
}

relocate()
{
#ifdef __STANDALONE__
   unsigned moved, codelen;
   unsigned es      = __get_es();
   unsigned newseg;

   /* Where do we start */
   if(memseg == 0)
   {
      extern int _heap_top;
      memseg = __get_cs();
      codelen = __get_ds()-memseg;
      __set_es(memseg-2);

      memlen = (((int)&_heap_top) >> 4);

      /*
      if (__deek_es(0) == 0x0301 ) memlen = (__deek_es(24) >> 4);
      */

      if( memlen == 0 ) memlen = 0x1000;
      memlen += codelen;
      __set_es(es);
   }

   newseg = 0x80;

   /* If the old area overlaps the new then fail */
   if( newseg >= memseg && newseg < memseg+memlen ) return;
   if( memseg >= newseg && memseg < newseg+memlen ) return;

   /* Copy segments, done in 32k chunks */
   for(moved=0; moved < memlen; )
   {
       unsigned int lump;
       if( memlen-moved <= 0x800 ) lump = memlen-moved; else lump = 0x800;

       __movedata(memseg+moved, 0, newseg+moved, 0, (lump<<4));
       moved += lump;
   }

   /* re-link int 0x80, this one is only an example (used by 'standalone.c') */
   /* __set_es(0); __doke_es(0x80*4+2, newseg); __set_es(es); */

   /* The actual jump ... */
   memseg = newseg;

#asm
   mov	ax,ds
   mov	bx,cs
   sub	ax,bx
   mov	bx,[_memseg]
   add	ax,bx
   push bx
   call	L_x
   mov	ds,ax
   mov	ss,ax
   mov	[_memseg],bx
#endasm
}
#asm
L_x:
   retf
#endasm

#else
}
#endif

char win95_bb[512] = {
0xeb,0x3c,0x90,0x29,0x69,0x71,0x22,0x5a,0x49,0x48,0x43,0x00,0x02,0x01,0x01,0x00,
0x02,0xe0,0x00,0x40,0x0b,0xf0,0x09,0x00,0x12,0x00,0x02,0x00,0x00,0x00,0x00,0x00,
0x40,0x0b,0x00,0x00,0x00,0x00,0x29,0xfa,0x16,0x58,0x2c,0x4e,0x4f,0x20,0x4e,0x41,
0x4d,0x45,0x20,0x20,0x20,0x20,0x46,0x41,0x54,0x31,0x32,0x20,0x20,0x20,0xfa,0x33,
0xc9,0x8e,0xd1,0xbc,0xfc,0x7b,0x16,0x07,0xbd,0x78,0x00,0xc5,0x76,0x00,0x1e,0x56,
0x16,0x55,0xbf,0x22,0x05,0x89,0x7e,0x00,0x89,0x4e,0x02,0xb1,0x0b,0xfc,0xf3,0xa4,
0x06,0x1f,0xbd,0x00,0x7c,0xc6,0x45,0xfe,0x0f,0x8b,0x46,0x18,0x88,0x45,0xf9,0x38,
0x4e,0x24,0x7d,0x22,0x8b,0xc1,0x99,0xe8,0x77,0x01,0x72,0x1a,0x83,0xeb,0x3a,0x66,
0xa1,0x1c,0x7c,0x66,0x3b,0x07,0x8a,0x57,0xfc,0x75,0x06,0x80,0xca,0x02,0x88,0x56,
0x02,0x80,0xc3,0x10,0x73,0xed,0x33,0xc9,0x8a,0x46,0x10,0x98,0xf7,0x66,0x16,0x03,
0x46,0x1c,0x13,0x56,0x1e,0x03,0x46,0x0e,0x13,0xd1,0x8b,0x76,0x11,0x60,0x89,0x46,
0xfc,0x89,0x56,0xfe,0xb8,0x20,0x00,0xf7,0xe6,0x8b,0x5e,0x0b,0x03,0xc3,0x48,0xf7,
0xf3,0x01,0x46,0xfc,0x11,0x4e,0xfe,0x61,0xbf,0x00,0x07,0xe8,0x23,0x01,0x72,0x39,
0x38,0x2d,0x74,0x17,0x60,0xb1,0x0b,0xbe,0xd8,0x7d,0xf3,0xa6,0x61,0x74,0x39,0x4e,
0x74,0x09,0x83,0xc7,0x20,0x3b,0xfb,0x72,0xe7,0xeb,0xdd,0xbe,0x7f,0x7d,0xac,0x98,
0x03,0xf0,0xac,0x84,0xc0,0x74,0x17,0x3c,0xff,0x74,0x09,0xb4,0x0e,0xbb,0x07,0x00,
0xcd,0x10,0xeb,0xee,0xbe,0x82,0x7d,0xeb,0xe5,0xbe,0x80,0x7d,0xeb,0xe0,0x98,0xcd,
0x16,0x5e,0x1f,0x66,0x8f,0x04,0xcd,0x19,0xbe,0x81,0x7d,0x8b,0x7d,0x1a,0x8d,0x45,
0xfe,0x8a,0x4e,0x0d,0xf7,0xe1,0x03,0x46,0xfc,0x13,0x56,0xfe,0xb1,0x04,0xe8,0xc1,
0x00,0x72,0xd6,0xea,0x00,0x02,0x70,0x00,0xb4,0x42,0xeb,0x2d,0x60,0x66,0x6a,0x00,
0x52,0x50,0x06,0x53,0x6a,0x01,0x6a,0x10,0x8b,0xf4,0x74,0xec,0x91,0x92,0x33,0xd2,
0xf7,0x76,0x18,0x91,0xf7,0x76,0x18,0x42,0x87,0xca,0xf7,0x76,0x1a,0x8a,0xf2,0x8a,
0xe8,0xc0,0xcc,0x02,0x0a,0xcc,0xb8,0x01,0x02,0x8a,0x56,0x24,0xcd,0x13,0x8d,0x64,
0x10,0x61,0x72,0x0a,0x40,0x75,0x01,0x42,0x03,0x5e,0x0b,0x49,0x75,0x77,0xc3,0x03,
0x18,0x01,0x27,0x0d,0x0a,0x49,0x6e,0x76,0x61,0x6c,0x69,0x64,0x20,0x73,0x79,0x73,
0x74,0x65,0x6d,0x20,0x64,0x69,0x73,0x6b,0xff,0x0d,0x0a,0x44,0x69,0x73,0x6b,0x20,
0x49,0x2f,0x4f,0x20,0x65,0x72,0x72,0x6f,0x72,0xff,0x0d,0x0a,0x52,0x65,0x70,0x6c,
0x61,0x63,0x65,0x20,0x74,0x68,0x65,0x20,0x64,0x69,0x73,0x6b,0x2c,0x20,0x61,0x6e,
0x64,0x20,0x74,0x68,0x65,0x6e,0x20,0x70,0x72,0x65,0x73,0x73,0x20,0x61,0x6e,0x79,
0x20,0x6b,0x65,0x79,0x0d,0x0a,0x00,0x00,0x49,0x4f,0x20,0x20,0x20,0x20,0x20,0x20,
0x53,0x59,0x53,0x4d,0x53,0x44,0x4f,0x53,0x20,0x20,0x20,0x53,0x59,0x53,0x7f,0x01,
0x00,0x41,0xbb,0x00,0x07,0x80,0x7e,0x02,0x0e,0xe9,0x40,0xff,0x00,0x00,0x55,0xaa
};

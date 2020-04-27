#ifndef _ANSITOUNICODE_
#define _ANSITOUNICODE_

extern  unsigned short GBKToUnicode(unsigned char* Ansi_str, unsigned short* unicode) ;
extern void GBKToUnicodeHexString(unsigned char* Ansi, unsigned char* unicode_str);

extern void UnicodeToUtf8(unsigned short unicode, unsigned char* Utf8);
extern unsigned short Utf8ToUnicode(unsigned char* Utf8);

extern void GBKStringToUTF8(char * GBK, char* UTF8);
extern void UTF8StringToGBK(char * UTF8, char* GBK);
extern unsigned short GetUtf8StringCharsCount(char * UTF8);
#endif

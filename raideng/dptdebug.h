/* Copyright (c) 1996-2004, Adaptec Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of the Adaptec Corporation nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *      Standard debugger handler.
 *
 *	Author: Mark Salyzyn
 */

/* Is it SMOR? */
#if (defined(DEBUG))
# if (defined(I2O_TOO)||defined(NO_MOUSE)||defined(REQUEST_SENSE_TEST)||defined(FLASH_HBA)||defined(DO_DRIVER)||defined(ENABLE_FORMATS)||defined(ZAP_DRIVES)||defined(REBUILD_ARRAY)||defined(MINIMAL_BUILD))
   /*
    *   SMOR's screen write capability.
    */
   extern "C" {
       extern void AbsoluteWrite(int x, int y, int attribute, char * string);
       extern unsigned sleep(unsigned seconds);
   }

#  if (!defined(DEBUG_SCREEN_WIDTH))
#      define DEBUG_SCREEN_WIDTH  50
#  endif
#  if (!defined(DEBUG_SCREEN_HEIGHT))
#      define DEBUG_SCREEN_HEIGHT 1
#  endif

   STATIC INLINE void printCharacter(char character)
   {   char Buffer[2];
       static char CurrentPosition;
#      if (defined(DEBUG_SCREEN_HEIGHT))
#          if (DEBUG_SCREEN_HEIGHT > 1)
               static char CurrentLine;
#              define printCharacterLine  (25-DEBUG_SCREEN_HEIGHT)
#          else
#              define CurrentLine         0
#          endif
#      else
#          define CurrentLine             0
#      endif
       static char DoNotClear;
#      define printCharacterStart (80-DEBUG_SCREEN_WIDTH)

       if ((CurrentPosition < printCharacterStart) || (CurrentPosition > 80)) {
           CurrentPosition = printCharacterStart;
       }
#      if (defined(DEBUG_SCREEN_HEIGHT))
#          if (DEBUG_SCREEN_HEIGHT > 1)
               if ((CurrentLine < printCharacterLine) || (CurrentLine > 25)) {
                   CurrentLine = printCharacterLine;
               }
#          endif
#      endif
       switch (character) {
       case '\b':
           if (CurrentPosition > printCharacterStart) {
               --CurrentPosition;
           }
           break;
       case '\t':
           if (CurrentPosition < 80) {
               CurrentPosition = (((CurrentPosition & ~7) | 7) + 1);
           }
           break;
       case '\n':
#          if (defined(DEBUG_SCREEN_HEIGHT))
#              if (DEBUG_SCREEN_HEIGHT > 1)
                   if (++CurrentLine >= 25) {
                       CurrentLine = printCharacterLine;
                       sleep (5);
                   }
#              else
                   sleep(2);
#              endif
#          else
               sleep(2);
#          endif
           DoNotClear = (char)-1;
       case '\r':
           ++DoNotClear;
           CurrentPosition = printCharacterStart;
           break;
       default:
           Buffer[1] = '\0';
           if (((DoNotClear == 0)
             && (CurrentPosition == printCharacterStart))
            || (CurrentPosition >= 80)) {
               unsigned char Position = printCharacterStart;

               if (CurrentPosition >= 80) {
#                 if (defined(DEBUG_SCREEN_HEIGHT))
#                     if (DEBUG_SCREEN_HEIGHT > 1)
                           if (CurrentLine >= 25) {
                               CurrentLine = printCharacterLine;
                               sleep (5);
                           }
#                      else
                           sleep (2);
#                      endif
#                  else
                       sleep (2);
#                  endif
               }
               Buffer[0] = ' ';
               while (Position < 80) {
                   AbsoluteWrite(Position++, CurrentLine, 113, Buffer);
               }
               if (CurrentPosition >= 80) {
                   CurrentPosition = printCharacterStart + 2;
                   Buffer[0] = '+';
                   AbsoluteWrite(printCharacterStart, CurrentLine, 113, Buffer);
               }
           }
           DoNotClear = 0;
           Buffer[0] = character;
           AbsoluteWrite(CurrentPosition++, CurrentLine, 113, Buffer);
       }
#      undef CurrentLine
   }

   STATIC INLINE void printString(char * string)
   {
       while (*string) {
           printCharacter (*string++);
       }
   }

   STATIC INLINE void printNumber(unsigned long value, int base, int length)
   {   unsigned char   buffer[12];
       unsigned char * cp = &buffer[(sizeof(buffer) / sizeof(buffer[0])) - 1];
       unsigned long   b = base;
       unsigned char   l = length;

       if (base < 0) {
           b = -base;
       }
       if (length < 0) {
           l = -length;
       }
       if (l >= sizeof(buffer)) {
           l = sizeof(buffer) - 1;
       }
       *cp = '\0';
       if (value) do {
           *(--cp) = (unsigned char)(value % b);
           if (*cp > 9) {
               *cp += 'A' - '9' - 1;
           }
           *cp += '0';
       } while ((--l > 0) && (value /= b));
       else {
           *(--cp) = '0';
           --l;
       }
       if (length > 0) while (l > 0) {
           *(--cp) = (base < 0) ? '0' : ' ';
           --l;
       }
       printString((char *)cp);
       while ((l > 0) && (l < 80)) {
           printString (" ");
           --l;
       }
   }

#  if (!defined(DEBUG_PUTS))
#   define DEBUG_PUTS(string)              printString((char *)string)
#  endif

#  if (!defined(DEBUG_NUMBER))
#   define DEBUG_NUMBER(value,base,length) printNumber(value,base,length)
#  endif

#  if (!defined(DEBUG_ARRAY))
#   define DEBUG_ARRAY(pointer,type,size)  \
        debugArray((unsigned char __FAR__ *)(pointer),type,(unsigned)(size))
#   define DEBUG_array
#  endif

#  if (!defined(DEBUG_strlen))
#   define DEBUG_strlen(string) strlen((char *)string)
#  endif
# elif (defined(__BORLANDC__) && defined(FAR_DATA)) /* Is it I2ODDL? */
#  include "iobios.h"

#  if (!defined(DEBUG_PUTS))
#   define DEBUG_PUTS(string)              printString(string)
#  endif

#  if (!defined(DEBUG_NUMBER))
#   define DEBUG_NUMBER(value,base,length) printNumber(value,base,length)
#  endif

#  if (!defined(DEBUG_ARRAY))
#   define DEBUG_ARRAY(pointer,type,size)  \
        debugArray((unsigned char __FAR__ *)(pointer),type,(unsigned)(size))
#   define DEBUG_array
#  endif

# endif /* !SMOR? && !I2ODDL? */
#endif /* DEBUG */

#if (defined(DEBUG) || defined(DEBUG_PUTS) || defined(DEBUG_NUMBER) || defined(DEBUG_FUNCTION))
# if (!defined(DEBUG_PUTS))
#  define DEBUG_PUTS(string)
# endif
# if (!defined(DEBUG_NUMBER))
#  define DEBUG_NUMBER(value,base,length)
# endif
# if (!defined(DEBUG_FUNCTION))
#  define DEBUG_FUNCTION(tag)
# endif
# if (!defined(DEBUG_ARRAY))
#  define DEBUG_ARRAY(pointer,type,size)
#  undef DEBUG_array
# endif
# if (!defined(DEBUG_SCREEN_WIDTH))
#  define DEBUG_SCREEN_WIDTH 80
# endif
/*
 *      buffer tags
 */
# define TAG_NULL         0
# define TAG_IN           1
# define TAG_OUT          2
# define TAG_INTERMEDIATE 3

/*
 *      buffer types
 */
# define TYPE_NULL      0
# define TYPE_STRING    1
# define TYPE_S8        2
# define TYPE_U8        3
# define TYPE_X8        4
# define TYPE_S16       5
# define TYPE_U16       6
# define TYPE_X16       7
# define TYPE_S32       8
# define TYPE_U32       9
# define TYPE_X32       10
# define TYPE_STRUCT    11

typedef struct {
    unsigned short size:8;      /* May grow             */
#   define DebugEventMaxSize 255
    unsigned short type:4;      /* May grow             */
    unsigned short tag:4;       /* Bound to grow        */
    unsigned char  buffer[1];
} DebugEvent_S;

PUBLIC unsigned long DebugEvent[1024];
PUBLIC DebugEvent_S * In_DebugEvent = (DebugEvent_S *)DebugEvent;

# if (!defined(DEBUG_strlen))
#  define DEBUG_strlen(string) Strlen((unsigned char *)string)
   STATIC INLINE unsigned
   Strlen (unsigned char * String)
   {   unsigned count = 0;

       while (*(String++)) {
           ++count;
       }
       return (count);
   }
# endif

# if (defined(DEBUG_array))
   STATIC INLINE void
   debugArray(
       unsigned char __FAR__ * pointer,
       unsigned char           element,
       unsigned short          size)
   {   unsigned                column;
       unsigned long           value;
       int                     base, length, minus, first;

       for (first = 1; size != 0;--size) {
           minus = 0;
           switch (element) {
           case TYPE_S8:
               if (*((char __FAR__ *)pointer) < 0) {
                   ++minus;
                   value = -*((char __FAR__ *)pointer);
               } else {
           case TYPE_U8:
                   value = *pointer;
               }
            ++pointer;
               base = 10;
               length = 3;
               break;
           case TYPE_X8:
               value = *(pointer++);
               base = -16;
               length = 2;
               break;
           case TYPE_S16:
               if (*((short __FAR__ *)pointer) < 0) {
                   ++minus;
                   value = -*((short __FAR__ *)pointer);
               } else {
           case TYPE_U16:
                   value = *((unsigned short __FAR__ *)pointer);
               }
            pointer = (unsigned char __FAR__ *)
              (((short __FAR__ *)pointer) + 1);
               base = 10;
               length = 5;
               break;
           case TYPE_X16:
               value = *((unsigned short __FAR__ *)pointer);
            pointer = (unsigned char __FAR__ *)
              (((unsigned short __FAR__ *)pointer) + 1);
               base = -16;
               length = 4;
               break;
           case TYPE_S32:
               if (*((long __FAR__ *)pointer) < 0) {
                   ++minus;
                   value = -*((long __FAR__ *)pointer);
               } else {
           case TYPE_U32:
                   value = *((unsigned long __FAR__ *)pointer);
               }
            pointer = (unsigned char __FAR__ *)
              (((unsigned long __FAR__ *)pointer) + 1);
               base = 10;
               length = 10;
               break;
           case TYPE_X32:
               value = *((unsigned long __FAR__ *)pointer);
            pointer = (unsigned char __FAR__ *)
              (((unsigned long __FAR__ *)pointer) + 1);
               base = -16;
               length = 8;
               break;
           }
           if (first) {
               DEBUG_PUTS ("{");
               first = 0;
               column = DEBUG_SCREEN_WIDTH;
           } else {
               DEBUG_PUTS (",");
               ++column;
           }
           if ((column + length + minus + 1) >= DEBUG_SCREEN_WIDTH) {
               DEBUG_PUTS ("\r\n    ");
               column = 4;
           } else {
               DEBUG_PUTS (" ");
               ++column;
           }
           if (minus) {
               DEBUG_PUTS ("-");
               ++column;
           }
           DEBUG_NUMBER (value, base, length);
           column += length;
       }
       if (first == 0) {
           if ((column + 2) >= DEBUG_SCREEN_WIDTH) {
               DEBUG_PUTS ("\r\n");
           } else {
               DEBUG_PUTS (" ");
           }
           DEBUG_PUTS ("}");
       }
   }
# endif

# define DEBUG_ALLOCATE_STRING(Name,String) static unsigned char Name[]=String
# define DEBUG_add(Type,Tag,Buffer,Size)                                     \
    In_DebugEvent->type = (Type);                                            \
    In_DebugEvent->tag = (Tag);                                              \
    if ((In_DebugEvent->size = (Size)) != 0) {                               \
        osdBcopy((void __FAR__ *)(Buffer),                                   \
          (void __FAR__ *)In_DebugEvent->buffer,(unsigned)(Size));           \
        In_DebugEvent = (DebugEvent_S *)(((unsigned long *)In_DebugEvent)    \
          + 1 + ((sizeof(long) + (Size) - sizeof(short) - 1)/sizeof(long))); \
    } else {                                                                 \
        In_DebugEvent = (DebugEvent_S *)(((unsigned long *)In_DebugEvent)+1);\
    }
# define DEBUG_ADD(Type,Tag,Buffer,Size)                                     \
    for (;;) {                                                               \
        if ((((unsigned long *)In_DebugEvent) < DebugEvent)                  \
         || (&DebugEvent[sizeof(DebugEvent)/sizeof(DebugEvent[0])]           \
          <= ((unsigned long *)In_DebugEvent))) {                            \
            In_DebugEvent = (DebugEvent_S *)DebugEvent;                      \
        }                                                                    \
        if (&DebugEvent[sizeof(DebugEvent)/sizeof(DebugEvent[0])]            \
         >= (((Size) != 0)                                                   \
         ? (((unsigned long *)In_DebugEvent) + 1 + ((sizeof(long) + (Size)   \
          - sizeof(short) - 1) / sizeof(long)))                              \
         : (((unsigned long *)In_DebugEvent) + 1))) {                        \
             break;                                                          \
        }                                                                    \
        DEBUG_add(TYPE_NULL,TAG_NULL,0,0);                                   \
    }                                                                        \
    DEBUG_add(Type,Tag,Buffer,Size)
/*
 *      This macro deals with ensuring that the size of the transfer is
 * limited to the maximum size that can be stored on a Debug Event. It
 * creates many unreachable code complaints due to the possibility of Size
 * being a constant (but not necessarily a pre-processor time constant :-( ).
# define DEBUG_ADD(Type,Tag,Buffer,Size)                                     \
    if (Size > DebugEventMaxSize) {                                          \
        unsigned size;                                                       \
        unsigned char *cp;                                                   \
        for (size = Size, cp = Buffer;                                       \
          size > (DebugEventMaxSize-3);                                      \
          cp += (DebugEventMaxSize-3), size -= (DebugEventMaxSize-3)) {      \
            DEBUG_Add(Type,Tag,cp,(DebugEventMaxSize-3));                    \
        }                                                                    \
        if (size > 0) {                                                      \
            DEBUG_Add(Type,Tag,cp,size);                                     \
        }                                                                    \
    } else {                                                                 \
        DEBUG_Add(Type,Tag,Buffer,Size);                                     \
    }
 */
# define DEBUG_TAG(Tag)                  DEBUG_ADD(TYPE_NULL,Tag,0,0);       \
                                         DEBUG_FUNCTION(Tag)
# define DEBUG_IN(Type,Buffer,Size)      DEBUG_ADD(Type,TAG_IN,Buffer,Size)
# define DEBUG_OUT(Type,Buffer,Size)     DEBUG_ADD(Type,TAG_OUT,Buffer,Size)
# define DEBUG_IN_S8(Val) {                                                  \
    char val = (char)(Val);                                                  \
    DEBUG_IN(TYPE_S8,&val,1);                                                \
    if (val < 0) {                                                           \
        DEBUG_PUTS("-");                                                     \
        val = -val;                                                          \
    }                                                                        \
    DEBUG_NUMBER(val,10,0);                                                  \
    DEBUG_PUTS(",");                                                         \
}
# define DEBUG_IN_U8(Val) {                                                  \
    unsigned char val = (unsigned char)(Val);                                \
    DEBUG_IN(TYPE_U8,&val,1);                                                \
    DEBUG_NUMBER(val,10,0);                                                  \
    DEBUG_PUTS(",");                                                         \
}
# define DEBUG_IN_X8(Val) {                                                  \
    unsigned char val = (unsigned char)(Val);                                \
    DEBUG_IN(TYPE_X8,&val,1);                                                \
    DEBUG_NUMBER(val,16,0);                                                  \
    DEBUG_PUTS(",");                                                         \
}
# define DEBUG_IN_S16(Val) {                                                 \
    short val = (short)(Val);                                                \
    DEBUG_IN(TYPE_S16,&val,1);                                               \
    if (val < 0) {                                                           \
        DEBUG_PUTS("-");                                                     \
        val = -val;                                                          \
    }                                                                        \
    DEBUG_NUMBER(val,10,0);                                                  \
    DEBUG_PUTS(",");                                                         \
}
# define DEBUG_IN_U16(Val) {                                                 \
    unsigned short val = (unsigned short)(Val);                              \
    DEBUG_IN(TYPE_U16,&val,1);                                               \
    DEBUG_NUMBER(val,10,0);                                                  \
    DEBUG_PUTS(",");                                                         \
}
# define DEBUG_IN_X16(Val) {                                                 \
    unsigned short val = (unsigned short)(Val);                              \
    DEBUG_IN(TYPE_X16,&val,1);                                               \
    DEBUG_NUMBER(val,16,0);                                                  \
    DEBUG_PUTS(",");                                                         \
}
# define DEBUG_IN_S32(Val) {                                                 \
    long val = (long)(Val);                                                  \
    DEBUG_IN(TYPE_S32,&val,1);                                               \
    if (val < 0) {                                                           \
        DEBUG_PUTS("-");                                                     \
        val = -val;                                                          \
    }                                                                        \
    DEBUG_NUMBER(val,10,0);                                                  \
    DEBUG_PUTS(",");                                                         \
}
# define DEBUG_IN_U32(Val) {                                                 \
    unsigned long val = (unsigned long)(Val);                                \
    DEBUG_IN(TYPE_U32,&val,1);                                               \
    DEBUG_NUMBER(val,10,0);                                                  \
    DEBUG_PUTS(",");                                                         \
}
# define DEBUG_IN_X32(Val) {                                                 \
    unsigned long val = (unsigned long)(Val);                                \
    DEBUG_IN(TYPE_X32,&val,1);                                               \
    DEBUG_NUMBER(val,16,0);                                                  \
    DEBUG_PUTS(",");                                                         \
}
# define DEBUG_IN_ARRAY_S8(Buffer,Size)  DEBUG_IN(TYPE_S8,Buffer,(Size));    \
                                         DEBUG_ARRAY(Buffer,TYPE_S8,(Size)); \
                                         DEBUG_PUTS(",");
# define DEBUG_IN_ARRAY_U8(Buffer,Size)  DEBUG_IN(TYPE_U8,Buffer,(Size));    \
                                         DEBUG_ARRAY(Buffer,TYPE_U8,(Size)); \
                                         DEBUG_PUTS(",");
# define DEBUG_IN_ARRAY_X8(Buffer,Size)  DEBUG_IN(TYPE_X8,Buffer,(Size));    \
                                         DEBUG_ARRAY(Buffer,TYPE_X8,(Size)); \
                                         DEBUG_PUTS(",");
# define DEBUG_IN_ARRAY_S16(Buffer,Size) DEBUG_IN(TYPE_S16,Buffer,(Size)*2); \
                                         DEBUG_ARRAY(Buffer,TYPE_S16,(Size));\
                                         DEBUG_PUTS(",");
# define DEBUG_IN_ARRAY_U16(Buffer,Size) DEBUG_IN(TYPE_U16,Buffer,(Size)*2); \
                                         DEBUG_ARRAY(Buffer,TYPE_U16,(Size));\
                                         DEBUG_PUTS(",");
# define DEBUG_IN_ARRAY_X16(Buffer,Size) DEBUG_IN(TYPE_X16,Buffer,(Size)*2); \
                                         DEBUG_ARRAY(Buffer,TYPE_X16,(Size));\
                                         DEBUG_PUTS(",");
# define DEBUG_IN_ARRAY_S32(Buffer,Size) DEBUG_IN(TYPE_S32,Buffer,(Size)*4); \
                                         DEBUG_ARRAY(Buffer,TYPE_S32,(Size));\
                                         DEBUG_PUTS(",");
# define DEBUG_IN_ARRAY_U32(Buffer,Size) DEBUG_IN(TYPE_U32,Buffer,(Size)*4); \
                                         DEBUG_ARRAY(Buffer,TYPE_U32,(Size));\
                                         DEBUG_PUTS(",");
# define DEBUG_IN_ARRAY_X32(Buffer,Size) DEBUG_IN(TYPE_X32,Buffer,(Size)*4); \
                                         DEBUG_ARRAY(Buffer,TYPE_X32,(Size));\
                                         DEBUG_PUTS(",");
# define DEBUG_IN_STRUCT(Struct)                                             \
         DEBUG_IN(TYPE_STRUCT,Struct,sizeof(*(Struct)));                     \
         DEBUG_ARRAY(Struct,TYPE_X8,sizeof(*(Struct)));                      \
         DEBUG_PUTS(",")
# define DEBUG_IN_STRING(String)                                             \
          DEBUG_IN(TYPE_STRING,String,DEBUG_strlen(String));                 \
          DEBUG_PUTS(String);                                                \
          DEBUG_PUTS(",")

# define DEBUG_OUT_S8(Val) {                                                 \
    char val = (char)(Val);                                                  \
    DEBUG_OUT(TYPE_S8,&val,1);                                               \
    DEBUG_PUTS("\r\n");                                                      \
    if (val < 0) {                                                           \
        DEBUG_PUTS("-");                                                     \
        val = -val;                                                          \
    }                                                                        \
    DEBUG_NUMBER(val,10,0);                                                  \
    DEBUG_PUTS("=");                                                         \
}
# define DEBUG_OUT_U8(Val) {                                                 \
    unsigned char val = (unsigned char)(Val);                                \
    DEBUG_OUT(TYPE_U8,&val,1);                                               \
    DEBUG_PUTS("\r\n");                                                      \
    DEBUG_NUMBER(val,10,0);                                                  \
    DEBUG_PUTS("=");                                                         \
}
# define DEBUG_OUT_X8(Val) {                                                 \
    unsigned char val = (unsigned char)(Val);                                \
    DEBUG_OUT(TYPE_X8,&val,1);                                               \
    DEBUG_PUTS("\r\n");                                                      \
    DEBUG_NUMBER(val,16,0);                                                  \
    DEBUG_PUTS("=");                                                         \
}
# define DEBUG_OUT_S16(Val) {                                                \
    short val = (short)(Val);                                                \
    DEBUG_OUT(TYPE_S16,&val,1);                                              \
    DEBUG_PUTS("\r\n");                                                      \
    if (val < 0) {                                                           \
        DEBUG_PUTS("-");                                                     \
        val = -val;                                                          \
    }                                                                        \
    DEBUG_NUMBER(val,10,0);                                                  \
    DEBUG_PUTS("=");                                                         \
}
# define DEBUG_OUT_U16(Val) {                                                \
    unsigned short val = (unsigned short)(Val);                              \
    DEBUG_OUT(TYPE_U16,&val,1);                                              \
    DEBUG_PUTS("\r\n");                                                      \
    DEBUG_NUMBER(val,10,0);                                                  \
    DEBUG_PUTS("=");                                                         \
}
# define DEBUG_OUT_X16(Val) {                                                \
    unsigned short val = (unsigned short)(Val);                              \
    DEBUG_OUT(TYPE_X16,&val,1);                                              \
    DEBUG_PUTS("\r\n");                                                      \
    DEBUG_NUMBER(val,16,0);                                                  \
    DEBUG_PUTS("=");                                                         \
}
# define DEBUG_OUT_S32(Val) {                                                \
    long val = (long)(Val);                                                  \
    DEBUG_OUT(TYPE_S32,&val,1);                                              \
    DEBUG_PUTS("\r\n");                                                      \
    if (val < 0) {                                                           \
        DEBUG_PUTS("-");                                                     \
        val = -val;                                                          \
    }                                                                        \
    DEBUG_NUMBER(val,10,0);                                                  \
    DEBUG_PUTS("=");                                                         \
}
# define DEBUG_OUT_U32(Val) {                                                \
    unsigned long val = (unsigned long)(Val);                                \
    DEBUG_OUT(TYPE_U32,&val,1);                                              \
    DEBUG_PUTS("\r\n");                                                      \
    DEBUG_NUMBER(val,10,0);                                                  \
    DEBUG_PUTS("=");                                                         \
}
# define DEBUG_OUT_X32(Val) {                                                \
    unsigned long val = (unsigned long)(Val);                                \
    DEBUG_OUT(TYPE_X32,&val,1);                                              \
    DEBUG_PUTS("\r\n");                                                      \
    DEBUG_NUMBER(val,16,0);                                                  \
    DEBUG_PUTS("=");                                                         \
}
# define DEBUG_OUT_ARRAY_S8(Buffer,Size)  DEBUG_OUT(TYPE_S8,Buffer,(Size));  \
                                          DEBUG_PUTS("\r\n");                \
                                          DEBUG_ARRAY(Buffer,TYPE_S8,(Size));\
                                          DEBUG_PUTS("=")
# define DEBUG_OUT_ARRAY_U8(Buffer,Size)  DEBUG_OUT(TYPE_U8,Buffer,(Size));  \
                                          DEBUG_PUTS("\r\n");                \
                                          DEBUG_ARRAY(Buffer,TYPE_U8,(Size));\
                                          DEBUG_PUTS("=")
# define DEBUG_OUT_ARRAY_X8(Buffer,Size)  DEBUG_OUT(TYPE_X8,Buffer,(Size));  \
                                          DEBUG_PUTS("\r\n");                \
                                          DEBUG_ARRAY(Buffer,TYPE_X8,(Size));\
                                          DEBUG_PUTS("=")
# define DEBUG_OUT_ARRAY_S16(Buffer,Size) DEBUG_OUT(TYPE_S16,Buffer,(Size)*2);\
                                          DEBUG_PUTS("\r\n");                \
                                          DEBUG_ARRAY(Buffer,TYPE_S16,(Size));\
                                          DEBUG_PUTS("=")
# define DEBUG_OUT_ARRAY_U16(Buffer,Size) DEBUG_OUT(TYPE_U16,Buffer,(Size)*2);\
                                          DEBUG_PUTS("\r\n");                \
                                          DEBUG_ARRAY(Buffer,TYPE_U16,(Size));\
                                          DEBUG_PUTS("=")
# define DEBUG_OUT_ARRAY_X16(Buffer,Size) DEBUG_OUT(TYPE_X16,Buffer,(Size)*2);\
                                          DEBUG_PUTS("\r\n");                \
                                          DEBUG_ARRAY(Buffer,TYPE_X16,(Size));\
                                          DEBUG_PUTS("=")
# define DEBUG_OUT_ARRAY_S32(Buffer,Size) DEBUG_OUT(TYPE_S32,Buffer,(Size)*4);\
                                          DEBUG_ARRAY(Buffer,TYPE_S32,(Size));\
                                          DEBUG_PUTS("=")
# define DEBUG_OUT_ARRAY_U32(Buffer,Size) DEBUG_OUT(TYPE_U32,Buffer,(Size)*4);\
                                          DEBUG_PUTS("\r\n");                \
                                          DEBUG_ARRAY(Buffer,TYPE_U32,(Size));\
                                          DEBUG_PUTS("=")
# define DEBUG_OUT_ARRAY_X32(Buffer,Size) DEBUG_OUT(TYPE_X32,Buffer,(Size)*4);\
                                          DEBUG_PUTS("\r\n");                \
                                          DEBUG_ARRAY(Buffer,TYPE_X32,(Size));\
                                          DEBUG_PUTS("=")
# define DEBUG_OUT_STRUCT(Struct)                                            \
          DEBUG_OUT(TYPE_STRUCT,Struct,sizeof(*(Struct)));                   \
          DEBUG_PUTS("\r\n");                                                \
          DEBUG_ARRAY(Buffer,TYPE_X8,sizeof(*(Struct)));                     \
          DEBUG_PUTS("=")
# define DEBUG_OUT_STRING(String)                                            \
          DEBUG_OUT(TYPE_STRING,String,DEBUG_strlen(String));                \
          DEBUG_PUTS("\r\n");                                                \
          DEBUG_PUTS(String);                                                \
          DEBUG_PUTS("=")
#else /* !DEBUG || !DEBUG_PUTS || !DEBUG_NUMBER && !DEBUG_FUNCTION */
# define DEBUG_ALLOCATE_STRING(Name,String)
# define DEBUG_TAG(Tag)
# define DEBUG_IN(Type,Buffer,Size)
# define DEBUG_OUT(Type,Buffer,Size)
# define DEBUG_IN_S8(Val)
# define DEBUG_IN_U8(Val)
# define DEBUG_IN_X8(Val)
# define DEBUG_IN_S16(Val)
# define DEBUG_IN_U16(Val)
# define DEBUG_IN_X16(Val)
# define DEBUG_IN_S32(Val)
# define DEBUG_IN_U32(Val)
# define DEBUG_IN_X32(Val)
# define DEBUG_IN_ARRAY_S8(Buffer,Size)
# define DEBUG_IN_ARRAY_U8(Buffer,Size)
# define DEBUG_IN_ARRAY_X8(Buffer,Size)
# define DEBUG_IN_ARRAY_S16(Buffer,Size)
# define DEBUG_IN_ARRAY_U16(Buffer,Size)
# define DEBUG_IN_ARRAY_X16(Buffer,Size)
# define DEBUG_IN_ARRAY_S32(Buffer,Size)
# define DEBUG_IN_ARRAY_U32(Buffer,Size)
# define DEBUG_IN_ARRAY_X32(Buffer,Size)
# define DEBUG_IN_STRUCT(Struct)
# define DEBUG_IN_STRING(String)
# define DEBUG_OUT_S8(Val)
# define DEBUG_OUT_U8(Val)
# define DEBUG_OUT_X8(Val)
# define DEBUG_OUT_S16(Val)
# define DEBUG_OUT_U16(Val)
# define DEBUG_OUT_X16(Val)
# define DEBUG_OUT_S32(Val)
# define DEBUG_OUT_U32(Val)
# define DEBUG_OUT_X32(Val)
# define DEBUG_OUT_ARRAY_S8(Buffer,Size)
# define DEBUG_OUT_ARRAY_U8(Buffer,Size)
# define DEBUG_OUT_ARRAY_X8(Buffer,Size)
# define DEBUG_OUT_ARRAY_S16(Buffer,Size)
# define DEBUG_OUT_ARRAY_U16(Buffer,Size)
# define DEBUG_OUT_ARRAY_X16(Buffer,Size)
# define DEBUG_OUT_ARRAY_S32(Buffer,Size)
# define DEBUG_OUT_ARRAY_U32(Buffer,Size)
# define DEBUG_OUT_ARRAY_X32(Buffer,Size)
# define DEBUG_OUT_STRUCT(Struct)
# define DEBUG_OUT_STRING(String)
#endif /* !DEBUG || !DEBUG_PUTS || !DEBUG_NUMBER && !DEBUG_FUNCTION */

/*
The MIT License (MIT)
Copyright (c) 2017 Charles Gunyon
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmp.h"

#define BUFFER_SIZE 256
static char buffer[BUFFER_SIZE];
static char *current_pointer;
static int current_buffer_length;
static char string_buffer[256];
static char *str_pointer;
static void reset_buffer( void )
{
  current_buffer_length = 0;
  current_pointer = buffer;
}
static void set_to_read(void)
{
    current_pointer = buffer;
    
}
//typedef bool   (*cmp_reader)(struct cmp_ctx_s *ctx, void *data, size_t limit);
//typedef size_t (*cmp_writer)(struct cmp_ctx_s *ctx, const void *data,
//                                                    size_t count);
//typedef char*   (*cmp_skip)(struct cmp_ctx_s *ctx, void *data, size_t *limit);
static bool end_of_buffer(void)
{
   if( current_pointer >= buffer+current_buffer_length)
   {
       return true;
   }
   else
   {
       return false;
   }
    
}

//typedef char*   (*cmp_skip)(struct cmp_ctx_s *ctx, void *data, size_t limit);
static char *skip_bytes(struct cmp_ctx_s *ctx, void *data, size_t  count )
{
    char *return_value;
    return_value =  current_pointer;
    if (( current_pointer + count ) > buffer+current_buffer_length )
    {  
       printf("skip return false \n");
       return NULL;
    }
    else
    {
         printf("skip return true\n");
         
         current_pointer+= count;
         
    }  
    return return_value;
}      


static bool read_bytes(struct cmp_ctx_s *ctx, void *data, size_t  count ) 
{
    printf("read size %d\n",count);
    if (( current_pointer + count ) > buffer+current_buffer_length )
    {  printf("read return false \n");
       return false;
    }
    else
    {
         printf("read return true\n");
         memcpy(data, (char *)current_pointer, count);
         current_pointer+= count;
         
    }  
    return true;
}

//typedef size_t (*cmp_writer)(struct cmp_ctx_s *ctx, const void *data,
//                                                    size_t count);

static size_t write_bytes(struct cmp_ctx_s *ctx, const void *data, size_t  count ) 
{
    printf("write count %d \n",count);
    if (( current_pointer+ count ) >= (buffer+BUFFER_SIZE ) )
    {
       printf("write return false \n");
       return false;
    }
    else
    {
         printf("write return true \n");
         memcpy(current_pointer, (char *)data, count);
         current_pointer+= count;
         current_buffer_length = current_pointer-buffer;
    }  
    return true;
}

void error_and_exit(const char *msg) {
    fprintf(stderr, "%s\n\n", msg);
    exit(EXIT_FAILURE);
}

void app_main(void) 
{
    
    cmp_ctx_t cmp;
    int64_t sint = 0;
    uint64_t uint = 0;
    float flt = 0.0f;
    double dbl = 0.0;
    bool boolean = false;
    uint8_t fake_bool = 0;
    uint32_t string_size = 0;
    uint32_t array_size = 0;
    uint32_t binary_size = 0;
    uint32_t map_size = 0;
    

    reset_buffer(  );
    cmp_init(&cmp, buffer, read_bytes,skip_bytes, write_bytes );

    /*
     * When you write an array, you first specify the number of array
     * elements, then you write that many elements.
     */
    if (!cmp_write_array(&cmp, 9))
        error_and_exit(cmp_strerror(&cmp));

    if (!cmp_write_sint(&cmp, -14))
        error_and_exit(cmp_strerror(&cmp));

    if (!cmp_write_uint(&cmp, 38))
        error_and_exit(cmp_strerror(&cmp));

    if (!cmp_write_float(&cmp, 1.8f))
        error_and_exit(cmp_strerror(&cmp));

    if (!cmp_write_double(&cmp, 300.4))
        error_and_exit(cmp_strerror(&cmp));

    if (!cmp_write_nil(&cmp))
        error_and_exit(cmp_strerror(&cmp));

    if (!cmp_write_true(&cmp))
        error_and_exit(cmp_strerror(&cmp));

    if (!cmp_write_false(&cmp))
        error_and_exit(cmp_strerror(&cmp));

    if (!cmp_write_bool(&cmp, false))
        error_and_exit(cmp_strerror(&cmp));

    if (!cmp_write_u8_as_bool(&cmp, 1))
        error_and_exit(cmp_strerror(&cmp));

    /* Array full */

    /*
     * Maps work similar to arrays, but the length is in "pairs", so this
     * writes 2 pairs to the map.  Subsequently, pairs are written in key,
     * value order.
     */

    if (!cmp_write_map(&cmp, 2))
        error_and_exit(cmp_strerror(&cmp));

    /* You can write string data all at once... */

    if (!cmp_write_str(&cmp, "Greeting", 8))
        error_and_exit(cmp_strerror(&cmp));

    if (!cmp_write_str(&cmp, "Hello", 5))
        error_and_exit(cmp_strerror(&cmp));

    if (!cmp_write_str(&cmp, "Name", 4))
        error_and_exit(cmp_strerror(&cmp));

    /* ...or in chunks */

    if (!cmp_write_str(&cmp, "Linus", 5))
        error_and_exit(cmp_strerror(&cmp));


    /* Map full */

    

    if (!cmp_write_str(&cmp, "MessagePack", 11))
        error_and_exit(cmp_strerror(&cmp));
#if 0
    if (!cmp_write_bin_marker(&cmp, 8))
        error_and_exit(cmp_strerror(&cmp));

    if (file_writer(&cmp, "is ", 3) != 3)
        error_and_exit(strerror(errno));

    if (file_writer(&cmp, "great", 5) != 5)
        error_and_exit(strerror(errno));
#endif
    set_to_read(); 
    if (!cmp_read_array(&cmp, &array_size))
        error_and_exit(cmp_strerror(&cmp));

    if (array_size != 9)
        error_and_exit("Array size was not 9");

    if (!cmp_read_sinteger(&cmp, &sint))
        error_and_exit(cmp_strerror(&cmp));

    if (sint != -14)
        error_and_exit("Signed int was not -14");

    if (!cmp_read_uinteger(&cmp, &uint))
        error_and_exit(cmp_strerror(&cmp));

    if (uint != 38)
        error_and_exit("Unsigned int was not 38");

    if (!cmp_read_float(&cmp, &flt))
        error_and_exit(cmp_strerror(&cmp));

    if (flt != 1.8f)
        error_and_exit("Float was not 1.8f");

    if (!cmp_read_double(&cmp, &dbl))
        error_and_exit(cmp_strerror(&cmp));

    if (dbl != 300.4)
        error_and_exit("Double was not 300.f");

    if (!cmp_read_nil(&cmp))
        error_and_exit(cmp_strerror(&cmp));

    if (!cmp_read_bool(&cmp, &boolean))
        error_and_exit(cmp_strerror(&cmp));

    if (boolean != true)
        error_and_exit("First boolean was not true");

    if (!cmp_read_bool(&cmp, &boolean))
        error_and_exit(cmp_strerror(&cmp));

    if (boolean != false)
        error_and_exit("Second boolean was not false");

    if (!cmp_read_bool(&cmp, &boolean))
        error_and_exit(cmp_strerror(&cmp));

    if (boolean != false)
        error_and_exit("Third boolean was not false");

    if (!cmp_read_bool_as_u8(&cmp, &fake_bool))
        error_and_exit(cmp_strerror(&cmp));

    if (fake_bool != 1) {
        fprintf(stderr, "%u.\n", fake_bool);
        error_and_exit("Third boolean (u8) was not 1");
    }

    if (!cmp_read_map(&cmp, &map_size))
        error_and_exit(cmp_strerror(&cmp));

    if (map_size != 2)
        error_and_exit("Map size was not 2");

    /*
     * String reading here.  Note that normally strings are encoded using
     * UTF-8.  I have cleverly restricted this example to ASCII, which overlaps
     * UTF-8 encoding, but this must not be assumed in real-world code.
     *
     * You can read strings in two ways.  Either you can read the string's size
     * in bytes and then read the bytes manually...
     */
   string_size = sizeof(string_buffer);
   memset(string_buffer,0,sizeof(string_buffer));
    if (!cmp_read_str(&cmp, string_buffer,&string_size))
        error_and_exit(cmp_strerror(&cmp));

    if (string_size != 8)
        error_and_exit("Greeting string key size was not 8");



    if (strncmp(string_buffer, "Greeting", 8) != 0)
         
        error_and_exit("Greeting string key name was not 'Greeting'");

    /*
     * ...or you can set the maximum number of bytes to read and do it all in
     * one call.  cmp_read_str will write no more than "size" bytes, including
     * the terminating NULL, to the passed buffer.  If the string's size
     * exceeds the passed buffer size, the "size" input is set to the number of
     * bytes necessary, not including the terminating NULL.  Otherwise, the
     * "size" input is set to the number of bytes written, not including the
     * terminating NULL.
     */

    string_size = sizeof(string_buffer);
    if (!cmp_read_str(&cmp, string_buffer, &string_size))
        error_and_exit(cmp_strerror(&cmp));

    if (strncmp(string_buffer, "Hello", 5) != 0)
        error_and_exit("Greeting string value was not 'Hello'");

    string_size = sizeof(string_buffer);
    if (!cmp_read_str(&cmp, string_buffer, &string_size))
        error_and_exit(cmp_strerror(&cmp));

    if (strncmp(string_buffer, "Name", 4) != 0)
        error_and_exit("Name key name was not 'Name'");

    string_size = sizeof(string_buffer);
    if (!cmp_read_str_ptr(&cmp, &str_pointer, &string_size))
        error_and_exit(cmp_strerror(&cmp));

    if (strncmp(str_pointer, "Linus", 5) != 0)
        error_and_exit("Name key value was not 'Linus'");

    memset(string_buffer, 0, sizeof(string_buffer));
    binary_size = 0;
    if (cmp_read_str_ptr(&cmp, &str_pointer, &binary_size) == false)
        error_and_exit(cmp_strerror(&cmp));
    printf("binary size i %d \n",binary_size);
    if (memcmp(str_pointer, "MessagePack", 11) != 0)
        error_and_exit("1st binary value was not 'MessagePack'");


   set_to_read();
   printf("reading by objects -------------------------------- \n");
    /* Alternately, you can read objects until the stream is empty */
    while (1) {
        cmp_object_t obj;

        if (!cmp_read_object(&cmp, &obj)) {
            if (end_of_buffer())
                break;

            error_and_exit(cmp_strerror(&cmp));
        }

        switch (obj.type) {
            case CMP_TYPE_POSITIVE_FIXNUM:
            case CMP_TYPE_UINT8:
                printf("Unsigned Integer: %u\n", obj.as.u8);
                break;
            case CMP_TYPE_FIXMAP:
            case CMP_TYPE_MAP16:
            case CMP_TYPE_MAP32:
                printf("Map: %u\n", obj.as.map_size);
                break;
            case CMP_TYPE_FIXARRAY:
            case CMP_TYPE_ARRAY16:
            case CMP_TYPE_ARRAY32:
                printf("Array: %u\n", obj.as.array_size);
                break;
            case CMP_TYPE_FIXSTR:
            case CMP_TYPE_STR8:
            case CMP_TYPE_STR16:
            case CMP_TYPE_STR32:
                if (!read_bytes(&cmp,string_buffer, obj.as.str_size))
                    error_and_exit(strerror(errno));
                string_buffer[obj.as.str_size] = 0;
                printf("String: %s\n",string_buffer);
                break;
            case CMP_TYPE_BIN8:
            case CMP_TYPE_BIN16:
            case CMP_TYPE_BIN32:
                memset(string_buffer, 0, sizeof(string_buffer));
                if (!read_bytes(&cmp,string_buffer, obj.as.str_size))
                    error_and_exit(strerror(errno));
                printf("Binary: %s\n", string_buffer);
                break;
            case CMP_TYPE_NIL:
                printf("NULL\n");
                break;
            case CMP_TYPE_BOOLEAN:
                if (obj.as.boolean)
                    printf("Boolean: true\n");
                else
                    printf("Boolean: false\n");
                break;

            case CMP_TYPE_FLOAT:
                printf("Float: %f\n", obj.as.flt);
                break;
            case CMP_TYPE_DOUBLE:
                printf("Double: %f\n", obj.as.dbl);
                break;
            case CMP_TYPE_UINT16:
                printf("Unsigned Integer: %u\n", obj.as.u16);
                break;
            case CMP_TYPE_UINT32:
                printf("Unsigned Integer: %u\n", obj.as.u32);
                break;
            case CMP_TYPE_UINT64:
                printf("Unsigned Integer: %" PRIu64 "\n", obj.as.u64);
                break;
            case CMP_TYPE_NEGATIVE_FIXNUM:
            case CMP_TYPE_SINT8:
                printf("Signed Integer: %d\n", obj.as.s8);
                break;
            case CMP_TYPE_SINT16:
                printf("Signed Integer: %d\n", obj.as.s16);
                break;
            case CMP_TYPE_SINT32:
                printf("Signed Integer: %d\n", obj.as.s32);
                break;
            case CMP_TYPE_SINT64:
                printf("Signed Integer: %" PRId64 "\n", obj.as.s64);
                break;
            default:
                printf("Unrecognized object type %u\n", obj.type);
                break;
        }
    }

    

    
}

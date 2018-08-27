#ifndef _MSG_DICT_STREAM_H_
#define _MSG_DICT_STREAM_H_

typedef enum {INT_TYPE = 0, 
               UINT_TYPE = 1, 
               FLOAT_TYPE = 2, 
               STR_TYPE = 3, 
               BIN_TYPE = 4
               } MSG_TYPE;
 

typedef struct
{
  MSG_TYPE type;
  union {
  long integer;
  char *string;
  double real;
  void *binary;
  } data;
  int size; // needed by binary element
  char *field_name;
}MSG_PACK_ELEMENT;





static inline void msg_dict_pack_integer(MSG_PACK_ELEMENT *element,char *field_name, long data)
{
    element->field_name = field_name;
    element->type = INT_TYPE;
    element->data.integer = data;
    
}

static inline void msg_dict_pack_unsigned_integer(MSG_PACK_ELEMENT *element,char *field_name, long data)
{
    element->field_name = field_name;
    element->type = UINT_TYPE;
    element->data.integer = data;
    
}

static inline void msg_dict_pack_string(MSG_PACK_ELEMENT *element,char *field_name, char *data)
{
    element->field_name = field_name;
    element->type = STR_TYPE;
    element->data.string = data;
    element->size = strlen(data);    
    
}

static inline void msg_dict_pack_binary(MSG_PACK_ELEMENT *element, char *field_name,void *data, int size )
{
    element->field_name = field_name;
    element->type = BIN_TYPE;
    element->data.binary = data;
    element->size = size ;

    
}


static inline void msg_dict_pack_float(MSG_PACK_ELEMENT *element,char *field_name, double data)
{
    element->field_name = field_name;
    element->type = FLOAT_TYPE;
    element->data.real = data;
    
  

    
    
}


char *msg_dict_stream( int *buffer_size, int number, MSG_PACK_ELEMENT *msg_pack);


#endif

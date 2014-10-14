/* 
    udf_bitarray - aggregate ints into a bitarray
    Copyright (C) 2014  Timothy Caro-Bruce

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.
    
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.
    
    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
    
*/

#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(WIN32)
#define DLLEXP __declspec(dllexport) 
#else
#define DLLEXP
#endif

#ifdef STANDARD
#include <string.h>
#include <stdlib.h>
#include <time.h>
#else
#include <my_global.h>
#include <my_sys.h>
#endif
#include <mysql.h>
#include <m_ctype.h>
#include <m_string.h>
#include <stdlib.h>

#include <ctype.h>

#ifdef HAVE_DLOPEN

#define BITARRAY_DEFAULT_SIZE 1024*1024

/**
 * udf_bitarray
 */
#ifdef	__cplusplus
extern "C" {
#endif
my_bool bitarray_init(UDF_INIT* initid, UDF_ARGS* args, char* message);
void bitarray_deinit(UDF_INIT* initid);
void bitarray_clear(UDF_INIT* initid, char* is_null, char *error);
void bitarray_reset(UDF_INIT* initid, UDF_ARGS* args, char* is_null, 
        char *error);
void bitarray_add(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error);
char *bitarray(UDF_INIT *initid, UDF_ARGS *args, char *result, 
        unsigned long *length, char *is_null, char *error);
#ifdef	__cplusplus
}
#endif

typedef struct st_bitarray_container {
    long long size;
    char *bitarray;
} bitarray_container;

/**
 * int stat_accum_int(i)
 * 
 * return the i added to stat_accum_int(i-1)  
 */
my_bool bitarray_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    bitarray_container *container;
    long long size = BITARRAY_DEFAULT_SIZE;
    if(args->arg_count < 1 || args->arg_count > 2){
        strcpy(message, "Expect one or two arguments (udf: bitarray)");
        return 1;
    }
    if (args->arg_count == 2) 
    {
        if(args->arg_type[1] != INT_RESULT) {
            strcpy(message, "Second argument must be an int (udf: bitarray)");
            return 1;
        } else {
            size = *((long long*) args->args[1]) / 8 + 1;
        }
    } 
    container = malloc(sizeof(bitarray_container));
    if (! container) {
        container = NULL;
        strcpy(message, "Could not allocate memory (udf: bitarray)");
        return 1;
    }

    container->size = size;
    container->bitarray = malloc(size * sizeof(char));

    if (!container->bitarray) {
        container->bitarray = NULL;
        strcpy(message, "Could not allocate memory (udf: bitarray)");
        return 1;
    }
    initid->ptr = (char *) container;
    initid->maybe_null = 0;
    initid->max_length = size;
    args->arg_type[0] = INT_RESULT;
    return 0;
}

void bitarray_clear( UDF_INIT* initid, char* is_null, char *error ) {
    bitarray_container *container = (bitarray_container *)initid->ptr;
    for (int i=0; i < container->size; i++) {
        container->bitarray[i] = 0;
    }
}

void bitarray_reset(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error) {
    bitarray_clear(initid, is_null, error);
    bitarray_add(initid, args, is_null, error);
}

void bitarray_add(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error)
{
    bitarray_container *container = (bitarray_container *)initid->ptr;
    long long idx, offset, id;

    if(args->args[0] != NULL){
        id = *((long long*) args->args[0]);
        idx = id / 8;
        if (idx >= container->size) {
            *error = 1;
            return;
        }
        offset = id % 8;
        container->bitarray[idx] |= 1 << offset;
    } 
}

char *bitarray(UDF_INIT *initid, UDF_ARGS *args,
          char *result, unsigned long *length,
          char *is_null, char *error) {
    bitarray_container *container = (bitarray_container *)initid->ptr;
    *length = container->size;
    return container->bitarray;
}

void bitarray_deinit(UDF_INIT *initid){
    bitarray_container *container = (bitarray_container *)initid->ptr;
    if (container != NULL) {
        if (container->bitarray != NULL) {
            free(container->bitarray);
        }
        free(container);
    }
}


#endif /* HAVE_DLOPEN */


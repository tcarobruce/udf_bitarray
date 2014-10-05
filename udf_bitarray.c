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

#define BITARRAY_SIZE 1024*1024

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

/**
 * int stat_accum_int(i)
 * 
 * return the i added to stat_accum_int(i-1)  
 */
my_bool bitarray_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    my_bool status;
    if(args->arg_count != 1){
        strcpy(message, "Expect exactly one argument (udf: bitarray)");
        status = 1;
    } else if(!(initid->ptr = malloc(BITARRAY_SIZE * sizeof(char)))){
        initid->ptr = NULL;
        strcpy(message, "Could not allocate memory (udf: bitarray)");
        status = 1;
    } else {
        initid->maybe_null = 0;
        initid->max_length = BITARRAY_SIZE;
        args->arg_type[0] = INT_RESULT;
        status = 0;
    }
    return status;
}

void bitarray_clear( UDF_INIT* initid, char* is_null, char *error ) {
    for (int i=0; i < BITARRAY_SIZE; i++) {
        initid->ptr[i] = 0;
    }
}

void bitarray_reset(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error) {
    bitarray_clear(initid, is_null, error);
    bitarray_add(initid, args, is_null, error);
}

void bitarray_add(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char *error)
{
    long long idx, offset, id;
    char *bitarray;

    if(args->args[0] != NULL){
        id = *((long long*) args->args[0]);
        idx = id / 8;
        if (idx >= BITARRAY_SIZE) {
            *error = 1;
            return;
        }
        offset = id % 8;
        bitarray = (char *) initid->ptr;
        bitarray[idx] |= 1 << offset;
    } 
}

char *bitarray(UDF_INIT *initid, UDF_ARGS *args,
          char *result, unsigned long *length,
          char *is_null, char *error) {
    *length = BITARRAY_SIZE;
    return initid->ptr;
}

void bitarray_deinit(UDF_INIT *initid){
    if(initid->ptr != NULL){
        free(initid->ptr);
    }
}


#endif /* HAVE_DLOPEN */


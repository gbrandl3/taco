#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "API.h"
#include "private/ApiP.h"

/* 
 * timeouts for rpc calls
 */
#ifdef __cplusplus
extern "C" {
#endif
struct timeval zero_timeout = { 0 , 0 };        /* { sec , usec } */

/**
 * timeout for a client server connection  
 */
struct timeval timeout = { 3 , 0 };        /* { sec , usec } */
#ifndef lynx
struct timeval retry_timeout = { 3 , 0 };  /* { sec , usec } */
#else /* lynx */
struct timeval retry_timeout = { 1 , 0 };  /* { sec , usec } */
#endif

/**
 * timeout for a server server connection  
 */
struct timeval inner_timeout = { 2, 0 };        /* { sec , usec } */
#ifndef lynx
struct timeval inner_retry_timeout = { 2 , 0 };  /* { sec , usec } */
#else /* lynx */
struct timeval inner_retry_timeout = { 1 , 0 };  /* { sec , usec } */
#endif /* lynx */

/**
 * internal timeout for api library functions  
 */
struct timeval api_timeout = { 1 , 0 };        /* { sec , usec } */
#ifndef lynx
struct timeval api_retry_timeout = { 1 , 0 };  /* { sec , usec } */
#else /* lynx */
struct timeval api_retry_timeout = { 0 , 500000 };  /* { sec , usec } */
#endif /* lynx */

/**
 * timeout for a message server connection  
 */
struct timeval msg_timeout = { 1 , 0 };        /* { sec , usec } */
#ifndef lynx
struct timeval msg_retry_timeout = { 1 , 0 };  /* { sec , usec } */
#else /* lynx */
struct timeval msg_retry_timeout = { 0 , 500000 };  /* { sec , usec } */
#endif /* lynx */

/**  
 * timeout for database server connection  
 */
struct timeval dbase_timeout = { 4, 0 };        /* { sec , usec } */
struct timeval dbase_retry_timeout = { 4 , 0 };  /* { sec , usec } */

/**
 * timeout for asynchronous calls i.e. maximum time to wait for reply after 
 * sending request, after this time declare the request as failed due to TIMEOUT
 */
struct timeval asynch_timeout = { 25 , 0 };        /* { sec , usec } */

/**  
 * default timeout for import 
 */
struct timeval import_timeout = { 2 , 0 };        /* { sec , usec } */
struct timeval import_retry_timeout = { 2 , 0 };  /* { sec , usec } */

/**  
 * timeout for the select in rpc_check_host   
 */
struct timeval check_host_timeout = { 0 , 200000 }; /* { sec , usec } */

#ifdef __cplusplus
}
#endif

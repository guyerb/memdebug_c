#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include "dmalloc_common.h"
#include "dmalloc_wrappers.h"

void * dmalloc_calloc(size_t count, size_t size)
{
  void *ptr;

  dmalloc_printf("dmalloc_calloc\n");
  /* alloc bytes and hide our birthday inside */
  ptr = dmalloc_calloc_wrapper(count, size + dmalloc_extrabytes_sz());
  ptr = dmalloc_extrabytes_setandhide(ptr, time(NULL));

  dmalloc_stats_newalloc(ptr, dmalloc_usable_size(ptr));

  return ptr; 
}

void dmalloc_free(void *ptr)
{
  dmalloc_printf("dmalloc_free\n");

  dmalloc_stats_newfree(ptr, dmalloc_usable_size(ptr), dmalloc_extrabytes_get(ptr));
  dmalloc_free_wrapper(ptr);

  return;
}

void * dmalloc_malloc(size_t size)
{
  void *ptr;

  dmalloc_printf("dmalloc_malloc\n");
  
  /* alloc bytes and hide our birthday inside */
  ptr = dmalloc_malloc_wrapper(size + dmalloc_extrabytes_sz());
  ptr = dmalloc_extrabytes_setandhide(ptr, time(NULL));

  dmalloc_stats_newalloc(ptr, dmalloc_usable_size(ptr));

  return ptr;
}

/* see TODO in README for thoughts on a better implementation */
void * dmalloc_realloc(void *ptr, size_t size)
{
  void *p;

  dmalloc_printf("dmalloc_realloc\n");
  
  dmalloc_stats_newfree(ptr, dmalloc_usable_size(ptr), dmalloc_extrabytes_get(ptr));

  /* alloc bytes and hide birthday inside */
  p = dmalloc_realloc_wrapper(ptr, size + dmalloc_extrabytes_sz());
  p = dmalloc_extrabytes_setandhide(ptr, time(NULL));
  
  dmalloc_stats_newalloc(p, dmalloc_usable_size(p));
  
  return p;
}

#ifdef DARWIN
/* from dyld-interposing.h */
#define DYLD_INTERPOSE(_replacment,_replacee)				\
  __attribute__((used)) static struct{					\
    const void* replacment;						\
    const void* replacee;						\
  } _interpose_##_replacee __attribute__ ((section ("__DATA,__interpose"))) = { \
    (const void*)(unsigned long)&_replacment,				\
    (const void*)(unsigned long)&_replacee				\
  }; 


DYLD_INTERPOSE(dmalloc_calloc, calloc)
DYLD_INTERPOSE(dmalloc_free, free)
DYLD_INTERPOSE(dmalloc_malloc, malloc)
DYLD_INTERPOSE(dmalloc_realloc, realloc)

#endif	/* DARWIN */

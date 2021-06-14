//  mulle-container
//
//  Created by Nat! on 02/11/15.
//  Copyright (c) 2015 Nat! - Mulle kybernetiK.
//  Copyright (c) 2015 Codeon GmbH.
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//  Redistributions of source code must retain the above copyright notice, this
//  list of conditions and the following disclaimer.
//
//  Redistributions in binary form must reproduce the above copyright notice,
//  this list of conditions and the following disclaimer in the documentation
//  and/or other materials provided with the distribution.
//
//  Neither the name of Mulle kybernetiK nor the names of its contributors
//  may be used to endorse or promote products derived from this software
//  without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
#ifndef mulle_allocator_h__
#define mulle_allocator_h__

#include "include.h"

#include "mulle-allocator-struct.h"
#include <stddef.h>
#include <assert.h>


#if MULLE_C11_VERSION < ((3 << 20) | (0 << 8) | 0)
# error "mulle_c11 is too old"
#endif

//
// community version is always even
//
#define MULLE_ALLOCATOR_VERSION  ((4 << 20) | (2 << 8) | 3)


#ifndef MULLE_ALLOCATOR_EXTERN_GLOBAL
# define MULLE_ALLOCATOR_EXTERN_GLOBAL    MULLE_C_EXTERN_GLOBAL
#endif


MULLE_ALLOCATOR_EXTERN_GLOBAL struct mulle_allocator   mulle_default_allocator;
MULLE_ALLOCATOR_EXTERN_GLOBAL struct mulle_allocator   mulle_stdlib_allocator;
MULLE_ALLOCATOR_EXTERN_GLOBAL struct mulle_allocator   mulle_stdlib_nofree_allocator;

MULLE_C_NO_RETURN
void   mulle_allocation_fail( void *block, size_t size);

// NO_RETURN really but....
int   mulle_aba_abort( void *aba, void (*free)( void *), void *block);


# pragma mark - Petty Accessors
typedef   int (*mulle_allocator_aba_t)( void *, void (*f)( void *), void *);

static inline void   mulle_allocator_set_aba( struct mulle_allocator *p,
                                              void *aba,
                                              mulle_allocator_aba_t f)
{
   if( ! p)
      p = &mulle_default_allocator;

   p->aba     = aba;
   p->abafree = f ? f : mulle_aba_abort;
}


typedef void (*mulle_allocator_fail_t)( void *, size_t) _MULLE_C_NO_RETURN;

static inline void   mulle_allocator_set_fail( struct mulle_allocator *p,
                                               mulle_allocator_fail_t f)
{
   if( ! p)
      p = &mulle_default_allocator;

   p->fail = f ? f : mulle_allocation_fail;
}



# pragma mark - Vectoring


MULLE_C_NO_RETURN
static inline void   _mulle_allocator_fail( struct mulle_allocator *p, void *block, size_t size)
{
   (*p->fail)( block, size);
}


MULLE_C_NONNULL_RETURN
static inline void   *_mulle_allocator_malloc( struct mulle_allocator *p, size_t size)
{
   void   *q;

   assert( size);

   q = (*p->realloc)( NULL, size);
   if( ! q)
      (*p->fail)( NULL, size);
   return( q);
}


MULLE_C_NONNULL_RETURN
static inline void   *_mulle_allocator_calloc( struct mulle_allocator *p, size_t n, size_t size)
{
   void   *q;

   assert( n && size);

   q = (*p->calloc)( n, size);
   if( ! q)
      (*p->fail)( NULL, n * size);
   return( q);
}


//
// this reallocs, but doesn't free. If you pass in size 0, you risk failing.
// you can pass in block 0 for malloc
//
MULLE_C_NONNULL_RETURN
static inline void *
   _mulle_allocator_realloc( struct mulle_allocator *p, void *block, size_t size)
{
   void   *q;

   assert( size);

   q = (*p->realloc)( block, size);
   if( ! q)
      (*p->fail)( block, size);
   return( q);
}


//
// this function is more like the real realloc, but it is guaranteed that
// if you pass in block != 0 and size 0, that you free AND get NULL back
//
void   *_mulle_allocator_realloc_strict( struct mulle_allocator *p, void *block, size_t size);



static inline void   _mulle_allocator_free( struct mulle_allocator *p, void *block)
{
   if( block)
      (*p->free)( block);
}


static inline int   _mulle_allocator_abafree( struct mulle_allocator *p, void *block)
{
   if( ! block)
      return( 0);
   return( (*p->abafree)( p->aba, p->free, block));
}


static inline void   mulle_allocator_assert( struct mulle_allocator *p)
{
   assert( (p ? p : &mulle_default_allocator));
   assert( (p ? p : &mulle_default_allocator)->calloc);
   assert( (p ? p : &mulle_default_allocator)->realloc);
   assert( (p ? p : &mulle_default_allocator)->free);
   assert( (p ? p : &mulle_default_allocator)->fail);
}


# pragma mark -
# pragma mark API

MULLE_C_NONNULL_RETURN
static inline void   *mulle_allocator_malloc( struct mulle_allocator *p, size_t size)
{
   return( _mulle_allocator_malloc( p ? p : &mulle_default_allocator, size));
}


MULLE_C_NONNULL_RETURN
static inline void   *mulle_allocator_calloc( struct mulle_allocator *p, size_t n, size_t size)
{
   return( _mulle_allocator_calloc( p ? p : &mulle_default_allocator, n, size));
}


MULLE_C_NONNULL_RETURN
static inline void   *mulle_allocator_realloc( struct mulle_allocator *p, void *block, size_t size)
{
   return( _mulle_allocator_realloc( p ? p : &mulle_default_allocator, block, size));
}


static inline void   *mulle_allocator_realloc_strict( struct mulle_allocator *p, void *block, size_t size)
{
   return( _mulle_allocator_realloc_strict( p ? p : &mulle_default_allocator, block, size));
}



static inline void   mulle_allocator_free( struct mulle_allocator *p, void *block)
{
   _mulle_allocator_free( p ? p : &mulle_default_allocator, block);
}


static inline int   mulle_allocator_abafree( struct mulle_allocator *p, void *block)
{
   return( _mulle_allocator_abafree( p ? p : &mulle_default_allocator, block));
}


MULLE_C_NO_RETURN
static inline void   mulle_allocator_fail( struct mulle_allocator *p, void *block, size_t size)
{
   _mulle_allocator_fail( p ? p : &mulle_default_allocator, block, size);
}



# pragma mark -
# pragma mark Convenience API

MULLE_C_NONNULL_RETURN
static inline void   *mulle_malloc( size_t size)
{
   return( _mulle_allocator_malloc( &mulle_default_allocator, size));
}


MULLE_C_NONNULL_RETURN
static inline void   *mulle_calloc( size_t n, size_t size)
{
   return( _mulle_allocator_calloc( &mulle_default_allocator, n, size));
}


MULLE_C_NONNULL_RETURN
static inline void   *mulle_realloc( void *block, size_t size)
{
   return( _mulle_allocator_realloc( &mulle_default_allocator, block, size));
}


static inline void   *mulle_realloc_strict( void *block, size_t size)
{
   return( _mulle_allocator_realloc_strict( &mulle_default_allocator, block, size));
}


static inline void   mulle_free( void *block)
{
   _mulle_allocator_free( &mulle_default_allocator, block);
}


static inline int   mulle_abafree( void *block)
{
   return( _mulle_allocator_abafree( &mulle_default_allocator, block));
}


# pragma mark -
# pragma mark strdup convenience

MULLE_C_NONNULL_RETURN
char   *_mulle_allocator_strdup( struct mulle_allocator *p, char *s);


# pragma mark -
# pragma mark strdup API

static inline char   *mulle_allocator_strdup( struct mulle_allocator *p, char *s)
{
   if( ! s)
      return( s);
   return( _mulle_allocator_strdup( p ? p : &mulle_default_allocator, s));
}


static inline char   *mulle_strdup( char *s)
{
   return( mulle_allocator_strdup( &mulle_default_allocator, s));
}

#endif

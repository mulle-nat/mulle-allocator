#include <mulle-allocator/mulle-allocator.h>

#include <string.h>
#include <stdio.h>


static int  test( unsigned int amount)
{
   unsigned int  i;

   mulle_alloca_do( values, int, amount)
   {
      for( i = 0; i < amount; i++)
         values[ i] = i;

      amount *= 4;
      mulle_alloca_do_realloc( values, amount);
      for( ; i < amount; i++)
         values[ i] = i;
      amount /= 2;
      mulle_alloca_do_realloc( values, amount);
      for( i = 0; i < amount; i++)
         if( values[ i] != i)
            return( 1);
   }
   return( 0);
}


static double  test_return( unsigned int amount)
{
   mulle_alloca_do( values, int, amount)
   {
      // do something
      _mulle_alloca_do_return( values, (double) amount);
   }
   return( 0.0);
}


static int  *_test_extract( unsigned int amount)
{
   int  *p;
   int  *rval;

   rval = NULL;
   mulle_alloca_do( values, int, amount)
   {
      mulle_alloca_do_for( values, p)
      {
         *p++ = amount;
      }

      mulle_alloca_do_extract( values, rval);
   }
   return( rval);
}


static int  test_extract( unsigned int amount)
{
   int  *buf;
   int  *p;

   buf = _test_extract( amount);

   mulle_malloc_for( buf, amount, p)
   {
      if( *p++ != amount)
         return( -1);
   }

   mulle_free( buf);
   return( 0);
}


//  MEMO: leaks should be caught by valgrind
int  main( int argc, char *argv[])
{
   int      rc;
   int      i;
   double   v;

   rc = 0;
   for( i = 0; i < 256; i = i ? i * 2 : 1)
      rc |= test( i);

   v   = test_return( 1000);
   rc |= v != 1000.0;

   rc  |= test_extract( 8);
   rc  |= test_extract( 1848);

   return( rc);
}

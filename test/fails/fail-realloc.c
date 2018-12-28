#include <mulle-allocator/mulle-allocator.h>


static void   fail( void *unused, size_t ignored)
{
   printf( "OK\n");
   exit( 0);
}


int  main( int argc, char *argv[])
{
   mulle_default_allocator.fail = fail;
   mulle_realloc( NULL, -1);  // just leak
   return( -1);
}
#include <stdio.h>

/*
 check removing comment*/

int some_func ( int param1 , int param2 ) {
    if ( param1 < 0 ) // check remove a line comment
        return 0;
    else
        return param1 - param2;
}

int main ()
{
    printf( "Test obfsucated_file!\n" );
    printf( "Test function %d" , some_func ( 1 , 2 ));
    return 0;
}

// comment check
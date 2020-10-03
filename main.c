#include <stdio.h>

#include "obfuscater.h"

int main(int argc, char const *argv[])
{
    char* input_file = "test_ob.c";
    char* output_file = "test_ob_d.c";

    obfuscate_file(input_file, output_file);

    return 0;
}

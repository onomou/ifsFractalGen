// Initial Code by Rodrigo Hernandez (Kwizatz) for aedGUI (C) 2003
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int
main(int argc, char *argv[])
{
    unsigned char byte;
    unsigned long int filesize, bytecount = 0;

    if(argc != 2)
    {
        printf("Usage: %s [font]\n", argv[0]);
        return 1;
    }

    FILE *infile = fopen(argv[1], "rb");

    if(infile == NULL)
    {
        fprintf(stderr, "Couldn't open '%s': %s\n", argv[1], strerror(errno));
        return 1;
    }

    FILE *outfile = fopen("font.h", "wb");

    if(outfile == NULL)
    {
        fprintf(stderr, "Couldn't open 'font.h' for writing: %s\n",
                strerror(errno));
        return 1;
    }

    fseek(infile, 0, SEEK_END);
    filesize = ftell(infile);
    fseek(infile, 0, SEEK_SET);
    fprintf(outfile, "unsigned long int fontsize = %lu;\n", filesize);
    fprintf(outfile, "unsigned char fontdata[] = {\n");

    fread(&byte, 1, 1, infile);
    fprintf(outfile, "0x%x", byte);
    while(1)
    {
        if(fread(&byte, 1, 1, infile) == 0)
        {
            if(ferror(infile))
            {
                fprintf(stderr, "Error reading input file.\n");
                return 1;
            }
            else
                break;          // EOF
        }

        fputc(',', outfile);
        if(++bytecount == 10)
        {
            bytecount = 0;
            fputc('\n', outfile);
        }
        fprintf(outfile, "0x%x", byte);
    }
    fprintf(outfile, " };\n");
    fclose(infile);
    fclose(outfile);

    return 0;
};

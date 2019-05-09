#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "md5.h"

const int PASS_LEN=50;        // Maximum any password can be
const int HASH_LEN=33;        // Length of MD5 hash strings


// Stucture to hold both a plaintext password and a hash.
struct entry 
{
    char password[100];
    char hash[100];
};

int file_length(char *filename)
{
    struct stat info;
    int res = stat(filename, &info);
    if (res == -1) return -1;
    else return info.st_size;
}

int passcomp(const void *a, const void *b)
{
    struct entry *pa = (struct entry *)a;
    struct entry *pb = (struct entry *)b;
    return strcmp(pa->password, pb->password);
}

int hashcomp(const void *target, const void *elem)
{
    char *target_str = (char *)target;
    struct entry *pelem = (struct entry *)elem;
    return strcmp(target_str, (*pelem).password);
}

// TODO
// Read in the dictionary file and return an array of structs.
// Each entry should contain both the hash and the dictionary
// word.
struct entry *read_dictionary(char *filename, int *size)
{
    int filelength = file_length(filename);
    FILE *c = fopen(filename, "r");
    if (!c)
    {
        printf("Can't open %s for reading\n", filename);
        exit(1);
    }
    
    char *contents = malloc(filelength);
    fread(contents, 1, filelength, c);
    fclose(c);
    
    // Loop through contents, replace \n with \0
    int lines = 0;
    for (int i = 0; i < filelength; i++)
    {
        if (contents[i] == '\n') {
            contents[i] = '\0';
            lines++;
        }
    }
    printf("lines %d\n", lines);
    
    // Allocate array of structs
    struct entry *pass = malloc(lines * sizeof(struct entry));

    char pword[20];
    
    // Copy the first password into the pass array
    strcpy(pass[0].password, &contents[0]);
    pass[0].hash = md5(pass[0].password, strlen(pass[0].password));
    
    int count = 1;
    for (int i = 0; i < filelength; i++)
    {
        if (contents[i] == '\0')
        {
            char *nextpass = &contents[i] + 1;

            strcpy(pass[count].password, nextpass);
            pass[count].hash = md5(pass[count].password, strlen(pass[count].password));
            count++;
        }
    }
    
    // Sort them by password
    qsort(pass, lines, sizeof(struct entry), passcomp);
    
    *size = lines;
    return pass;
}


int main(int argc, char *argv[])
{
    if (argc < 3) 
    {
        printf("Usage: %s hash_file dict_file\n", argv[0]);
        exit(1);
    }
    int dlen;
    // TODO: Read the dictionary file into an array of entry structures
    struct entry *dict = read_dictionary(argv[1], &dlen);
    
    // TODO: Sort the hashed dictionary using qsort.
    // You will need to provide a comparison function.
    //qsort(dict, 0, 0, NULL);

    // TODO
    // Open the hash file for reading.

    // TODO
    // For each hash, search for it in the dictionary using
    // binary search.
    // If you find it, get the corresponding plaintext dictionary word.
    // Print out both the hash and word.
    // Need only one loop. (Yay!)
    char password[100];
    struct entry *found = bsearch(password, dict, dlen, sizeof(struct entry), hashcomp);
    if (found == NULL)
    {
        printf("Not found\n");
    }
    else
    {
        printf("Found %s %s\n", found->password, found->hash);
    }
}

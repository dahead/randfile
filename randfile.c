// randfile (c) 2024 by dahead (dahead@mailbox.org)
//
// About:
// This little application can create a index file
// by scanning a directory for files with a certain
// extension. 
//
// This is done by calling it: 
// $ ./randfile scan /path/to/scan <file-filter>
//
// After that randfile writes one random entry of
// that index file to STDOUT.
// $ ./randfile
// this adds quotation marks around the output
// $ ./randfile -p 
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

#define MAX_PATH 1024
#define MAX_FILTERS 10
#define BUFFER_SIZE 2048

#define DEFAULT_FILENAME "files.txt"
char *output_filename;

void init_output_filename() {
    char *env_filename = getenv("RANDFILE_FILENAME");
    if (env_filename != NULL) {
        output_filename = env_filename;
    } else {
        output_filename = DEFAULT_FILENAME;
    }
}

int matches_filter(const char *filename, char *filters[], int filter_count) {
    if (filter_count == 0) return 1; 
    for (int i = 0; i < filter_count; i++) {
        if (strstr(filename, filters[i]) != NULL) {
            return 1;
        }
    }
    return 0;
}

void list_files(const char *base_path, FILE *output_file, char *filters[], int filter_count) {
    struct dirent *entry;
    DIR *dir = opendir(base_path);

    if (dir == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char path[MAX_PATH];
            snprintf(path, sizeof(path), "%s/%s", base_path, entry->d_name);
            struct stat statbuf;
            if (stat(path, &statbuf) == 0) {
                if (S_ISDIR(statbuf.st_mode)) {
                    list_files(path, output_file, filters, filter_count);
                } else {
                    if (matches_filter(entry->d_name, filters, filter_count)) {
                        fprintf(output_file, "%s\n", path);
                    }
                }
            }
        }
    }
    closedir(dir);
}

int get_random_number(int min, int max) {
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        // use c default random
        srand(time(NULL));
        int random_value = rand() % max;
        if (random_value > 0)
            return random_value;

        perror("Failed to open /dev/urandom");
        return -1;
    }

    unsigned int random_value;
    if (read(fd, &random_value, sizeof(random_value)) != sizeof(random_value)) {
        perror("Failed to read from /dev/urandom");
        close(fd);
        return -1;
    }
    close(fd);

    return (random_value % (max - min + 1)) + min;
}

void print_random_file(int use_quotes) {
    FILE *file = fopen(output_filename, "r");
    if (file == NULL) {
        return; 
    }

    size_t count = 0;
    char buffer[BUFFER_SIZE];

    while (fgets(buffer, sizeof(buffer), file)) {
        count++;
    }
    rewind(file);

    if (count > 0) {
        // srand(time(NULL));
        // size_t random_index = rand() % count;
        int random_index = get_random_number(0, count);

        for (size_t i = 0; i <= random_index; i++) {
            fgets(buffer, sizeof(buffer), file);
        }
        
        // remove line break
        buffer[strcspn(buffer, "\n")] = '\0';

        // printf("%s", buffer);     
        if (use_quotes) {
            printf("\"%s\"\n", buffer);
        } else {
            printf("%s\n", buffer);
        }

        fflush(stdout);
    }

    fclose(file);
}

void process_directory(int argc, char *argv[]) {
    const char *directory = argv[1];
    char *filters[MAX_FILTERS];
    int filter_count = 0;

    if (argc > 2) {
        char *token = strtok(argv[2], ";");
        while (token != NULL && filter_count < MAX_FILTERS) {
            filters[filter_count++] = token;
            token = strtok(NULL, ";");
        }
    }

    FILE *output_file = fopen(output_filename, "w");
    if (output_file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    list_files(directory, output_file, filters, filter_count);
    fclose(output_file);
}

int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);
    int use_quotes = 0;
    init_output_filename();
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0) {
            use_quotes = 1;
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "Invalid option: %s\n", argv[i]);
            return EXIT_FAILURE;
        }
    }

    if (argc >= 2) {
        struct stat statbuf;
        if (stat(argv[1], &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
            process_directory(argc, argv);
            return EXIT_SUCCESS;
        }
    }

    print_random_file(use_quotes);
    return EXIT_SUCCESS;
}


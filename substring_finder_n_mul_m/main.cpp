#include <cstdlib>
#include <iostream>
#include <iterator>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

using namespace std;

const int32_t buffer_size = 3;

signed main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Illegal number of arguments" << endl;
        return 0;
    }
    int32_t str_size = strlen(argv[1]);
    char* str = argv[1];
    bool mask[str_size];
    for (size_t i = 0; i < str_size; i++) {
        mask[i] = 0;
    }
    int fd = open(argv[2], O_RDONLY | O_CLOEXEC);
    if (fd < 0)
    {
        perror("open failed");
        return EXIT_FAILURE;
    }
    for (;;)
    {
        char buffer[buffer_size];
        ssize_t bytes_read = read(fd, buffer, buffer_size);

        if (bytes_read < 0)
        {
            perror("read failed");
            close(fd);
            return EXIT_FAILURE;
        }

        if (bytes_read == 0) {
            break;
        }
        for (int32_t i = 0; i < bytes_read; i++) {
            char cur_ch = buffer[i];
            if (cur_ch == str[str_size - 1] && mask[str_size - 1]) {
                ssize_t bytes_written = write(STDOUT_FILENO, "true\n", 5);
                if (bytes_written < 0)
                {
                    perror("write failed");
                    close(fd);
                    return EXIT_FAILURE;
                }
                close(fd);
                return EXIT_SUCCESS;
            }
            for (int32_t j = str_size - 2; j >= 0; j--) {
                if (j == 0 || mask[j]) {
                    mask[j + 1] = (cur_ch == str[j]);
                } else {
                    mask[j + 1] = false;
                }
            }
        }
    }
    ssize_t bytes_written = write(STDOUT_FILENO, "false\n", 6);
    if (bytes_written < 0)
    {
        perror("write failed");
        close(fd);
        return EXIT_FAILURE;
    }
    close(fd);
    return EXIT_SUCCESS;
}

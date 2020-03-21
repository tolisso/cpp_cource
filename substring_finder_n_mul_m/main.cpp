#include <cstdlib>
#include <iostream>
#include <iterator>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

using namespace std;

const int32_t buffer_size = 3;

signed main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Illegal number of arguments" << endl;
        return 0;
    }
    int32_t str_size = (int32_t)strlen(argv[2]);
    char* str = argv[2];
    bool mask[str_size];
    for (size_t i = 0; i < str_size; i++) {
        mask[i] = false;
    }
    int fd = open(argv[1], O_RDONLY | O_CLOEXEC);
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
        for (size_t i = 0; i < bytes_read; i++) {
            char cur_ch = buffer[i];

            for (int32_t j = str_size - 1; j >= 0; j--) {
                if (j == 0 || mask[j - 1]) {
                    mask[j] = (cur_ch == str[j]);
                } else {
                    mask[j] = false;
                }
            }
            if (cur_ch == str[str_size - 1] && mask[str_size - 1]) {
                ssize_t bytes_written = write(STDOUT_FILENO, "TRUE\n", 5);
                if (bytes_written < 0)
                {
                    perror("write failed");
                    close(fd);
                    return EXIT_FAILURE;
                }
                close(fd);
                return EXIT_SUCCESS;
            }
        }
    }
    ssize_t bytes_written = write(STDOUT_FILENO, "FALSE\n", 6);
    if (bytes_written < 0)
    {
        perror("write failed");
        close(fd);
        return EXIT_FAILURE;
    }
    close(fd);
    return EXIT_SUCCESS;
}

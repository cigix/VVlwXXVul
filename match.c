#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <immintrin.h>
#include <pthread.h>

#ifndef MAX_PROCESS_COUNT
# define MAX_PROCESS_COUNT 8
#endif

int main(int argc, const char *argv[])
{
  int fd = STDIN_FILENO;
  if (argc > 1)
  {
    if (strcmp(argv[1], "-") != 0)
    {
      fd = open(argv[1], O_RDONLY);
      if (fd == -1)
      {
        perror(argv[0]);
        exit(1);
      }
    }
  }

  int32_t buffer[1024];
  int32_t *codes = NULL;
  size_t codes_size = 0; // size in bytes, always 0 % sizeof(*codes)
  ssize_t size;
  while ((size = read(fd, buffer, sizeof(buffer))) > 0)
  {
    if (size % sizeof(*codes)) // Not aligned
    {
      fprintf(stderr, "%s: Error reading input: misaligned end (%zd)", argv[0],
              size);
      exit(1);
    }
    size_t old_size = codes_size;
    codes_size += size;
    codes = realloc(codes, codes_size);
    if (codes == NULL)
    {
        perror(argv[0]);
        exit(1);
    }
    memcpy(((char*)codes) + old_size, buffer, size);
  }
  if (size < 0)
  {
    perror(argv[0]);
    exit(1);
  }

  int codes_len = codes_size / sizeof(*codes);
  fprintf(stderr, "Loaded %d Vlw codes\n", codes_len);

  // Check all codes have popcount == 5
  fprintf(stderr, "Sanity check... ");
  for (int i = 0; i < codes_len; ++i)
  {
    if (_mm_popcnt_u32(codes[i]) != 5)
    {
      fprintf(stderr, "KO: %d\n", i);
      exit(1);
    }
  }
  fprintf(stderr, "OK\n");

  int process_count = 0;
  pthread_mutex_t print_mutex;
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&print_mutex, &attr);

  struct timespec clock_start;
  clock_gettime(CLOCK_REALTIME, &clock_start);
  int_fast64_t count = 0;
#define PRINT_PROGRESS() \
  do { \
    struct timespec clock_end; \
    clock_gettime(CLOCK_REALTIME, &clock_end); \
    double seconds = (double)(clock_end.tv_sec - clock_start.tv_sec) \
                   + (double)(clock_end.tv_nsec - clock_start.tv_nsec) / 1e9; \
    double items_per_second = (double)count / seconds; \
    fprintf(stderr, "Processed %llu items in %.2fs (%.2e items/s)\r", \
            (unsigned long long)count, seconds, items_per_second); \
  } while (0)

  for (int id1 = 0; id1 < codes_len; ++id1)
  {
    PRINT_PROGRESS();
    if (MAX_PROCESS_COUNT <= process_count)
    {
      // Wait for one process before spawning one new
      int status;
      if (wait(&status) < 0)
      {
        perror(argv[0]);
        exit(1);
      }
      if (status != 0)
        // Child must print its own error message
        exit(1);
    }

    pid_t pid = fork();

    if (pid == 0)
    {
      // This is the child process
      int_fast32_t Vlwcode = codes[id1];
      for (int id2 = id1 + 1; id2 < codes_len; ++id2)
      {
        int_fast32_t Xlwcode = Vlwcode | codes[id2];
        if (_mm_popcnt_u32(Xlwcode) != 10)
        {
          continue;
        }

        for (int id3 = id2 + 1; id3 < codes_len; ++id3)
        {
          int_fast32_t XVlwcode = Xlwcode | codes[id3];
          if (_mm_popcnt_u32(XVlwcode) != 15)
          {
            continue;
          }

          for (int id4 = id3 + 1; id4 < codes_len; ++id4)
          {
            int_fast32_t XXlwcode = XVlwcode | codes[id4];
            if (_mm_popcnt_u32(XXlwcode) != 20)
            {
              continue;
            }

            for (int id5 = id4 + 1; id5 < codes_len; ++id5)
            {
              int_fast32_t XXVlwcode = XXlwcode | codes[id5];
              if (_mm_popcnt_u32(XXVlwcode) == 25)
              {
                pthread_mutex_lock(&print_mutex);
                fprintf(stderr, "%64c\r", ' ');
                printf("%#010x,%#010x,%#010x,%#010x,%#010x\n", codes[id1],
                       codes[id2], codes[id3], codes[id4], codes[id5]);
                fflush(NULL);
                pthread_mutex_unlock(&print_mutex);
              }
            }
          }
        }
      }
      exit(0);
    }

    if (pid < 0)
    {
      perror(argv[0]);
      exit(1);
    }

    // 0 < pid

    if (process_count < MAX_PROCESS_COUNT)
      process_count++;
    count += ((unsigned long long)codes_len - id1 - 1)
           * ((unsigned long long)codes_len - id1 - 2)
           * ((unsigned long long)codes_len - id1 - 3)
           * ((unsigned long long)codes_len - id1 - 4);
  }
  // while there are processes to wait on
  while (1)
  {
    PRINT_PROGRESS();
    int status;
    if (wait(&status) < 0)
    {
      if (errno == ECHILD)
        // wait() failed because there are no remaining children
        break;
      perror(argv[0]);
      exit(1);
    }
    if (status != 0)
      // Child must print its own error message
      exit(1);
  }
  PRINT_PROGRESS();
  fprintf(stderr, "\n");
}

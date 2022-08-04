#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <immintrin.h>

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

  clock_t clock_start = clock();
  int_fast64_t count = 0;

  for (int id1 = 0; id1 < codes_len; ++id1)
  {
    int_fast32_t Vlwcode = codes[id1];
    for (int id2 = id1; id2 < codes_len; ++id2)
    {
      int_fast32_t Xlwcode = Vlwcode | codes[id2];
      for (int id3 = id2; id3 < codes_len; ++id3)
      {
        int_fast32_t XVlwcode = Xlwcode | codes[id3];
        for (int id4 = id3; id4 < codes_len; ++id4)
        {
          int_fast32_t XXlwcode = XVlwcode | codes[id4];
          for (int id5 = id4; id5 < codes_len; ++id5)
          {
            int_fast32_t XXVlwcode = XXlwcode | codes[id5];
            if (_mm_popcnt_u32(XXVlwcode) == 25)
            {
              printf("%#010x,%#010x,%#010x,%#010x,%#010x\n", codes[id1],
                     codes[id2], codes[id3], codes[id4], codes[id5]);
            }
            ++count;
          }
        }
      }
      clock_t clock_end = clock();
      double seconds = ((double)(clock_end - clock_start)) / CLOCKS_PER_SEC;
      double items_per_second = (double)count / seconds;
      fprintf(stderr, "Processed %llu items in %fs (%f items/s)\n",
              (unsigned long long)count, seconds, items_per_second);
    }
  }
}

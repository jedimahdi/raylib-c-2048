#ifndef _ARRAY_H
#define _ARRAY_H

#include <assert.h>
#include <stdlib.h>

#define da_append(da, item)                                                    \
  do {                                                                         \
    if ((da)->count >= (da)->capacity) {                                       \
      (da)->capacity = (da)->capacity == 0 ? 16 : (da)->capacity * 2;          \
      (da)->items =                                                            \
          realloc((da)->items, (da)->capacity * sizeof(*(da)->items));         \
      assert((da)->items != NULL && "Buy more RAM lol");                       \
    }                                                                          \
    (da)->items[(da)->count++] = (item);                                       \
  } while (0)

#endif // _ARRAY_H

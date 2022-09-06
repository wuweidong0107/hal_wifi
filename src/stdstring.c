#include <ctype.h>
#include "stdstring.h"

char *string_init(const char *src)
{
    return src ? strdup(src) : NULL;
}

char *string_to_upper(char *s)
{
    char *cs = (char *)s;
    for (; *cs != '\0'; cs++) {
        *cs = toupper(*cs);
    }
    return s;
}

char *string_to_lower(char *s)
{
    char *cs = (char *)s;
    for (; *cs != '\0'; cs++) {
        *cs = tolower(*cs);
    }
    return s;
}

char *string_ucwords(char *s)
{
   char *cs = (char *)s;
   for ( ; *cs != '\0'; cs++)
   {
      if (*cs == ' ')
         *(cs+1) = toupper(*(cs+1));
   }

   s[0] = toupper(s[0]);
   return s;
}

char *string_trim_left(char *s)
{
    if (s && *s) {
        size_t len = strlen(s);
        char *current = s;

        while(*current && isspace(*current)) {
            ++current;
            --len;
        }

        if (s != current)
            memmove(s, current, len+1);
    }
    return s;
}

char *string_trim_right(char *s)
{
    if(s && *s) {
        size_t len = strlen(s);
        char *current = s + len -1;

        while(current != s && isspace(*current)) {
            --current;
            --len;
        }
        current[isspace(*current) ? 0:1] = '\0';
    }
    return s;
}

char *string_trim(char *s)
{
    string_trim_right(s);
    string_trim_left(s);
}
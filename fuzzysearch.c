#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(levenshtein_match);

int min3(int a, int b, int c)
{
    if (a < b && a < c)
        return a;
    if (b < c)
        return b;
    return c;
}

Datum levenshtein_match(PG_FUNCTION_ARGS)
{
    text *txt1 = PG_GETARG_TEXT_PP(0);
    text *txt2 = PG_GETARG_TEXT_PP(1);

    int len1 = VARSIZE_ANY_EXHDR(txt1);
    int len2 = VARSIZE_ANY_EXHDR(txt2);
    const char *str1 = VARDATA_ANY(txt1);
    const char *str2 = VARDATA_ANY(txt2);

    int i, j;
    int *v0 = palloc0((len2 + 1) * sizeof(int));
    int *v1 = palloc0((len2 + 1) * sizeof(int));

    for (j = 0; j <= len2; j++)
        v0[j] = j;

    for (i = 0; i < len1; i++)
    {
        v1[0] = i + 1;
        for (j = 0; j < len2; j++)
        {
            int cost = (str1[i] == str2[j]) ? 0 : 1;
            v1[j + 1] = min3(v1[j] + 1, v0[j + 1] + 1, v0[j] + cost);
        }
        int *tmp = v0;
        v0 = v1;
        v1 = tmp;
    }

    int result = v0[len2];
    pfree(v0);
    pfree(v1);

    PG_RETURN_INT32(result);
}

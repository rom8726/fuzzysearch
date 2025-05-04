#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "utils/varlena.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(levenshtein_match);
PG_FUNCTION_INFO_V1(jaro_winkler_match);

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
    const char *str1 = (const char *)(VARDATA_ANY(txt1));
    const char *str2 = (const char *)(VARDATA_ANY(txt2));

    int i, j;
    int *v0 = palloc0((len2 + 1) * sizeof(int));
    int *v1 = palloc0((len2 + 1) * sizeof(int));

    for (j = 0; j <= len2; j++)
        v0[j] = j;

    for (i = 0; i < len1; i++)
    {
        int *tmp = v0;

        v1[0] = i + 1;
        for (j = 0; j < len2; j++)
        {
            int cost = (str1[i] == str2[j]) ? 0 : 1;
            v1[j + 1] = min3(v1[j] + 1, v0[j + 1] + 1, v0[j] + cost);
        }
        
        v0 = v1;
        v1 = tmp;
    }

    int result = v0[len2];
    pfree(v0);
    pfree(v1);

    PG_RETURN_INT32(result);
}

Datum jaro_winkler_match(PG_FUNCTION_ARGS)
{
    text *txt1 = PG_GETARG_TEXT_PP(0);
    text *txt2 = PG_GETARG_TEXT_PP(1);

    int len1 = VARSIZE_ANY_EXHDR(txt1);
    int len2 = VARSIZE_ANY_EXHDR(txt2);
    const char *s1 = (const char *)(VARDATA_ANY(txt1));
    const char *s2 = (const char *)(VARDATA_ANY(txt2));

    if (len1 == 0 || len2 == 0) {
        PG_RETURN_FLOAT8(0.0);
    }

    int match_distance = (len1 > len2 ? len1 : len2) / 2 - 1;
    int *s1_matches = palloc0(len1 * sizeof(int));
    int *s2_matches = palloc0(len2 * sizeof(int));

    int matches = 0;
    for (int i = 0; i < len1; i++) {
        int start = Max(0, i - match_distance);
        int end = Min(i + match_distance + 1, len2);

        for (int j = start; j < end; j++) {
            if (s2_matches[j]) continue;
            if (s1[i] != s2[j]) continue;
            s1_matches[i] = 1;
            s2_matches[j] = 1;
            matches++;
            break;
        }
    }

    if (matches == 0) {
        pfree(s1_matches);
        pfree(s2_matches);
        PG_RETURN_FLOAT8(0.0);
    }

    int t = 0;
    int k = 0;
    for (int i = 0; i < len1; i++) {
        if (!s1_matches[i]) continue;
        while (!s2_matches[k]) k++;
        if (s1[i] != s2[k]) t++;
        k++;
    }

    double m = matches;
    double jaro = ((m / len1) + (m / len2) + ((m - t / 2.0) / m)) / 3.0;

    // Jaro-Winkler adjustment
    int prefix = 0;
    for (int i = 0; i < Min(4, Min(len1, len2)); i++) {
        if (s1[i] == s2[i])
            prefix++;
        else
            break;
    }

    double jw = jaro + (prefix * 0.1 * (1 - jaro));

    pfree(s1_matches);
    pfree(s2_matches);

    PG_RETURN_FLOAT8(jw);
}

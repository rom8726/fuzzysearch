CREATE FUNCTION levenshtein_match(text, text)
RETURNS integer
AS 'MODULE_PATHNAME', 'levenshtein_match'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION jaro_winkler_match(text, text)
    RETURNS float
AS 'MODULE_PATHNAME', 'jaro_winkler_match'
LANGUAGE C IMMUTABLE STRICT;

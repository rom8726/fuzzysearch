CREATE FUNCTION levenshtein_match(text, text)
RETURNS integer
AS 'MODULE_PATHNAME', 'levenshtein_match'
LANGUAGE C IMMUTABLE STRICT;

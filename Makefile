EXTENSION = fuzzysearch
MODULES = fuzzysearch
DATA = fuzzysearch--1.0.sql
PG_CONFIG = pg_config

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

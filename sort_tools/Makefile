
UNSORTED_FILES:= $(wildcard x/*)
SORTED_FILES:= $(patsubst x/%,y/%,$(UNSORTED_FILES))

$(SORTED_FILES): $(UNSORTED_FILES)
	cat $(patsubst y/%,x/%,$@) | sort > $@
sort: $(SORTED_FILES)

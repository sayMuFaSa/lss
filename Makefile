include config.mk
SRC_FILES:=main.c ls.c list.c print.c
SRC_DIR:=$(PROJ_DIR)/src
OBJ:=$(SRC_FILES:.c=.o)

all:$(DEFAULT_MODE)

debug: CFLAGS:=$(DEBUG_CFLAGS)
debug: LDFLAGS:=$(DEBUG_CFLAGS)
debug:$(APP)

release: CFLAGS:=$(RELEASE_CFLAGS)
release: LDFLAGS:=$(RELEASE_LDFLAGS)
release:$(APP)

install:$(DEFAULT_MODE)
	@mv -v $(APP) $(PREFIX)
uninstall:
	@rm -v $(PREFIX)/$(APP)

$(APP):$(OBJ)
	$(CC) -o $@ $(LDFLAGS) $^

%.o:$(SRC_DIR)/%.c
	$(CC) -o $@ $(CFLAGS) -c $<

clean:
	rm *.o $(APP)

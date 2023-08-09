BIN:=mqtt_subscriber
SRCS:=$(wildcard *.c)
OBJS:=$(SRCS:.c=.o)
CFLAGS:=-Wall
CPPFLAGS:=-I$(CURDIR)

LDLIBS:=-llink_core \
        -lmiddleware_implementation \
		-lplatform_port \
		-lutils_modules \
		-lblobmsg_json -lubox -lubus \
		-lusb-1.0 \
		-largp -luci -ltlt_uci -lmosquitto 

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -Wl,-rpath=$(RPATH) -o $@ $^ $(LDLIBS)

%.o:%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $^

clean:
	$(RM) $(OBJS) $(BIN)
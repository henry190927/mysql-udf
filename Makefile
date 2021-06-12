CC = g++
CFLAGS = -fPIC $(INCL)
LDFLAGS = -shared

INCL = $(shell mysql_config --include) $(shell gsl-config --cflags) -I/usr/local/mysql/include
LIBS = $(shell gsl-config --libs)
PLUGINS = /usr/local/mysql/lib/plugin

TARGET_LIB = stats.so

SRCS = $(shell echo *.cc)
OBJS = $(SRCS:.cc=.o)

all : $(TARGET_LIB)

$(TARGET_LIB): $(OBJS)
	$(CC) $(LDFLAGS) $(LIBS) -o $@ $^

%.o : %.cc
	@echo "Compiling: $<"
	$(CC) $(CFLAGS) -o $@ -c $<

install :
	@cp $(TARGET_LIB) $(shell mysql_config --plugindir) $(PLUGINS)

clean :
	@rm -rf $(TARGET_LIB) $(OBJS)

TARGETS = template_usage.exe cmpl_usage.exe  inline_usage.exe static_member.exe const_usage.exe vector_trans.exe vector_array.exe main.exe file_ftruncate.exe \
	ImplicitConop.exe socket_hostname.exe strlensizeof.exe array_as_pointer.exe conv_fromString.exe set_test.exe const_ref.exe fork_process.exe stream_test.exe \
	udtfile_test.exe
CC = gcc
CCFLAGS = -g
LIBFLAGS = -lpthread -lstdc++ -lc -ludt

all:	$(TARGETS)
	

template_usage.exe: template_usage_test.cpp template_usage.cpp
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)

cmpl_usage.exe: cmpl_usage_test.cpp cmpl_usage.cpp
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)

inline_usage.exe: inline_usage_test.cpp inline_usage.cpp
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)

static_member.exe: static_member_test.cpp static_member.cpp
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)

const_usage.exe: const_usage_test.cpp const_usage.cpp
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)

vector_trans.exe: vector_trans.cpp
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)

vector_array.exe: vector_array.cpp
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)

main.exe: main.cpp
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)

file_ftruncate.exe: file_ftruncate.cpp
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)

ImplicitConop.exe: ImplicitConop.cpp
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)

socket_hostname.exe: socket_hostname.cpp
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)


strlensizeof.exe: strlensizeof.cpp
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)


array_as_pointer.exe: array_as_pointer.cpp
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)


conv_fromString.exe: conv_fromString.cpp
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)

set_test.exe: set_test.cpp
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)


const_ref.exe: const_ref.cpp
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)

fork_process.exe: fork_process.cpp
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)

stream_test.exe: stream_test.cpp
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)


udtfile_test.exe: udtfile_test.cpp
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)


.PHONY:	clean

clean:
	rm *.exe
	rm *.o
	rm *.gch*
	rm *.bin*

FROM gcc:4.9

ARG TARGET
ARG LIBRARY_NAME=common

ENV NAME ARG0
ENV NAME ARG1
ENV NAME ARG2
ENV NAME ARG3

COPY . /tlpi
WORKDIR /tlpi

RUN mkdir exe

# build object files
RUN gcc -Ilib -c ./lib/error_functions.c && \
    gcc -Ilib -c ./lib/get_num.c && \
    gcc -Ilib -c ./lib/file_perms.c && \
    gcc -Ilib -c ./lib/curr_time.c && \
    gcc -Ilib -c ./lib/print_wait_status.c 

# # build static library
RUN ar -cr ${LIBRARY_NAME} error_functions.o get_num.o file_perms.o curr_time.o print_wait_status.o

RUN gcc ${TARGET} ${LIBRARY_NAME} -Ilib -o exe/out

CMD ["/bin/sh", "./run_program.sh"]

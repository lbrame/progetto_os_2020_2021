CFLAGS   = -Wall -std=gnu99
INCLUDES = -I .
OBJDIR   = obj

RECEIVER_SRCS = defines.c err_exit.c shared_memory.c semaphore.c pipe.c fifo.c receiver_manager.c 
RECEIVER_OBJS = $(addprefix $(OBJDIR)/, $(RECEIVER_SRCS:.c=.o))

R1_SRCS = defines.c err_exit.c R1.c
R1_OBJS = $(addprefix $(OBJDIR)/, $(R1_SRCS:.c=.o))

R2_SRCS = defines.c err_exit.c R2.c
R2_OBJS = $(addprefix $(OBJDIR)/, $(R2_SRCS:.c=.o))

R3_SRCS = defines.c err_exit.c R3.c
R3_OBJS = $(addprefix $(OBJDIR)/, $(R3_SRCS:.c=.o))

SENDER_SRCS = defines.c err_exit.c shared_memory.c semaphore.c pipe.c fifo.c sender_manager.c 
SENDER_OBJS = $(addprefix $(OBJDIR)/, $(SENDER_SRCS:.c=.o))

S1_SRCS = defines.c err_exit.c S1.c
S1_OBJS = $(addprefix $(OBJDIR)/, $(S1_SRCS:.c=.o))

S2_SRCS = defines.c err_exit.c S2.c
S2_OBJS = $(addprefix $(OBJDIR)/, $(S2_SRCS:.c=.o))

S3_SRCS = defines.c err_exit.c S3.c
S3_OBJS = $(addprefix $(OBJDIR)/, $(S3_SRCS:.c=.o))

HACKLER_SRCS = defines.c err_exit.c hackler.c
HACKLER_OBJS = $(addprefix $(OBJDIR)/, $(HACKLER_SRCS:.c=.o))

all: $(OBJDIR) receiver_manager sender_manager hackler S1 S2 S3 R1 R2 R3

receiver_manager: $(RECEIVER_OBJS)
	@echo "Making executable: "$@
	@$(CC) $^ -o $@  -lm

R1: $(R1_OBJS)
	@echo "Making executable: "$@
	@$(CC) $^ -o $@  -lm

R2: $(R2_OBJS)
	@echo "Making executable: "$@
	@$(CC) $^ -o $@  -lm

R3: $(R3_OBJS)
	@echo "Making executable: "$@
	@$(CC) $^ -o $@  -lm

sender_manager: $(SENDER_OBJS)
	@echo "Making executable: "$@
	@$(CC) $^ -o $@  -lm

S1: $(S1_OBJS)
	@echo "Making executable: "$@
	@$(CC) $^ -o $@  -lm

S2: $(S2_OBJS)
	@echo "Making executable: "$@
	@$(CC) $^ -o $@  -lm

S3: $(S3_OBJS)
	@echo "Making executable: "$@
	@$(CC) $^ -o $@  -lm

hackler: $(HACKLER_OBJS)
	@echo "Making executable: "$@
	@$(CC) $^ -o $@  -lm

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

clean:
	@rm -vf ${RECEIVER_OBJS}
	@rm -vf ${R1_OBJS}
	@rm -vf ${R2_OBJS}
	@rm -vf ${R3_OBJS}
	@rm -vf ${SENDER_OBJS}
	@rm -vf ${S1_OBJS}
	@rm -vf ${S2_OBJS}
	@rm -vf ${S3_OBJS}
	@rm -vf ${HACKLER_OBJS}
	@rm -vf receiver_manager
	@rm -vf R1
	@rm -vf R2
	@rm -vf R3
	@rm -vf sender_manager
	@rm -vf S1
	@rm -vf S2
	@rm -vf S3
	@rm -vf hackler
	@rm -rf ${OBJDIR}
	@ipcrm -a
	@echo "Removed object files and executables..."

.PHONY: run clean

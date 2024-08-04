#include <stdint.h>

// __asm__(".org 0173000"); // Set the origin to the boot rom

typedef short word;
typedef unsigned short uword;
typedef unsigned char byte;

struct longword
    {
    uword lo;
    word hi;
    };


#define COMPILE_BARRIER  __asm__ __volatile__("":::"memory")


struct unit_identifier
    {
    long serno_lo;
    word serno_hi;
    byte model;
    byte dev_class;
    };

struct command
    {
    // UQSSP header
    uword       msglen;
    unsigned    credits     : 4;
    unsigned    msgtype     : 4;
    byte        vcid;

    // MSCP header
    long        cmdref;
    word        unit;
    word                    : 16;
    byte        opcode;
    byte                    : 8;
    word        modifiers;

    union
        {
        // online command parameters
        struct
            {
            word                    : 16;
            word        unit_flags;
            long                    : 32;
            long                    : 32;
            long                    : 32;
            long        device_dependent;
            long                    : 32;
            };

        // read parameters
        struct
            {
            long        bytecount;
            struct
                {
                long    buffer_address;
                long                : 32;
                long                : 32;
                };
            long LBN;
            };
        };
    };


struct response
    {
    // UQSSP header
    uword       msglen;
    unsigned    credits     : 4;
    unsigned    msgtype     : 4;
    byte        vcid;

    // MSCP header
    long        cmdref;
    word        unit;
    word        sequence;
    byte        endcode;
    byte        flags;
    word        status;

    union
        {
        // online parameters
        struct
            {
            word        multiunit_code;
            word        unit_flags;

            struct
                {
                byte spndles;
                long                : 24;
                };

            struct unit_identifier id;
            long        media_type_identifier;
            long                    : 32;
            struct
                {
                uword lo;
                word hi;
                }unit_size;                      // size in LBNs
            long        volume_serial_number;           // optional, often zero
            };

        // read parameters
        struct
            {
            long                    : 32;
            long                    : 32;
            long                    : 32;

            long        first_bad_LBN;
            };
        };
    };


struct descriptor
    {
    word lo;
    word hi;
    };

struct FIFOs
    {
    struct descriptor rsp;
    struct descriptor cmd;
    };


typedef void FN();

#define ZERO (*(FN *)0)

#define IP *(word volatile *)0172150
#define SA *(word volatile *)0172152

#define BASE 040000
#define RSP_PACKET (BASE + 0004)
#define CMD_PACKET (BASE + 0104)
#define COMM_AREA  (BASE + 0204)



void error();
void do_step(word mask, word response);
void clear_packets();
void send_and_wait();

void boot()
    {
    struct response *rsp = (struct response *)RSP_PACKET;
    struct command *cmd = (struct command *)CMD_PACKET;
    int read_size;

    IP = 0;
    do_step(004000, 0100000);
    do_step(010000, COMM_AREA);
    do_step(020000, 0000000);
    do_step(040000, 0000001);

    clear_packets();
    cmd->msglen = 32;
    cmd->opcode = 011;
    send_and_wait();
    
    if(rsp->unit_size.hi != 0
    || rsp->unit_size.lo >64)
        {
        read_size = 64u * 512u;
        }
    else
        {
        read_size = rsp->unit_size.lo * 512;
        }

    clear_packets();
    cmd->msglen = 64;
    cmd->opcode = 041;
    cmd->bytecount = read_size;
    send_and_wait();

    ZERO();

    while(1);
    }



void do_step(word mask, word response)
    {
    word sa;

    do
        {
        sa = SA;
        if(sa<0)error();
        }
    while((sa&mask) == 0);

    SA = response;
    }


void clear_packets()
    {
    for(word *p = (word *)BASE; p<(word *)COMM_AREA;)
        {
        *p++ = 0;
        }
    }

void send_and_wait()
    {
    struct response *rsp = (struct response *)RSP_PACKET;
    struct command *cmd = (struct command *)CMD_PACKET;
    struct FIFOs *fifos = (struct FIFOs *)COMM_AREA;

    fifos->rsp.lo = (uintptr_t)&rsp->cmdref & 01777777;
    fifos->rsp.hi = 0100000;
    fifos->cmd.lo = (uintptr_t)&cmd->cmdref & 01777777;
    fifos->cmd.hi = 0100000;

    COMPILE_BARRIER;

    (void)IP;

    do
        {
        COMPILE_BARRIER;
        if(SA<0)error();
        }
    while(fifos->rsp.hi < 0);

    if(rsp->status != 0)error();
    }


void error()
    {
    while(1);
    }


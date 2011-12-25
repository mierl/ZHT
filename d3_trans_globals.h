#define PORT 50003

/*This macro has to be managed globally from this point. It marks whether
the transport is TCP or UDP or may be any other implementation. Please use the
values USE_TCP or USE_UDP to define the transport type*/
#define USE_TCP 0
#define USE_UDP 1
#define TRANS_PROTOCOL USE_TCP

/*The following pre processing will ease the usage complexity for the 
D3_transport layer. These parameters are used in defining the socket for
communication. If a fine tuning is required, the definitions in d3_transport.h
will have to be enhanced and changes will be required at the programs using 
d3 transport layer.*/
#if TRANS_PROTOCOL==USE_TCP
#define NMSPACE AF_INET
#define STYLE SOCK_STREAM
#define PROTOCOL 0
#elif TRANS_PROTOCOL==USE_UDP
#define NMSPACE AF_INET
#define STYLE SOCK_DGRAM
#define PROTOCOL 0
#endif


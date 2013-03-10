#ifndef _FFSNET_H_
#define _FFSNET_H_

int ffs_recvfile(const char *proto, const char *remote_ip, const char *server_port, const char *remote_filename, const char *local_filename);
int ffs_sendfile(const char *proto, const char *remote_ip, const char *server_port, const char *local_filename, const char *remote_filename);

static int _recvfile_udt(const char *remote_ip, const char *server_port, const char *remote_filename, const char *local_filename);
static int _sendfile_udt(const char *remote_ip, const char *server_port, const char *local_filename, const char *remote_filename);

#endif
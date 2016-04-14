/*
 * Syslog library for the arduino/energia api stuffs
 *
 * BSD licensed, largely borrowed from sys/syslog.h
 *
 */

#ifndef Syslog_h
#define Syslog_h

#include <Energia.h>
// EthernetUdp dosn't protect against multiple inclusion.
#ifndef UDP_TX_PACKET_MAX_SIZE
#include <EthernetUdp.h>
#endif

#include "lwip/udp.h"
#include <lwip/dns.h>

#define	LOG_EMERG	0	/* system is unusable */
#define	LOG_ALERT	1	/* action must be taken immediately */
#define	LOG_CRIT	2	/* critical conditions */
#define	LOG_ERR		3	/* error conditions */
#define	LOG_WARNING	4	/* warning conditions */
#define	LOG_NOTICE	5	/* normal but significant condition */
#define	LOG_INFO	6	/* informational */
#define	LOG_DEBUG	7	/* debug-level messages */

#define	LOG_PRIMASK	0x07	/* mask to extract priority part (internal) */
				/* extract priority */
#define	LOG_PRI(p)	((p) & LOG_PRIMASK)
#define	LOG_MAKEPRI(fac, pri)	((fac) | (pri))

/* facility codes */
#define	LOG_KERN	(0<<3)	/* kernel messages */
#define	LOG_USER	(1<<3)	/* random user-level messages */
#define	LOG_MAIL	(2<<3)	/* mail system */
#define	LOG_DAEMON	(3<<3)	/* system daemons */
#define	LOG_AUTH	(4<<3)	/* security/authorization messages */
#define	LOG_SYSLOG	(5<<3)	/* messages generated internally by syslogd */
#define	LOG_LPR		(6<<3)	/* line printer subsystem */
#define	LOG_NEWS	(7<<3)	/* network news subsystem */
#define	LOG_UUCP	(8<<3)	/* UUCP subsystem */
#define	LOG_CRON	(9<<3)	/* clock daemon */
#define	LOG_AUTHPRIV	(10<<3)	/* security/authorization messages (private) */
#define	LOG_FTP		(11<<3)	/* ftp daemon */

	/* other codes through 15 reserved for system use */
#define	LOG_LOCAL0	(16<<3)	/* reserved for local use */
#define	LOG_LOCAL1	(17<<3)	/* reserved for local use */
#define	LOG_LOCAL2	(18<<3)	/* reserved for local use */
#define	LOG_LOCAL3	(19<<3)	/* reserved for local use */
#define	LOG_LOCAL4	(20<<3)	/* reserved for local use */
#define	LOG_LOCAL5	(21<<3)	/* reserved for local use */
#define	LOG_LOCAL6	(22<<3)	/* reserved for local use */
#define	LOG_LOCAL7	(23<<3)	/* reserved for local use */

#define	LOG_NFACILITIES	24	/* current number of facilities */
#define	LOG_FACMASK	0x03f8	/* mask to extract facility part */
				/* facility of pri */
#define	LOG_FAC(p)	(((p) & LOG_FACMASK) >> 3)


class Syslog
{
  public:
    Syslog();
    void begin(const char *server, const char * ident, int fac);
    void syslog(int pri, const char *message);
    void setFacility(int fac);
    void offline();
  private:
    static void do_dns(const char *name, struct ip_addr *ipaddr, void *arg);
    const char * _server;
    const char * _ident;
    int _fac;
    #define SYSLOGBUFFERSIZE 128 + 64
    char packetBuffer[SYSLOGBUFFERSIZE];
    EthernetUDP Udp;
    boolean online;
};

#endif


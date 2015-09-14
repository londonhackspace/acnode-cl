#include <Energia.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>
#include <driverlib/sysctl.h>

#include "settings.h"
#include "user.h"
#include "version.h"
#include "microrl.h"

extern settings acsettings;

// microrl needs a print function
void mrlprint(const char * str) {
  Serial.print(str);
}

// definition commands word
#define _CMD_HELP   "help"
#define _CMD_SHOW   "show"
#define _CMD_MAC    "mac"
#define _CMD_SERVER "server"
#define _CMD_NODEID "nodeid"
#define _CMD_PORT   "port"
#define _CMD_VER    "version"
// sub commands for version command
#define _SCMD_MRL     "microrl"
#define _SCMD_ACNODE  "acnode"
#define _CMD_SAVE   "save"
#define _CMD_CLEAR  "clear"
#define _CMD_LIST   "list"
#define _CMD_REBOOT "reboot"
#define _CMD_NUKE   "nuke"
#define _CMD_SYSLOG "syslog"
#define _CMD_NAME   "name"
#define _CMD_RTIME  "resetruntime"
#define _CMD_ROLE   "role"
#define _CMD_ANN_PORT "announcer_port"

#define _NUM_OF_CMD 17
#define _NUM_OF_VER_SCMD 2

//available  commands
const char *keyworld [] = {
  _CMD_HELP, _CMD_SHOW, _CMD_MAC, _CMD_SERVER, _CMD_NODEID, _CMD_PORT, _CMD_VER, _CMD_SAVE, _CMD_CLEAR, _CMD_LIST, _CMD_REBOOT, _CMD_NUKE, _CMD_SYSLOG, _CMD_NAME, _CMD_RTIME, _CMD_ROLE, _CMD_ANN_PORT};
// version subcommands
const char * ver_keyworld [] = {
  _SCMD_MRL, _SCMD_ACNODE};

// array for comletion
char * compl_world [_NUM_OF_CMD + 1];

void print_help ()
{
  Serial.println("Use TAB key for completion\n\rCommand:");
  Serial.println ("\tversion {microrl | acnode} - print version of microrl lib or version of this acnode");
  Serial.println ("\thelp  - this message");
  Serial.println ("\tshow - show current settings");
  Serial.println ("\tmac <mac> - set mac address");
  Serial.println ("\tserver <hostname> - set acserver hostname");
  Serial.println ("\tnodeid <id> - set this node's id");
  Serial.println ("\tport <port> - set the port on the server to connect to");
  Serial.println ("\trole <role_id> - set the role of this acnode");
  Serial.println ("\tannouncer_port <port> - send announcements to this UDP port");
  Serial.println ("\tsave - save the current settings");
  Serial.println ("\tclear - clear the current settings");
  Serial.println ("\tlist - list the cached users");
  Serial.println ("\treboot - reboot the node");
  Serial.println ("\tnuke - clear the cached users");
  Serial.println ("\tsyslog <hostname> - set the syslog server");
  Serial.println ("\tname <toolname> - unique name of the tool");
  Serial.println ("\tresetruntime - reset the total time the tool has been running to zero");
}

bool ishex(char c) {
  if (c <= '9' && c >= '0') {
    return true;
  }
  if (c <= 'F' && c >= 'A') {
    return true;
  }
  if (c <= 'f' && c >= 'a') {
    return true;
  }
  return false;
}

uint8_t fromhex(char c) {
  if (c <= '9' && c >= '0') {
    return c - '0';
  }
  if (c <= 'F' && c >= 'A') {
    return (c - 'A') + 10;
  }
  if (c <= 'f' && c >= 'a') {
    return (c - 'a') + 10;
  }
  return 0;
}

int mrlexecute (int argc, const char * const * argv)
{
  int i = 0, j;
  // just iterate through argv word and compare it with your commands
  while (i < argc) {
    if (strcmp (argv[i], _CMD_HELP) == 0) {
      Serial.print ("microrl library based shell v 1.0\n\r");
      print_help ();        // print help
    } else if (strcmp (argv[i], _CMD_VER) == 0) {
      if (++i < argc) {
        if (strcmp (argv[i], _SCMD_ACNODE) == 0) {
          Serial.print ("ACNode v " ACVERSION "\n\r");
        } 
        else if (strcmp (argv[i], _SCMD_MRL) == 0) {
          Serial.print ("microrl v 1.2\n\r");
        } 
        else {
          Serial.print ((char*)argv[i]);
          Serial.print (" wrong argument, see help\n\r");
        }
      } 
      else {
        Serial.print ("version needs 1 paramater, see help\n\r");
      }
    } 
    else if (strcmp (argv[i], _CMD_SHOW) == 0) {
      dump_settings(acsettings);
    }
    else if (strcmp (argv[i], _CMD_MAC) == 0) {
      if ((++i) < argc) { // if value preset
        if (strlen (argv[i]) == (6 *2) + 5) {
          Serial.print("new mac: ");
          Serial.println(argv[i]);
          boolean ok;
          for (j = 0; j < 6 ; j++) {
            ok = false;
            if (ishex(argv[i][j * 3])) {
              ok = true;
            } else {
              ok = false; 
              break;
            }
            if (ishex(argv[i][(j * 3) + 1])) {
              ok = true;
            } else {
              ok = false; 
              break;
            }
            if (j < 5) {
              if (argv[i][(j * 3) + 2] == ':') {
                ok = true;
              } else {
                ok = false; 
                break;
              }
            }
          }
          if (!ok) {
            Serial.print("invalid mac address: ");
            Serial.println(argv[i]);
            break;
          }

          uint8_t out;

          for (j = 0; j < 6 ; j++) {
            out = fromhex(argv[i][j * 3]) << 4;
            out += fromhex(argv[i][(j * 3) + 1]);
            acsettings.mac[j] = out;
          }
        } else {
          Serial.println("mac address wrong length");
        }
      } 
      else {
        Serial.println("mac <macaddress>");
      }
    } 
    else if (strcmp (argv[i], _CMD_SERVER) == 0) {
      if ((++i) < argc) { // if value preset
        if (strlen (argv[i]) < SERVERNAMELEN) {
          Serial.print("new server: ");
          Serial.println(argv[i]);
          strncpy(acsettings.servername, argv[i], SERVERNAMELEN);
        } else {
          Serial.println("server name too long!");
        }
      } 
      else {
        Serial.println("server <hostname>");
      }
    } 
    else if (strcmp (argv[i], _CMD_NODEID) == 0) {
      if ((++i) < argc) { // if value preset
        // not likely to have more than 999 tools.
        if ( strlen(argv[i]) < 4) {
          Serial.print("new nodeid: ");
          Serial.println(argv[i]);
          errno = 0;
          int ret;
          char *end;
          boolean ok = true;
          ret = strtol(argv[i], &end, 10);

          if ((errno == ERANGE && (ret == LONG_MAX || ret == LONG_MIN))
                   || (errno != 0 && ret == 0)) {
            perror("strtol");
            ok = false;
          }

          if (end == argv[i]) {
            fprintf(stderr, "No digits were found\n");
            ok = false;
          }

          if (ok) {
            acsettings.nodeid = ret;
          } else {
            Serial.println("invalid nodeid");
          }
        } else {
          Serial.println("nodeid too big");
        }
      } 
    }
    else if (strcmp (argv[i], _CMD_PORT) == 0) {
      if ((++i) < argc) { // if value preset
        if ( strlen(argv[i]) < 6) {
          Serial.print("new port: ");
          Serial.println(argv[i]);
          errno = 0;
          int ret;
          char *end;
          boolean ok = true;
          ret = strtol(argv[i], &end, 10);

          if ((errno == ERANGE && (ret == LONG_MAX || ret == LONG_MIN))
                   || (errno != 0 && ret == 0)) {
            perror("strtol");
            ok = false;
          }

          if (end == argv[i]) {
            fprintf(stderr, "No digits were found\n");
            ok = false;
          }

          if (ret <= 0 || ret > 65535) {
            ok = false;
          }
          if (ok) {
            acsettings.port = ret;
          } else {
            Serial.println("invalid port");
          }
        } else {
          Serial.println("port too big");
        }
      } else {
        Serial.println("port <port>");
      }
    }
    else if (strcmp (argv[i], _CMD_SAVE) == 0) {
      Serial.print("Saveing Settings... ");
      int ret;
      ret = set_settings(acsettings);
      if (ret == 0) {
        Serial.println("OK!");
        acsettings.valid = 42;
      } 
      else {
        Serial.print("Error: ");
        Serial.println(ret);
      }
    }
    else if (strcmp (argv[i], _CMD_CLEAR) == 0) {
      Serial.print("Clearing Settings... ");
      int ret;
      ret = clear_settings();
      if (ret == 0) {
        Serial.println("OK!");
        acsettings.valid = -1;
      } else {
        Serial.print("Error: ");
        Serial.println(ret);
      }
    }
    else if (strcmp (argv[i], _CMD_LIST) == 0) {
      Serial.println("Listing cached users... ");
      list_users();
    }
    else if (strcmp (argv[i], _CMD_REBOOT) == 0) {
      Serial.print("Rebooting...!");
      SysCtlReset();
    }
    else if (strcmp (argv[i], _CMD_NUKE) == 0) {
      Serial.println("Nukeing cached users... ");
      nuke_users();
    }
    else if (strcmp (argv[i], _CMD_SYSLOG) == 0) {
      if ((++i) < argc) { // if value preset
        if (strlen (argv[i]) < SERVERNAMELEN) {
          Serial.print("new server: ");
          Serial.println(argv[i]);
          strncpy(acsettings.syslogserver, argv[i], SERVERNAMELEN);
        } else {
          Serial.println("syslog server name too long!");
        }
      }
      else {
        Serial.println("syslog <hostname>");
      }
    }
    else if (strcmp (argv[i], _CMD_NAME) == 0) {
      if ((++i) < argc) { // if value preset
        if (strlen (argv[i]) < sizeof(acsettings.toolname)) {
          Serial.print("toolname: ");
          Serial.println(argv[i]);
          strncpy(acsettings.toolname, argv[i], sizeof(acsettings.toolname));
        } else {
          Serial.println("toolname too long");
        }
      }
      else {
        Serial.println("name <toolname>");
      }
    }
    else if (strcmp (argv[i], _CMD_RTIME) == 0) {
      Serial.println("Resetting run time");
      acsettings.runtime = 0;
      set_settings(acsettings);
    }
    else if (strcmp (argv[i], _CMD_ROLE) == 0) {
      if ((++i) < argc) {
        int role = atoi(argv[i]);
        if (role > 1) {
          Serial.println("bad role");
        } else {
          Serial.print("new role: ");
          Serial.println(role);
          acsettings.role = role;
          set_settings(acsettings);  
        }
      } else {
        Serial.println("role <role_id>");
      }
    }
    else if (strcmp (argv[i], _CMD_ANN_PORT) == 0) {
      if ((++i) < argc) {
        int port = atoi(argv[i]);
        if (port > 0) {
          Serial.print("new announcer_port: ");
          Serial.println(port);
          acsettings.announcer_port = port;
          set_settings(acsettings);
        } else {
          Serial.println("bad port number");
        }
      } else {
        Serial.println("announcer_port <port>");
      }
    }
    else {
      Serial.print ("command: '");
      Serial.print ((char*)argv[i]);
      Serial.print ("' Not found.\n\r");
    }
    i++;
  }
  return 0;
}

// completion callback for microrl library
char ** mrlcomplete(int argc, const char * const * argv)
{
  int j = 0;

  compl_world [0] = NULL;

  // if there is token in cmdline
  if (argc == 1) {
    // get last entered token
    char * bit = (char*)argv [argc-1];
    // iterate through our available token and match it
    for (int i = 0; i < _NUM_OF_CMD; i++) {
      // if token is matched (text is part of our token starting from 0 char)
      if (strstr(keyworld [i], bit) == keyworld [i]) {
        // add it to completion set
        compl_world [j++] = (char *)keyworld[i];
      }
    }
  }
  else if ((argc > 1) && (strcmp (argv[0], _CMD_VER)==0)) { // if command needs subcommands
    // iterate through subcommand for command _CMD_VER array
    for (int i = 0; i < _NUM_OF_VER_SCMD; i++) {
      if (strstr (ver_keyworld [i], argv [argc-1]) == ver_keyworld [i]) {
        compl_world [j++] = (char *)ver_keyworld[i];
      }
    }
  } 
  else { // if there is no token in cmdline, just print all available token
    for (; j < _NUM_OF_CMD; j++) {
      compl_world[j] = (char *)keyworld[j];
    }
  }

  // note! last ptr in array always must be NULL!!!
  compl_world [j] = NULL;
  // return set of variants
  return compl_world;
}


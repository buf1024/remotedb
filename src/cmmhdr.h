/*
 * cmmhdr.h
 *
 *  Created on: 2012-12-11
 *      Author: buf1024@gmail.com
 */

#ifndef __48SLOTS_CMMHDR_H__
#define __48SLOTS_CMMHDR_H__

/*general
-------
list              #show all database
select [db]       #switch to database db, if args not provide, show the current db
whoami            #show the current user, if not auth, the user is root
stat   [db]       #show each db's information

ddl
-------
create [db]       #create a new db, if args missing, do nothing.
drop   [db]       #drop the db, if args missing, do nothing.
disable[db]       #disable the db, disable all operation.
enable [db]

dml
-------
get key
set key value
del key
scan [db]
rscan [db]

security
--------
auth [user] [pass]*/

#define CMD_GNR_LIST                           "list"
#define CMD_GNR_SELECT                         "select"
#define CMD_GNR_WHOAMI                         "whoami"
#define CMD_GNR_STAT                           "stat"

#define CMD_DDL_CREATE                         "create"
#define CMD_DDL_DROP                           "drop"
#define CMD_DDL_DISABLE                        "disable"
#define CMD_DDL_ENABLE                         "enable"

#define CMD_DML_GET                            "get"
#define CMD_DML_SET                            "set"
#define CMD_DML_DEL                            "del"
#define CMD_DML_SCAN                           "scan"
#define CMD_DML_RSCAN                          "rscan"

#define CMD_SCRTY_AUTH                         "auth"

////////////////////////////
#define CMM_PATH_SIZE                           256

#endif /* __48SLOTS_CMMHDR_H__ */

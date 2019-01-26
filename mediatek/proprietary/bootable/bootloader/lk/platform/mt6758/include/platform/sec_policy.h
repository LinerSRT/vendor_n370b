#ifndef _SEC_POLICY_H_
#define _SEC_POLICY_H_

#include "sec_policy_config_common.h"
#include "sec_policy_config1.h"
#include "sec_policy_config2.h"
#include "sec_policy_config3.h"
#include "sec_policy_config4.h"

#define MAX_POLICY_MAP_NUM 50

struct policy_part_map {
	unsigned int   sw_id;
	char          *part_name1;
	char          *part_name2;
	char          *part_name3;
	char          *part_name4;
	/*secure policy */
	unsigned char  sec_sbcdis_lock_policy;     /*SBC :Disable, LOCK_STATE: Lock   */
	unsigned char  sec_sbcdis_unlock_policy;   /*SBC :Disable, LOCK_STATE: Unlock */
	unsigned char  sec_sbcen_lock_policy;      /*SBC :Enable, LOCK_STATE: Lock    */
	unsigned char  sec_sbcen_unlock_policy;    /*SBC :ENable, LOCK_STATE: Unlock  */
	char          *hash;
};

/*Custom Secure Policy*/
#define  CUST1_SEC_POLICY_1  CREATE_POLICY_ENTRY(BIND_CUST1_POLICY_1, VERIFY_CUST1_POLICY_1, DL_CUST1_POLICY_1)
#define  CUST1_SEC_POLICY_2  CREATE_POLICY_ENTRY(BIND_CUST1_POLICY_2, VERIFY_CUST1_POLICY_2, DL_CUST1_POLICY_2)
#define  CUST1_SEC_POLICY_3  CREATE_POLICY_ENTRY(BIND_CUST1_POLICY_3, VERIFY_CUST1_POLICY_3, DL_CUST1_POLICY_3)
#define  CUST1_SEC_POLICY_4  CREATE_POLICY_ENTRY(BIND_CUST1_POLICY_4, VERIFY_CUST1_POLICY_4, DL_CUST1_POLICY_4)

#define  CUST2_SEC_POLICY_1  CREATE_POLICY_ENTRY(BIND_CUST2_POLICY_1, VERIFY_CUST2_POLICY_1, DL_CUST2_POLICY_1)
#define  CUST2_SEC_POLICY_2  CREATE_POLICY_ENTRY(BIND_CUST2_POLICY_2, VERIFY_CUST2_POLICY_2, DL_CUST2_POLICY_2)
#define  CUST2_SEC_POLICY_3  CREATE_POLICY_ENTRY(BIND_CUST2_POLICY_3, VERIFY_CUST2_POLICY_3, DL_CUST2_POLICY_3)
#define  CUST2_SEC_POLICY_4  CREATE_POLICY_ENTRY(BIND_CUST2_POLICY_4, VERIFY_CUST2_POLICY_4, DL_CUST2_POLICY_4)

extern unsigned int get_policy_entry_idx(char *part_name);
extern unsigned char *get_binding_hash(unsigned int policy_entry_idx);
extern unsigned int get_hash_binding_policy(unsigned int policy_entry_idx);
extern unsigned int get_vfy_policy(unsigned int policy_entry_idx);
extern unsigned int get_dl_policy(unsigned int policy_entry_idx);
extern unsigned int get_sw_id(unsigned int policy_entry_idx);
extern unsigned int is_all_partition_writable(unsigned int *write_all_perm);

#endif

/* This file was generated by mib2c and is intended for use as a mib module
  for the ucd-snmp snmpd agent. */


#ifndef _MIBGROUP_CLUSTERINFO_H
#define _MIBGROUP_CLUSTERINFO_H


/* we may use header_generic and header_simple_table from the util_funcs module */


config_require(util_funcs)


/* function prototypes */


void   init_ClusterInfo(void);
FindVarMethod var_ClusterInfo;




#endif /* _MIBGROUP_CLUSTERINFO_H */

/*
 * parental_genotype_service.h
 *
 *  Created on: 13 Jul 2018
 *      Author: billy
 */

#ifndef SERVICES_PARENTAL_GENOTYPE_SERVICE_INCLUDE_PARENTAL_GENOTYPE_SERVICE_H_
#define SERVICES_PARENTAL_GENOTYPE_SERVICE_INCLUDE_PARENTAL_GENOTYPE_SERVICE_H_


#include "cerealsdb_snp_service_library.h"
#include "service.h"
#include "schema_keys.h"


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Get the Service available for running the DFW Field Trial Service.
 *
 * @param user_p The UserDetails for the user trying to access the services.
 * This can be <code>NULL</code>.
 * @return The ServicesArray containing the DFW Field Trial Service. or
 * <code>NULL</code> upon error.
 *
 * @ingroup dfw_field_trial_service
 */
CEREALSDB_SNP_SERVICE_API ServicesArray *GetServices (UserDetails *user_p, GrassrootsServer *grassroots_p);


/**
 * Free the ServicesArray and its associated DFW Field Trial Service.
 *
 * @param services_p The ServicesArray to free.
 *
 * @ingroup dfw_field_trial_service
 */
CEREALSDB_SNP_SERVICE_API void ReleaseServices (ServicesArray *services_p);



CEREALSDB_SNP_SERVICE_LOCAL bool AddErrorMessage (ServiceJob *job_p, const json_t *value_p, const char *error_s, const int index);

#ifdef __cplusplus
}
#endif


#endif /* SERVICES_PARENTAL_GENOTYPE_SERVICE_INCLUDE_PARENTAL_GENOTYPE_SERVICE_H_ */

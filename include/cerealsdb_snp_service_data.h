/*
** Copyright 2014-2016 The Earlham Institute
** 
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** 
**     http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

/**
 * @file
 * @brief
 */
/*
 * parental_genotype_service_data.h
 *
 *  Created on: 18 Nov 2018
 *      Author: tyrrells
 */

#ifndef CEREALSDB_SNP_SERVICE_SERVICE_DATA_H_
#define CEREALSDB_SNP_SERVICE_SERVICE_DATA_H_

#include "cerealsdb_snp_service_library.h"
#include "jansson.h"

#include "service.h"
#include "mongodb_tool.h"



/**
 * The configuration data used by the Parental Genotype Service.
 *
 * @extends ServiceData
 */
typedef struct /*CEREALSDB_SNP_SERVICE_SERVICE_LOCAL*/ CerealsDBServiceData
{
	/** The base ServiceData. */
	ServiceData csd_base_data;


	/**
	 * @private
	 *
	 * The MongoTool to connect to the database where our data is stored.
	 */
	MongoTool *csd_mongo_p;


	/**
	 * @private
	 *
	 * The name of the database to use.
	 */
	const char *csd_database_s;


	/**
	 * @private
	 *
	 * The collection name of the population parentl-cross data use.
	 */
	const char *csd_collection_s;


} CerealsDBServiceData;



#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifdef ALLOCATE_CEREALSDB_SNP_SERVICE_TAGS
	#define CEREALSDB_SNP_SERVICE_PREFIX CEREALSDB_SNP_SERVICE_LOCAL
	#define CEREALSDB_SNP_SERVICE_VAL(x)	= x
	#define CEREALSDB_SNP_SERVICE_CONCAT_VAL(x,y) = x y
#else
	#define CEREALSDB_SNP_SERVICE_PREFIX extern
	#define CEREALSDB_SNP_SERVICE_VAL(x)
	#define CEREALSDB_SNP_SERVICE_CONCAT_VAL(x,y) = x y
#endif

#endif 		/* #ifndef DOXYGEN_SHOULD_SKIP_THIS */


#ifdef __cplusplus
extern "C"
{
#endif

CEREALSDB_SNP_SERVICE_LOCAL CerealsDBServiceData *AllocateCerealsDBServiceData (void);


CEREALSDB_SNP_SERVICE_LOCAL void FreeCerealsDBServiceData (CerealsDBServiceData *data_p);


CEREALSDB_SNP_SERVICE_LOCAL bool ConfigureCerealsDBService (CerealsDBServiceData *data_p, GrassrootsServer *grassroots_p);

#ifdef __cplusplus
}
#endif


#endif /* CEREALSDB_SNP_SERVICE_SERVICE_DATA_H_ */

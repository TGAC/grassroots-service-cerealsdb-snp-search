/*
 ** Copyright 2014-2018 The Earlham Institute
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
/*
 * search_service.c
 *
 *  Created on: 24 Oct 2018
 *      Author: billy
 */

#define ALLOCATE_CEREALSDB_SNP_SERVICE_TAGS (1)
#include "cerealsdb_snp_service.h"
#include "cerealsdb_snp_service_data.h"


#include "audit.h"
#include "streams.h"
#include "math_utils.h"
#include "string_utils.h"

/*
 * Static declarations
 */

static NamedParameterType S_MARKER = { "Marker", PT_KEYWORD };

static const char *GetCerealsDBSNPSearchServiceName (Service *service_p);

static const char *GetCerealsDBSNPSearchServiceDesciption (Service *service_p);

static const char *GetCerealsDBSNPSearchServiceInformationUri (Service *service_p);

static ParameterSet *GetCerealsDBSNPSearchServiceParameters (Service *service_p, Resource *resource_p, UserDetails *user_p);

static bool GetCerealsDBSNPSearchServiceParameterTypesForNamedParameters (struct Service *service_p, const char *param_name_s, ParameterType *pt_p);

static void ReleaseCerealsDBSNPSearchServiceParameters (Service *service_p, ParameterSet *params_p);

static ServiceJobSet *RunCerealsDBSNPSearchService (Service *service_p, ParameterSet *param_set_p, UserDetails *user_p, ProvidersStateTable *providers_p);

static ParameterSet *IsResourceForCerealsDBSNPSearchService (Service *service_p, Resource *resource_p, Handler *handler_p);

static bool CloseCerealsDBSNPSearchService (Service *service_p);

static ServiceMetadata *GetCerealsDBSNPSearchServiceMetadata (Service *service_p);

static void DoSearch (ServiceJob *job_p, const char *marker_s, CerealsDBServiceData *data_p);




/*
 * API definitions
 */


/*
 * API FUNCTIONS
 */

ServicesArray *GetServices (UserDetails *user_p, GrassrootsServer *grassroots_p)
{
	Service *service_p = (Service *) AllocMemory (sizeof (Service));

	if (service_p)
		{
			ServicesArray *services_p = AllocateServicesArray (1);

			if (services_p)
				{
					CerealsDBServiceData *data_p = AllocateCerealsDBServiceData ();

					if (data_p)
						{
							if (InitialiseService (service_p,
																		 GetCerealsDBSNPSearchServiceName,
																		 GetCerealsDBSNPSearchServiceDesciption,
																		 GetCerealsDBSNPSearchServiceInformationUri,
																		 RunCerealsDBSNPSearchService,
																		 IsResourceForCerealsDBSNPSearchService,
																		 GetCerealsDBSNPSearchServiceParameters,
																		 GetCerealsDBSNPSearchServiceParameterTypesForNamedParameters,
																		 ReleaseCerealsDBSNPSearchServiceParameters,
																		 CloseCerealsDBSNPSearchService,
																		 NULL,
																		 false,
																		 SY_SYNCHRONOUS,
																		 (ServiceData *) data_p,
																		 GetCerealsDBSNPSearchServiceMetadata,
																		 NULL,
																		 grassroots_p))
								{
									if (ConfigureCerealsDBService (data_p, grassroots_p))
										{
											* (services_p -> sa_services_pp) = service_p;

											return services_p;
										}
								}		/* if (InitialiseService (.... */
							else
								{
									FreeCerealsDBServiceData (data_p);
								}

						}		/* if (data_p) */

					FreeServicesArray (services_p);
				}

			if (service_p)
				{
					FreeService (service_p);
				}

		}		/* if (service_p) */

	return NULL;
}


void ReleaseServices (ServicesArray *services_p)
{
	FreeServicesArray (services_p);
}



Service *GetCerealsDBSNPSearchService (GrassrootsServer *grassroots_p)
{
	return NULL;
}



static const char *GetCerealsDBSNPSearchServiceName (Service * UNUSED_PARAM (service_p))
{
	return "CerealsDB SNP search service";
}


static const char *GetCerealsDBSNPSearchServiceDesciption (Service * UNUSED_PARAM (service_p))
{
	return "A service to search CerealsDB SNP data";
}


static const char *GetCerealsDBSNPSearchServiceInformationUri (Service * UNUSED_PARAM (service_p))
{
	return NULL;
}


static ParameterSet *GetCerealsDBSNPSearchServiceParameters (Service *service_p, Resource * UNUSED_PARAM (resource_p), UserDetails * UNUSED_PARAM (user_p))
{
	ParameterSet *param_set_p = AllocateParameterSet ("CerealsDB SNP search service parameters", "The parameters used for the CerealsDB SNP search service");

	if (param_set_p)
		{
			ServiceData *data_p = service_p -> se_data_p;
			Parameter *param_p = NULL;
			SharedType def;
			ParameterGroup *group_p = NULL;

			InitSharedType (&def);

			def.st_string_value_s = NULL;

			if ((param_p = EasyCreateAndAddParameterToParameterSet (data_p, param_set_p, group_p, S_MARKER.npt_type, S_MARKER.npt_name_s, "Marker", "The name of the marker to search for", def, PL_ALL)) != NULL)
				{
					return param_set_p;
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add %s parameter", S_MARKER.npt_name_s);
				}

			FreeParameterSet (param_set_p);
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate %s ParameterSet", GetCerealsDBSNPSearchServiceName (service_p));
		}

	return NULL;
}


static bool GetCerealsDBSNPSearchServiceParameterTypesForNamedParameters (struct Service *service_p, const char *param_name_s, ParameterType *pt_p)
{
	bool success_flag = true;

	if (strcmp (param_name_s, S_MARKER.npt_name_s) == 0)
		{
			*pt_p = S_MARKER.npt_type;
		}
	else
		{
			success_flag = false;
		}

	return success_flag;
}



static void ReleaseCerealsDBSNPSearchServiceParameters (Service * UNUSED_PARAM (service_p), ParameterSet *params_p)
{
	FreeParameterSet (params_p);
}


static bool CloseCerealsDBSNPSearchService (Service *service_p)
{
	bool success_flag = true;

	FreeCerealsDBServiceData ((CerealsDBServiceData *) (service_p -> se_data_p));;

	return success_flag;
}


static ServiceJobSet *RunCerealsDBSNPSearchService (Service *service_p, ParameterSet *param_set_p, UserDetails * UNUSED_PARAM (user_p), ProvidersStateTable * UNUSED_PARAM (providers_p))
{
	CerealsDBServiceData *data_p = (CerealsDBServiceData *) (service_p -> se_data_p);

	service_p -> se_jobs_p = AllocateSimpleServiceJobSet (service_p, NULL, "CerealsDB SNP search");

	if (service_p -> se_jobs_p)
		{
			ServiceJob *job_p = GetServiceJobFromServiceJobSet (service_p -> se_jobs_p, 0);

			LogParameterSet (param_set_p, job_p);

			SetServiceJobStatus (job_p, OS_FAILED_TO_START);

			if (param_set_p)
				{
					SharedType marker_value;
					InitSharedType (&marker_value);

					if (GetParameterValueFromParameterSet (param_set_p, S_MARKER.npt_name_s, &marker_value, true))
						{
							DoSearch (job_p, marker_value.st_string_value_s, data_p);
						}		/* if (GetParameterValueFromParameterSet (param_set_p, S_MARKER.npt_name_s, &marker_value, true)) */

				}		/* if (param_set_p) */

#if DFW_FIELD_TRIAL_SERVICE_DEBUG >= STM_LEVEL_FINE
			PrintJSONToLog (STM_LEVEL_FINE, __FILE__, __LINE__, job_p -> sj_metadata_p, "metadata 3: ");
#endif

			LogServiceJob (job_p);
		}		/* if (service_p -> se_jobs_p) */

	return service_p -> se_jobs_p;
}


static ServiceMetadata *GetCerealsDBSNPSearchServiceMetadata (Service * UNUSED_PARAM (service_p))
{
	const char *term_url_s = CONTEXT_PREFIX_EDAM_ONTOLOGY_S "topic_0625";
	SchemaTerm *category_p = AllocateSchemaTerm (term_url_s, "Genotype and phenotype",
																							 "The study of genetic constitution of a living entity, such as an individual, and organism, a cell and so on, "
																							 "typically with respect to a particular observable phenotypic traits, or resources concerning such traits, which "
																							 "might be an aspect of biochemistry, physiology, morphology, anatomy, development and so on.");

	if (category_p)
		{
			SchemaTerm *subcategory_p;

			term_url_s = CONTEXT_PREFIX_EDAM_ONTOLOGY_S "operation_0304";
			subcategory_p = AllocateSchemaTerm (term_url_s, "Query and retrieval", "Search or query a data resource and retrieve entries and / or annotation.");

			if (subcategory_p)
				{
					ServiceMetadata *metadata_p = AllocateServiceMetadata (category_p, subcategory_p);

					if (metadata_p)
						{
							SchemaTerm *input_p;

							term_url_s = CONTEXT_PREFIX_EDAM_ONTOLOGY_S "data_0968";
							input_p = AllocateSchemaTerm (term_url_s, "Keyword",
																						"Boolean operators (AND, OR and NOT) and wildcard characters may be allowed. Keyword(s) or phrase(s) used (typically) for text-searching purposes.");

							if (input_p)
								{
									if (AddSchemaTermToServiceMetadataInput (metadata_p, input_p))
										{
											SchemaTerm *output_p;


											/* Genotype */
											term_url_s = CONTEXT_PREFIX_EXPERIMENTAL_FACTOR_ONTOLOGY_S "EFO_0000513";
											output_p = AllocateSchemaTerm (term_url_s, "genotype", "Information, making the distinction between the actual physical material "
																										 "(e.g. a cell) and the information about the genetic content (genotype).");

											if (output_p)
												{
													if (AddSchemaTermToServiceMetadataOutput (metadata_p, output_p))
														{
															return metadata_p;
														}		/* if (AddSchemaTermToServiceMetadataOutput (metadata_p, output_p)) */
													else
														{
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add output term %s to service metadata", term_url_s);
															FreeSchemaTerm (output_p);
														}

												}		/* if (output_p) */
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate output term %s for service metadata", term_url_s);
												}

										}		/* if (AddSchemaTermToServiceMetadataInput (metadata_p, input_p)) */
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add input term %s to service metadata", term_url_s);
											FreeSchemaTerm (input_p);
										}

								}		/* if (input_p) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate input term %s for service metadata", term_url_s);
								}

							FreeServiceMetadata (metadata_p);
						}		/* if (metadata_p) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate service metadata");
						}

				}		/* if (subcategory_p) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate sub-category term %s for service metadata", term_url_s);
				}

		}		/* if (category_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate category term %s for service metadata", term_url_s);
		}

	return NULL;
}


static ParameterSet *IsResourceForCerealsDBSNPSearchService (Service * UNUSED_PARAM (service_p), Resource * UNUSED_PARAM (resource_p), Handler * UNUSED_PARAM (handler_p))
{
	return NULL;
}



static void DoSearch (ServiceJob *job_p, const char * const marker_s, CerealsDBServiceData *data_p)
{
	OperationStatus status = OS_FAILED_TO_START;

	if (!IsStringEmpty (marker_s))
		{
			bson_t *query_p = BCON_NEW ("name", BCON_UTF8 (marker_s));

			if (query_p)
				{
					json_t *results_p = GetAllMongoResultsAsJSON (data_p -> csd_mongo_p, query_p, NULL, NULL, 0);

					if (results_p)
						{
							if (json_is_array (results_p))
								{
									size_t i = 0;
									size_t num_added = 0;
									const size_t num_results = json_array_size (results_p);

									for (i = 0; i < num_results; ++ i)
										{
											json_t *entry_p = json_array_get (results_p, i);
											const char *name_s = GetJSONString (entry_p, "name");
											json_t *dest_record_p = NULL;

											json_object_del (entry_p, MONGO_ID_S);

											dest_record_p = GetResourceAsJSONByParts (PROTOCOL_INLINE_S, NULL, name_s, entry_p);

											if (dest_record_p)
												{
													if (AddResultToServiceJob (job_p, dest_record_p))
														{
															++ num_added;
														}
													else
														{
															json_decref (dest_record_p);
														}

												}		/* if (dest_record_p) */

										}		/* for (i = 0; i < num_results; ++ i) */


									if (num_added == num_results)
										{
											status = OS_SUCCEEDED;
										}
									else if (num_added > 0)
										{
											status = OS_PARTIALLY_SUCCEEDED;
										}
									else
										{
											status = OS_FAILED;
										}

								}		/* if (json_is_array (results_p)) */

							json_decref (results_p);
						}		/* if (results_p) */




					bson_destroy (query_p);
				}		/* if (query_p) */


		}		/* if (!IsStringEmpty (marker_s)) */


	SetServiceJobStatus (job_p, status);
}



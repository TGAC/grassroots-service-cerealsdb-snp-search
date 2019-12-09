/*
 * convert.c
 *
 *  Created on: 13 Nov 2019
 *      Author: billy
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "json_tools.h"
#include "json_util.h"
#include "typedefs.h"
#include "string_utils.h"
#include "math_utils.h"

typedef enum Mode
{
	MO_PROBE,
	MO_VARIETY,
	MO_NUM_MODES
} Mode;



static bool AddJSONFromRow (json_t *record_p, const char *probe_s, const char *variety_s, const int genotype, const char *chromosome_s, const Mode mode);

static int SaveRecord (char **previous_key_ss, char *new_key_s, json_t *record_json_p, FILE *out_f, const bool first_record_flag);


int main (int argc, char *argv [])
{
	int ret = 0;

	if (argc >= 2)
		{
			Mode mode = MO_PROBE;
			uint32 num_imported = 0;
			FILE *in_f = NULL;

			if (argc == 4)
				{
					if (strcmp (argv [3], "variety") == 0)
						{
							mode = MO_VARIETY;
						}
				}

			in_f = fopen (argv [1], "r");

			if (in_f)
				{
					FILE *out_f = fopen (argv [2], "w");

					if (out_f)
						{
							switch (mode)
								{
									case MO_PROBE:
										puts ("Ordering by Probe name");
										break;

									case MO_VARIETY:
										puts ("Ordering by Variety name");
										break;

									case MO_NUM_MODES:
										puts ("Unknown ordering mode");
										break;
								}


							if (fprintf (out_f, "[\n") > 0)
								{
									bool success_flag = true;
									char *previous_key_s = NULL;
									json_t *record_json_p = json_array ();
									bool first_record_flag = true;

									while ((!feof (in_f)) && (success_flag == true))
										{
											char *line_s = NULL;

											if (GetLineFromFile (in_f, &line_s))
												{
													if (!IsStringEmpty (line_s))
														{
															/*
															Peragis,AX-94381147,0,chr6A
															Kyalami,AX-94381147,0,chr6A
															AGT_Katana,AX-94381147,0,chr6A
															Barrel,AX-94381147,2,chr6A
															*/
															char *token_s = strtok (line_s, ",");

															if (token_s)
																{
																	char *variety_s = EasyCopyToNewString (token_s);

																	if (variety_s)
																		{
																			if ((token_s = strtok (NULL, ",")) != NULL)
																				{
																					char *probe_s = EasyCopyToNewString (token_s);

																					if (probe_s)
																						{
																							if ((token_s = strtok (NULL, ",")) != NULL)
																								{
																									int genotype = -1;
																									const char *temp_s = token_s;

																									if (GetValidInteger (&temp_s, &genotype))
																										{
																											if ((token_s = strtok (NULL, ",")) != NULL)
																												{
																													char *chromosome_s = EasyCopyToNewString (token_s);

																													if (chromosome_s)
																														{
																															char *key_s = probe_s;

																															if (mode == MO_VARIETY)
																																{
																																	key_s = variety_s;
																																}

																															if (previous_key_s)
																																{
																																	int res = SaveRecord (&previous_key_s, key_s, record_json_p, out_f, first_record_flag);

																																	if (res == 1)
																																		{
																																			if (first_record_flag)
																																				{
																																					first_record_flag = false;
																																				}
																																		}
																																	else if (res == -1)
																																		{
																																			char *dump_s = json_dumps (record_json_p, JSON_INDENT (2));

																																			if (dump_s)
																																				{
																																					printf ("Failed to save record for %s: %s\n", key_s, dump_s);
																																					free (dump_s);
																																				}
																																			else
																																				{
																																					printf ("Failed to save record for %s\n", key_s);
																																				}
																																		}
																																}
																															else
																																{
																																	previous_key_s = key_s;
																																}

																															if (AddJSONFromRow (record_json_p, probe_s, variety_s, genotype, chromosome_s, mode))
																																{
																																	++ num_imported;
																																}
																															else
																																{
																																	printf ("Failed to add \"%s\", \"%s\", %d, \"%s\"\n", probe_s, variety_s, genotype, chromosome_s);
																																	success_flag = false;
																																}

																															FreeCopiedString (chromosome_s);
																														}		/* if (chromosome_s) */
																													else
																														{
																															printf ("Failed to get chromosome from \"%s\"\n", token_s);
																															success_flag = false;
																														}
																												}
																											else
																												{
																													printf ("Failed to tokenize chromosome\n");
																													success_flag = false;
																												}

																										}		/* if (GetValidInteger (&genotype_s, &genotype)) */
																									else
																										{
																											printf ("Failed to get genotype from \"%s\"\n", token_s);
																											success_flag = false;
																										}

																								}
																							else
																								{
																									printf ("Failed to tokenize genotype\n");
																									success_flag = false;
																								}


																							if ((mode == MO_PROBE) && (previous_key_s != probe_s))
																								{
																									//printf ("freeing \"%s\", previous \%s\"\n", probe_s, previous_probe_s);
																									FreeCopiedString (probe_s);
																								}
																						}		/* if (probe_s) */
																					else
																						{
																							printf ("Failed to get probe from \"%s\"\n", token_s);
																							success_flag = false;
																						}

																				}
																			else
																				{
																					success_flag = false;
																				}

																			if ((mode == MO_VARIETY) && (previous_key_s != variety_s))
																				{
																					FreeCopiedString (variety_s);
																				}
																		}		/* if (variety_s) */
																	else
																		{
																			printf ("Failed to get variety from \"%s\"\n", token_s);
																			success_flag = false;
																		}

																}
															else
																{
																	success_flag = false;
																}
														}		/* if (!IsStringEmpty (line_s)) */


													FreeCopiedString (line_s);
												}
											else
												{
													printf ("line is empty\"n");
												}
										}		/* while (!feof (in_f)) */


									SaveRecord (&previous_key_s, "", record_json_p, out_f, first_record_flag);

									if (fprintf (out_f, "]\n") > 0)
										{

										}
								}

							fclose (out_f);
						}

					printf ("imported " UINT32_FMT " records\n", num_imported);

					fclose (in_f);
				}		/* if (in_f) */

		}		/* if (argc >= 2) */
	else
		{
			puts ("USAGE: converter <input file> <output file> \"variety\" (optional)\n");
		}

	return ret;
}


static int SaveRecord (char **previous_key_ss, char *new_key_s, json_t *record_json_p, FILE *out_f, const bool first_record_flag)
{
	int res = -1;

	if (strcmp (*previous_key_ss, new_key_s) != 0)
		{
			/* Save the latest probe */
			if (json_array_size (record_json_p) > 0)
				{
					const char *prefix_s = first_record_flag ? "" : ",\n";

					if (fprintf (out_f, "%s{ \"name\": \"%s\",\n \"data\": ", prefix_s, *previous_key_ss) >= 0)
						{
							if (json_dumpf (record_json_p, out_f, JSON_INDENT (2)) == 0)
								{
									if (fprintf (out_f, "\n}\n") >= 0)
										{
											res = 1;
										}
								}
						}

					json_array_clear (record_json_p);
				}

			*previous_key_ss = new_key_s;
		}
	else
		{
			res = 0;
		}

	return res;
}


static bool AddJSONFromRow (json_t *record_p, const char *probe_s, const char *variety_s, const int genotype, const char *chromosome_s, const Mode mode)
{
	json_t *data_p = json_object ();

	if (data_p)
		{
			if (SetJSONString (data_p, "chromosome", chromosome_s))
				{
					if (SetJSONInteger (data_p, "genotype", genotype))
						{
							bool success_flag = false;

							switch (mode)
								{
									case MO_PROBE:
										success_flag = SetJSONString (data_p, "variety", variety_s);
										break;

									case MO_VARIETY:
										success_flag = SetJSONString (data_p, "probe", probe_s);
										break;

									case MO_NUM_MODES:
										break;
								}

							if (success_flag)
								{
									if (json_array_append_new (record_p, data_p) == 0)
										{
											return true;
										}
								}
						}
				}

			json_decref (data_p);
		}

	return false;
}

#include "erl_nif.h"
#include "repository.h"
#include "reference.h"
#include "oid.h"
#include "object.h"
#include "commit.h"
#include "tree.h"
#include "blob.h"
#include "library.h"
#include "revwalk.h"
#include "index.h"
#include "geef.h"
#include <stdio.h>
#include <string.h>
#include <git2.h>

ErlNifResourceType *geef_repository_type;
ErlNifResourceType *geef_odb_type;
ErlNifResourceType *geef_ref_type;
ErlNifResourceType *geef_object_type;
ErlNifResourceType *geef_revwalk_type;
ErlNifResourceType *geef_index_type;

geef_atoms atoms;

static int load(ErlNifEnv *env, void **priv, ERL_NIF_TERM load_info)
{
	git_threads_init();

	geef_repository_type = enif_open_resource_type(env, NULL,
						     "repository_type", geef_repository_free, ERL_NIF_RT_CREATE, NULL);

	if (geef_repository_type == NULL)
		return -1;

	geef_odb_type = enif_open_resource_type(env, NULL,
					      "odb_type", geef_odb_free, ERL_NIF_RT_CREATE, NULL);

	if (geef_odb_type == NULL)
		return -1;

	geef_ref_type = enif_open_resource_type(env, NULL,
					      "ref_type", geef_ref_free, ERL_NIF_RT_CREATE, NULL);

	if (geef_ref_type == NULL)
		return -1;

	geef_object_type = enif_open_resource_type(env, NULL,
						     "object_type", geef_object_free, ERL_NIF_RT_CREATE, NULL);

	geef_revwalk_type = enif_open_resource_type(env, NULL,
		  "revwalk_type", geef_revwalk_free, ERL_NIF_RT_CREATE, NULL);

	geef_index_type = enif_open_resource_type(env, NULL,
		  "index_type", geef_index_free, ERL_NIF_RT_CREATE, NULL);

	if (geef_repository_type == NULL)
		return -1;

	atoms.ok = enif_make_atom(env, "ok");
	atoms.error = enif_make_atom(env, "error");
	atoms.true = enif_make_atom(env, "true");
	atoms.false = enif_make_atom(env, "false");
	atoms.repository = enif_make_atom(env, "repository");
	atoms.oid = enif_make_atom(env, "oid");
	atoms.symbolic = enif_make_atom(env, "symbolic");
	atoms.commit = enif_make_atom(env, "commit");
	atoms.tree = enif_make_atom(env, "tree");
	atoms.blob = enif_make_atom(env, "blob");
	atoms.tag = enif_make_atom(env, "tag");
	/* Revwalk */
	atoms.toposort    = enif_make_atom(env, "sort_topo");
	atoms.timesort    = enif_make_atom(env, "sort_time");
	atoms.reversesort = enif_make_atom(env, "sort_reverse");
	atoms.iterover    = enif_make_atom(env, "iterover");

	return 0;
}

int upgrade(ErlNifEnv* env, void** priv_data, void** old_priv_data, ERL_NIF_TERM load_info)
{
	return 0;
}

static void unload(ErlNifEnv* env, void* priv_data)
{
	git_threads_shutdown();
}

ERL_NIF_TERM
geef_error(ErlNifEnv *env)
{
	const git_error *error;
	ErlNifBinary bin;
	const char *message;
	size_t len;

	error = giterr_last();
	if (error && error->message)
		message = error->message;
	else
		message = "No message specified";

	len = strlen(message);
	if (!enif_alloc_binary(len, &bin))
		return atoms.error;

	memcpy(bin.data, message, len);

	return enif_make_tuple2(env, atoms.error, enif_make_binary(env, &bin));
}

static ErlNifFunc geef_funcs[] =
{
	{"repository_init", 2, geef_repository_init},
	{"repository_open", 1, geef_repository_open},
	{"repository_discover", 1, geef_repository_discover},
	{"repository_is_bare", 1, geef_repository_is_bare},
	{"repository_get_path", 1, geef_repository_path},
	{"repository_get_workdir", 1, geef_repository_workdir},
	{"repository_get_odb", 1, geef_repository_odb},
	{"odb_object_exists", 2, geef_odb_exists},
	{"odb_write", 3, geef_odb_write},
	{"reference_list", 1, geef_reference_list},
	{"reference_to_id", 2, geef_reference_to_id},
	{"reference_glob", 2, geef_reference_glob},
	{"reference_lookup", 2, geef_reference_lookup},
	{"reference_resolve", 1, geef_reference_resolve},
	{"reference_target", 1, geef_reference_target},
	{"reference_type", 1, geef_reference_type},
	{"reference_create", 5, geef_reference_create},
	{"oid_fmt", 1, geef_oid_fmt},
	{"oid_parse", 1, geef_oid_parse},
	{"object_lookup", 2, geef_object_lookup},
	{"object_id", 1, geef_object_id},
	{"commit_tree", 1, geef_commit_tree},
	{"commit_tree_id", 1, geef_commit_tree_id},
	{"tree_bypath", 2, geef_tree_bypath},
	{"blob_size", 1, geef_blob_size},
	{"blob_content", 1, geef_blob_content},
	{"library_version", 0, geef_library_version},
	{"revwalk_new",  1,    geef_revwalk_new},
	{"revwalk_push", 3,    geef_revwalk_push},
	{"revwalk_next", 1,    geef_revwalk_next},
	{"revwalk_sorting", 2, geef_revwalk_sorting},
	{"revwalk_reset", 1,   geef_revwalk_reset},
	{"index_new",   0, geef_revwalk_new},
	{"index_write", 1, geef_index_write},
	{"index_write_tree", 1, geef_index_write_tree},
	{"index_write_tree", 2, geef_index_write_tree},
	{"index_clear",      1, geef_index_clear},
	{"index_read_tree",  2, geef_index_read_tree},
};

ERL_NIF_INIT(geef_nif, geef_funcs, load, NULL, upgrade, unload)

#include <assert.h>
#include <getopt.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/utsname.h>

#include <libkmod.h>

#include "tt.h"

enum {
	COL_NAME,
	COL_PATH,

	MODTREE_NCOLUMNS
};

struct colinfo {
	const char *name;
	double whint;
	int flags;
	const char *help;
};

static struct colinfo infos[MODTREE_NCOLUMNS] = {
	[COL_NAME] = { "NAME", 0.30, TT_FL_TREE|TT_FL_NOEXTREMES,  "module name" },
	[COL_PATH] = { "PATH", 0.40, 0,                            "module path" },
};

static const struct option longopts[] = {
	{ "help",   0, 0, 'h' },
	{ "kernel", 1, 0, 'k' },
	{ "list",   0, 0, 'l' },
};

static int tt_flags;
static int columns[MODTREE_NCOLUMNS];
static int ncolumns;

static int get_column_id(int num)
{
	assert(num < ncolumns);
	assert(columns[num] < MODTREE_NCOLUMNS);
	return columns[num];
}

static struct colinfo *get_column_info(int num)
{
	return &infos[get_column_id(num)];
}

static int get_column_flags(int num)
{
	return get_column_info(num)->flags;
}

static const char *get_column_name(int num)
{
	return get_column_info(num)->name;
}

static float get_column_whint(int num)
{
	return get_column_info(num)->whint;
}

static bool is_module_filename(const char *name)
{
	struct stat st;
	const char *ptr;

	if (stat(name, &st) == 0 && S_ISREG(st.st_mode) &&
					(ptr = strstr(name, ".ko")) != NULL) {
		/*
		 * We screened for .ko; make sure this is either at the end of
		 * the name or followed by another '.' (e.g. gz or xz modules)
		 */
		if(ptr[3] == '\0' || ptr[3] == '.')
			return true;
	}

	return false;
}

static const char *get_data(struct kmod_module *mod, int num)
{
	switch (get_column_id(num)) {
	case COL_NAME:
		return strdup(kmod_module_get_name(mod));
	case COL_PATH:
		return strdup(kmod_module_get_path(mod));
	}

	return NULL;
}

static struct tt_line *add_line(struct tt *tt, struct kmod_module *mod,
		struct tt_line *parent)
{
	int i;
	struct tt_line *line = tt_add_line(tt, parent);

	if (line == NULL) {
		warn("failed to add line to output");
		return NULL;
	}

	for (i = 0; i < ncolumns; i++)
		tt_line_set_data(line, i, get_data(mod, i));

	return line;
}

static int modtree_do(struct kmod_ctx *kmod, struct tt *tt, struct kmod_module *mod,
		struct tt_line *parent_line)
{
	int rc;
	struct tt_line *line;
	struct kmod_list *l, *list = NULL;

	line = add_line(tt, mod, parent_line);
	if (line == NULL)
		return 1;

	rc = kmod_module_get_info(mod, &list);
	if (rc < 0) {
		warnx("get_info fail");
		return 1;
	}

	/* loop over info */
	kmod_list_foreach(l, list) {
		const char *k = kmod_module_info_get_key(l);
		const char *v = kmod_module_info_get_value(l);
		char *dep, *depstring;
		char *saveptr;

		if (strcmp(k, "depends") != 0)
			continue;

		/* loop over list,of,depends */
		depstring = strdup(v);
		for (dep = strtok_r(depstring, ",", &saveptr); dep; dep = strtok_r(NULL, ",", &saveptr)) {
			struct kmod_list *d, *deplist = NULL;
			int r = kmod_module_new_from_lookup(kmod, dep, &deplist);

			if (r < 0 || deplist == NULL) {
				warnx("failed to lookup %s\n", dep);
				continue;
			}

			/* for each depends, modtree_do it */
			kmod_list_foreach(d, deplist) {
				struct kmod_module *m = kmod_module_get_module(d);
				modtree_do(kmod, tt, m, line);
				kmod_module_unref(m);
			}
			kmod_module_unref_list(deplist);
		}
		free(depstring);
		break;
	}

	kmod_module_info_free_list(list);
	return rc;
}

static int modtree_alias_do(struct tt *tt, struct kmod_ctx *kmod,
		const char *alias)
{
	struct kmod_list *l, *filtered, *list = NULL;
	int rc = kmod_module_new_from_lookup(kmod, alias, &list);

	if (rc < 0) {
		warnx("Module alias %s not found.", alias);
		return rc;
	}

	if (list == NULL) {
		warnx("Module %s not found.", alias);
		return -ENOENT;
	}

	rc = kmod_module_apply_filter(kmod, KMOD_FILTER_BUILTIN, list, &filtered);
	kmod_module_unref_list(list);
	if (rc < 0) {
		warnx("failed to filter list: %m\n");
		return rc;
	}

	if (filtered == NULL) {
		warnx("Module %s not found.", alias);
		return -ENOENT;
	}

	kmod_list_foreach(l, filtered) {
		struct kmod_module *mod = kmod_module_get_module(l);
		int r = modtree_do(kmod, tt, mod, NULL);
		kmod_module_unref(mod);
		if (r < 0)
			rc = r;
	}

	kmod_module_unref_list(filtered);
	return rc;
}

static int modtree_path_do(struct tt *tt, struct kmod_ctx *kmod,
		const char *path)
{
	struct kmod_module *mod;
	int rc = kmod_module_new_from_path(kmod, path, &mod);

	if (rc < 0) {
		warnx("Module file %s not found", path);
		return rc;
	}

	rc = modtree_do(kmod, tt, mod, NULL);

	kmod_module_unref(mod);
	return rc;
}

static void __attribute__((__noreturn__)) usage(FILE *out)
{
	int i;

	fprintf(out,
			"Usage:\n"
			" %s [options] modules...\n\n"
			"Options:\n"
			" -h, --help             display this help text and exit\n"
			" -k, --kernel VERSION   use VERSION instead of $(uname -r)\n"
			" -l, --list             use list format output\n",
				program_invocation_short_name);

	fputs("\nAvailable columns:\n", out);

	for (i = 0; i < MODTREE_NCOLUMNS; i++)
		fprintf(out, " %11s  %s\n", infos[i].name, infos[i].help);

	exit(out == stderr ? EXIT_FAILURE : EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
	struct tt *tt = NULL;
	struct kmod_ctx *kmod;
	int i, rc = 0;
	char kdir_buf[PATH_MAX];

	const char *kver = NULL;

	assert(ARRAY_SIZE(columns) == MODTREE_NCOLUMNS);
	setlocale(LC_ALL, "");

	tt_flags |= TT_FL_TREE;

	for (;;) {
		int opt, idx;

		opt = getopt_long(argc, argv, "hk:l", longopts, &idx);
		if (opt < 0)
			break;

		switch (opt) {
		case 'h':
			usage(stdout);
		case 'k':
			kver = optarg;
			break;
		case 'l':
			tt_flags &= ~TT_FL_TREE;
			break;
		default:
			return EXIT_FAILURE;
		}
	}

	if (optind >= argc)
		errx(EXIT_FAILURE, "missing module name\n");

	argc -= optind;
	argv += optind;

	if (kver == NULL) {
		struct utsname u;

		if (uname(&u) < 0)
			err(EXIT_FAILURE, "failed to get current kernel name");
		kver = u.release;
	}
	snprintf(kdir_buf, sizeof(kdir_buf), "/lib/modules/%s", kver);

	kmod = kmod_new(kdir_buf, NULL);
	if (kmod == NULL) {
		fputs("error: kmod_new() failed!\n", stderr);
		return EXIT_FAILURE;
	}

	tt = tt_new_table(tt_flags);

	if (ncolumns == 0) {
		columns[ncolumns++] = COL_NAME;
		columns[ncolumns++] = COL_PATH;
	}

	for (i = 0; i < ncolumns; i++) {
		int fl = get_column_flags(i);

		if (!(tt_flags & TT_FL_TREE))
			fl &= ~TT_FL_TREE;

		if (!tt_define_column(tt, get_column_name(i),
					get_column_whint(i), fl)) {
			warn("failed to initialize output column");
			return EXIT_FAILURE;
		}
	}

	for (i = 0; i < argc; i++) {
		const char *name = argv[i];
		int r;

		if (is_module_filename(name))
			r = modtree_path_do(tt, kmod, name);
		else
			r = modtree_alias_do(tt, kmod, name);

		if (r < 0)
			rc = r;
	}

	tt_print_table(tt);

	kmod_unref(kmod);
	return rc >= 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

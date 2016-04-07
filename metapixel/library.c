/*
 * library.c
 *
 * metapixel
 *
 * Copyright (C) 1997-2009 Mark Probst
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#include "lispreader/lispreader.h"

#include "api.h"

static char*
tables_filename (const char *path)
{
    char *name = (char*)malloc(strlen(path) + 1 + strlen(TABLES_FILENAME) + 1);

    assert(name != 0);

    strcpy(name, path);
    strcat(name, "/");
    strcat(name, TABLES_FILENAME);

    return name;
}

static library_t*
make_library (const char *path)
{
    library_t *library = (library_t*)malloc(sizeof(library_t));

    assert(library != 0);

    library->path = strdup(path);
    assert(library->path != 0);

    library->metapixels = 0;
    library->num_metapixels = 0;

    return library;
}

static metapixel_t*
copy_metapixel_for_library (metapixel_t *metapixel, library_t *library, const char *filename)
{
    metapixel_t *copy = (metapixel_t*)malloc(sizeof(metapixel_t));

    assert(copy != 0);

    memcpy(copy, metapixel, sizeof(metapixel_t));

    copy->library = library;
    copy->name = strdup(metapixel->name);
    copy->filename = strdup(filename);
    assert(copy->filename != 0);
    copy->bitmap = 0;
    copy->next = 0;

    return copy;
}

static void
free_metapixels (metapixel_t *metapixel)
{
    while (metapixel != 0)
    {
	metapixel_t *next = metapixel->next;

	metapixel_free(metapixel);

	metapixel = next;
    }
}

static void
write_metapixel_metadata (metapixel_t *metapixel, FILE *out)
{
    int channel;

    /* FIXME: handle write errors */

    lisp_print_open_paren(out);
        lisp_print_symbol("small-image", out);
	lisp_print_string(metapixel->name, out);
	lisp_print_string(metapixel->filename, out);

	lisp_print_open_paren(out);
	    lisp_print_symbol("size", out);
	    lisp_print_integer(metapixel->width, out);
	    lisp_print_integer(metapixel->height, out);
	    lisp_print_real(metapixel->aspect_ratio, out);
	lisp_print_close_paren(out);

	lisp_print_open_paren(out);
	    lisp_print_symbol("flip", out);
	    lisp_print_boolean(metapixel->flip & FLIP_HOR, out);
	    lisp_print_boolean(metapixel->flip & FLIP_VER, out);
	lisp_print_close_paren(out);

	lisp_print_open_paren(out);
	    lisp_print_symbol("subpixel", out);
	    for (channel = 0; channel < NUM_CHANNELS; ++channel)
	    {
		static char *channel_names[] = { "r", "g", "b" };

		int i;

		lisp_print_open_paren(out);
		    lisp_print_symbol(channel_names[channel], out);
		    for (i = 0; i < NUM_SUBPIXELS; ++i)
			lisp_print_integer((int)metapixel->subpixels_rgb[i * NUM_CHANNELS + channel], out);
		lisp_print_close_paren(out);
	    }
	lisp_print_close_paren(out);

	lisp_print_open_paren(out);
	    lisp_print_symbol("anti", out);
	    lisp_print_integer(metapixel->anti_x, out);
	    lisp_print_integer(metapixel->anti_y, out);
	lisp_print_close_paren(out);
    lisp_print_close_paren(out);
    fputs("\n", out);
}

static int
read_tables (const char *library_dir, library_t *library)
{
    lisp_object_t *pattern;
    lisp_object_t *obj;
    lisp_stream_t stream;
    int num_subs;
    pools_t pools;
    allocator_t allocator;
    char *tables_name;
    int retval = 1;

    assert(library != 0);
    assert(library->metapixels == 0);

    tables_name = tables_filename(library_dir);
    if (lisp_stream_init_path(&stream, tables_name) == 0)
    {
	error_info_t info = error_make_string_info(tables_name);

	free(tables_name);

	error_report(ERROR_TABLES_FILE_CANNOT_OPEN, info);

	return 0;
    }
    free(tables_name);

    pattern = lisp_read_from_string("(small-image #?(string) #?(string)"
				    "  (size #?(integer) #?(integer) #?(real))"
				    "  (flip #?(boolean) #?(boolean))"
				    "  (subpixel (r . #?(list)) (g . #?(list)) (b . #?(list)))"
				    "  (anti #?(integer) #?(integer)))");
    assert(pattern != 0
	   && lisp_type(pattern) != LISP_TYPE_EOF
	   && lisp_type(pattern) != LISP_TYPE_PARSE_ERROR);
    assert(lisp_compile_pattern(&pattern, &num_subs));
    assert(num_subs == 12);

    init_pools(&pools);
    init_pools_allocator(&allocator, &pools);

    for (;;)
    {
        int type;

	reset_pools(&pools);
        obj = lisp_read_with_allocator(&allocator, &stream);
        type = lisp_type(obj);
        if (type != LISP_TYPE_EOF && type != LISP_TYPE_PARSE_ERROR)
        {
            lisp_object_t *vars[12];

            if (lisp_match_pattern(pattern, obj, vars, num_subs))
	    {
		metapixel_t *pixel;
		lisp_object_t *lst;
		int channel, i;

		pixel = metapixel_new(lisp_string(vars[0]), lisp_integer(vars[2]), lisp_integer(vars[3]),
				      lisp_real(vars[4]));
		assert(pixel != 0);

		pixel->filename = strdup(lisp_string(vars[1]));
		assert(pixel->filename != 0);

		pixel->anti_x = lisp_integer(vars[10]);
		pixel->anti_y = lisp_integer(vars[11]);

		if (lisp_boolean(vars[5]))
		    pixel->flip |= FLIP_HOR;
		if (lisp_boolean(vars[6]))
		    pixel->flip |= FLIP_VER;

		pixel->library = library;

		pixel->next = library->metapixels;
		library->metapixels = pixel;

		++library->num_metapixels;

		for (channel = 0; channel < NUM_CHANNELS; ++channel)
		{
		    lst = vars[7 + channel];

		    if (lisp_list_length(lst) != NUM_SUBPIXELS)
		    {
			error_info_t info = error_make_string_info(pixel->filename);

			lisp_stream_free_path(&stream);
			free_pools(&pools);

			error_report(ERROR_WRONG_NUM_SUBPIXELS, info);

			return 0;
		    }
		    else
			for (i = 0; i < NUM_SUBPIXELS; ++i)
			{
			    pixel->subpixels_rgb[i * NUM_CHANNELS + channel] = lisp_integer(lisp_car(lst));
			    lst = lisp_cdr(lst);
			}
		}

		metapixel_complete_subpixel(pixel);

		/*
		pixel->data = 0;
		pixel->collage_positions = 0;
		*/
	    }
	    else
	    {
		lisp_stream_free_path(&stream);
		free_pools(&pools);

		error_report(ERROR_TABLES_SYNTAX_ERROR, error_make_string_info(library_dir));

		return 0;
	    }
        }
        else if (type == LISP_TYPE_PARSE_ERROR)
	{
	    lisp_stream_free_path(&stream);
	    free_pools(&pools);

	    error_report(ERROR_TABLES_PARSE_ERROR, error_make_string_info(library_dir));

	    return 0;
	}

        if (type == LISP_TYPE_EOF)
            break;
    }

    lisp_stream_free_path(&stream);
    free_pools(&pools);

    return retval;
}

library_t*
library_new (const char *path)
{
    char *filename = tables_filename(path);
    int fd;

    assert(filename != 0);

    if (access(filename, F_OK) == 0)
    {
	error_info_t info = error_make_string_info(filename);

	free(filename);

	error_report(ERROR_TABLES_FILE_EXISTS, info);

	return 0;
    }

    fd = open(filename, O_RDWR | O_CREAT, 0666);

    if (fd == -1)
    {
	error_info_t info = error_make_string_info(filename);

	free(filename);

	error_report(ERROR_TABLES_FILE_CANNOT_CREATE, info);

	return 0;
    }

    free(filename);

    return make_library(path);
}

library_t*
library_open_without_reading (const char *path)
{
    char *filename = tables_filename(path);
    int result;

    result = access(filename, R_OK | W_OK);

    if (result == 0)
    {
	free(filename);

	return make_library(path);
    }
    else
    {
	error_info_t info = error_make_string_info(filename);

	free(filename);

	error_report(ERROR_TABLES_FILE_CANNOT_OPEN, info);

	return 0;
    }
}

library_t*
library_open (const char *path)
{
    library_t *library = make_library(path);

    assert(library != 0);

    if (!read_tables(path, library))
    {
	free_metapixels(library->metapixels);
	free(library);

	return 0;
    }

    return library;
}

void
library_close (library_t *library)
{
    free_metapixels(library->metapixels);
    free(library->path);
    free(library);
}

metapixel_t*
library_add_metapixel (library_t *library, metapixel_t *metapixel)
{
    char bitmap_filename[strlen(library->path) + 1 + strlen(metapixel->name) + 1 + 6 + 1];
    char tables_filename[strlen(library->path) + 1 + strlen(TABLES_FILENAME) + 1];
    bitmap_t *bitmap;
    FILE *file;

    /* get a filename for the bitmap */
    sprintf(bitmap_filename, "%s/%s", library->path, metapixel->name);
    if (access(bitmap_filename, F_OK) == 0)
    {
	int i;

	for (i = 0; i < 1000000; ++i)
	{
	    sprintf(bitmap_filename, "%s/%s.%06d", library->path, metapixel->name, i);
	    if (access(bitmap_filename, F_OK) == -1)
		break;
	}
    }

    if (access(bitmap_filename, F_OK) == 0)
    {
	error_report(ERROR_CANNOT_FIND_METAPIXEL_IMAGE_NAME, error_make_string_info(bitmap_filename));

	return 0;
    }

    /* write the bitmap */
    if (metapixel->bitmap == 0)
	bitmap = metapixel_get_bitmap(metapixel);
    else
	bitmap = metapixel->bitmap;

    /* FIXME: check for errors */
    bitmap_write(bitmap, bitmap_filename);

    if (metapixel->bitmap == 0)
	bitmap_free(bitmap);

    /* copy the metapixel */
    metapixel = copy_metapixel_for_library(metapixel, library, bitmap_filename + strlen(library->path) + 1);
    assert(metapixel != 0);

    /* add the metadata to the tables file */
    sprintf(tables_filename, "%s/%s", library->path, TABLES_FILENAME);
    file = fopen(tables_filename, "a");
    if (file == 0)
    {
	metapixel_free(metapixel);

	error_report(ERROR_TABLES_FILE_CANNOT_OPEN, error_make_string_info(tables_filename));

	return 0;
    }
    write_metapixel_metadata(metapixel, file);
    fclose(file);

    /* add the metapixel to the library */
    metapixel->next = library->metapixels;
    library->metapixels = metapixel;

    ++library->num_metapixels;

    return metapixel;
}

unsigned int
library_count_metapixels (int num_libraries, library_t **libraries)
{
    int i;
    unsigned int n = 0;

    for (i = 0; i < num_libraries; ++i)
	n += libraries[i]->num_metapixels;

    return n;
}

library_t*
library_find_or_open (int num_libraries, library_t **libraries,
		      const char *library_path,
		      int *num_new_libraries, library_t ***new_libraries)
{
    int i;
    library_t *library;

    /* FIXME: we should do better path comparison */

    for (i = 0; i < num_libraries; ++i)
	if (strcmp(libraries[i]->path, library_path) == 0)
	    return libraries[i];

    for (i = 0; i < *num_new_libraries; ++i)
	if (strcmp((*new_libraries)[i]->path, library_path) == 0)
	    return (*new_libraries)[i];

    library = library_open(library_path);

    if (library == 0)
	return 0;

    if ((*num_new_libraries)++ == 0)
	*new_libraries = (library_t**)malloc(sizeof(library_t*));
    else
	*new_libraries = (library_t**)realloc(*new_libraries, *num_new_libraries * sizeof(library_t*));
    assert(*new_libraries != 0);

    (*new_libraries)[*num_new_libraries - 1] = library;

    return library;
}

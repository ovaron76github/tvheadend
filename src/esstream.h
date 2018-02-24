/*
 *  Tvheadend
 *  Copyright (C) 2010 Andreas Öman
 *                2018 Jaroslav Kysela
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ESSTREAM_H__
#define ESSTREAM_H__

#include "descrambler.h"
#include "input/mpegts/dvb.h"

/**
 *
 */

typedef struct service service_t;
typedef struct streaming_start streaming_start_t;

typedef struct elementary_stream elementary_stream_t;
typedef struct elementary_set elementary_set_t;

/**
 * Stream, one media component for a service.
 */
struct elementary_stream {
  TAILQ_ENTRY(elementary_stream) es_link;
  TAILQ_ENTRY(elementary_stream) es_filter_link;

  uint32_t es_position;
  struct service *es_service;

  streaming_component_type_t es_type;
  int es_index;

  char *es_nicename;

  /* PID related */
  int16_t es_pid;
  uint16_t es_parent_pid;    /* For subtitle streams originating from
				a teletext stream. this is the pid
				of the teletext stream */
  int8_t es_pid_opened;      /* PID is opened */
  int8_t es_cc;              /* Last CC */

  /* CA ID's on this stream */
  struct caid_list es_caids;

  /* */
  int es_delete_me;      /* Temporary flag for deleting streams */

  /* Stream info */
  int es_frame_duration;

  int es_width;
  int es_height;

  uint16_t es_aspect_num;
  uint16_t es_aspect_den;

  char es_lang[4];           /* ISO 639 2B 3-letter language code */
  uint8_t es_audio_type;     /* Audio type */
  uint8_t es_audio_version;  /* Audio version/layer */

  uint16_t es_composition_id;
  uint16_t es_ancillary_id;

  /* Error log limiters */
  tvhlog_limit_t es_cc_log;
  /* Filter temporary variable */
  uint32_t es_filter;

  /* HBBTV PSI table (AIT) */
  mpegts_psi_table_t es_psi;
};

/*
 * Group of elementary streams
 */
struct elementary_set {
  TAILQ_HEAD(, elementary_stream) set_all;
  TAILQ_HEAD(, elementary_stream) set_filter;
  int set_subsys;
  char *set_nicename;
  uint16_t set_last_pid;
  elementary_stream_t *set_last_es;
  service_t *set_service;
};

/*
 * Prototypes
 */
void elementary_set_init
  (elementary_set_t *set, int subsys, const char *nicename, service_t *t);
void elementary_set_clean(elementary_set_t *set);
void elementary_set_update_nicename(elementary_set_t *set, const char *nicename);
void elementary_set_clean_streams(elementary_set_t *set);
void elementary_set_stream_destroy(elementary_set_t *set, elementary_stream_t *es);
void elementary_set_init_filter_streams(elementary_set_t *set);
void elementary_set_filter_build(elementary_set_t *set);
elementary_stream_t *elementary_stream_create
  (elementary_set_t *set, int pid, streaming_component_type_t type, int running);
elementary_stream_t *elementary_stream_find_(elementary_set_t *set, int pid);
elementary_stream_t *elementary_stream_type_find
  (elementary_set_t *set, streaming_component_type_t type);
static inline elementary_stream_t *elementary_stream_find
  (elementary_set_t *set, int pid)
  {
    if (set->set_last_pid != (pid))
      return elementary_stream_find_(set, pid);
    else
      return set->set_last_es;
  }
elementary_stream_t *elementary_stream_type_modify
  (elementary_set_t *set, int pid, streaming_component_type_t type, int running);
void elementary_stream_type_destroy
  (elementary_set_t *set, streaming_component_type_t type);
int elementary_stream_has_audio_or_video(elementary_set_t *set);
int elementary_stream_has_no_audio(elementary_set_t *set, int filtered);
int elementary_set_has_streams(elementary_set_t *set, int filtered);
void elementary_set_sort_streams(elementary_set_t *set);
streaming_start_t *elementary_stream_build_start(elementary_set_t *set);

#endif // ESSTREAM_H__

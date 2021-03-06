/*
 * This file is part of MPlayer.
 *
 * MPlayer is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * MPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with MPlayer; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef MPLAYER_ASFHEADER_H
#define MPLAYER_ASFHEADER_H

#include "asf.h"
#include "demuxer.h"

//#define CHECK_SEEK_DEBUG
#ifdef CHECK_SEEK_DEBUG
#define asfseek_debug mplayer_debug
#else /* else of CHECK_SEEK_DEBUG */
#define asfseek_debug(fmt, args...) {}
#endif /* end of CHECK_SEEK_DEBUG */

int asf_check_header(demuxer_t *demuxer);
int read_asf_header(demuxer_t *demuxer, struct asf_priv *asf);

//Fuchun 2010.08.27
int asf_found_index_flag;
int *asf_index_packet_num;
int asf_index_num;

#endif /* MPLAYER_ASFHEADER_H */

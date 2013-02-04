/***************************************************************************
 * Copyright (c) 2009-2010 Open Information Security Foundation
 * Copyright (c) 2010-2013 Qualys, Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.

 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.

 * - Neither the name of the Qualys, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

/**
 * @file
 * @author Ivan Ristic <ivanr@webkreator.com>
 */

#ifndef _HTP_MULTIPART_H
#define	_HTP_MULTIPART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bstr.h"
#include "htp.h"
#include "htp_table.h"


// Constants and enums

#define HTP_MULTIPART_LF_LINE                   0x0001
#define HTP_MULTIPART_CRLF_LINE                 0x0002
#define HTP_MULTIPART_BOUNDARY_LWS_AFTER        0x0004
#define HTP_MULTIPART_BOUNDARY_NLWS_AFTER       0x0008
#define HTP_MULTIPART_HAS_PREAMBLE              0x0010
#define HTP_MULTIPART_HAS_EPILOGUE              0x0020
#define HTP_MULTIPART_SEEN_LAST_BOUNDARY        0x0040
#define HTP_MULTIPART_PART_AFTER_LAST_BOUNDARY  0x0080
#define HTP_MULTIPART_PART_INCOMPLETE           0x0100

#define HTP_MULTIPART_MIME_TYPE                 "multipart/form-data"

enum htp_multipart_type_t {

    /** Unknown part. */
    MULTIPART_PART_UNKNOWN = 0,

    /** Text (parameter) part. */
    MULTIPART_PART_TEXT = 1,

    /** File part. */
    MULTIPART_PART_FILE = 2,

    /** Free-text part before the first boundary. */
    MULTIPART_PART_PREAMBLE = 3,

    /** Free-text part after the last boundary. */
    MULTIPART_PART_EPILOGUE = 4
};


// Structures

/**
 * Holds multipart parser configuration and state. Private.
 */
typedef struct htp_mpartp_t htp_mpartp_t;

/**
 * Holds information related to a multipart body.
 */
typedef struct htp_multipart_t {
    /** Multipart boundary. */
    char *boundary;

    /** Boundary length. */
    size_t boundary_len;

    /** How many boundaries were there? */
    int boundary_count;   

    /** List of parts, in the order in which they appeared in the body. */
    htp_list_t *parts;

    /** Parsing flags. */
    uint64_t flags;
} htp_multipart_t;

/**
 * Holds information related to a part.
 */
typedef struct htp_multipart_part_t {
    /** Pointer to the parser. */
    htp_mpartp_t *parser;

    /** Part type; see the MULTIPART_PART_* constants. */
    enum htp_multipart_type_t type;

    /** Raw part length (i.e., headers and data). */
    size_t len;
   
    /** Part name, from the Content-Disposition header. Can be NULL. */
    bstr *name;   

    /**
     * Part value; the contents depends on the type of the part:
     * 1) NULL for files; 2) contains complete part contents for
     * preamble and epilogue parts (they have no headers), and
     * 3) data only (headers excluded) for text and unknown parts.
     */
    bstr *value;

    /** Part content type, from the Content-Type header. Can be NULL. */
    bstr *content_type;

    /** Part headers (htp_header_t instances), using header name as the key. */
    htp_table_t *headers;

    /** File data, available only for MULTIPART_PART_FILE parts. */
    htp_file_t *file;
} htp_multipart_part_t;


// Functions

/**
 * Creates a new multipart/form-data parser.
 *
 * @param[in] boundary
 * @return New parser, or NULL on memory allocation failure.
 */
htp_mpartp_t *htp_mpartp_create(htp_cfg_t *cfg);

htp_status_t htp_mpartp_init_boundary(htp_mpartp_t *parser, bstr *c_t_header);

htp_status_t htp_mpartp_init_boundary_ex(htp_mpartp_t *parser, char *boundary);

/**
 * Returns the multipart structure created by the parser.
 *
 * @param[in] parser
 * @return The main multipart structure.
 */
htp_multipart_t *htp_mpartp_get_multipart(htp_mpartp_t *parser);

/**
 * Destroys the provided parser.
 *
 * @param[in] parser
 */
void htp_mpartp_destroy(htp_mpartp_t *parser);

/**
 * Finalize parsing.
 *
 * @param[in] parser
 * @returns HTP_OK on success, HTP_ERROR on failure.
 */
htp_status_t htp_mpartp_finalize(htp_mpartp_t *parser);

/**
 * Parses a chunk of multipart/form-data data. This function should be called
 * as many times as necessary until all data has been consumed.
 *
 * @param[in] parser
 * @param[in] data
 * @param[in] len
 * @return HTP_OK on success, HTP_ERROR on failure.
 */
htp_status_t htp_mpartp_parse(htp_mpartp_t *parser, const void *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif	/* _HTP_MULTIPART_H */

#pragma once

#include <stdint.h>

/* base64_encode:
 *  Description:
 *      Encode a string of characters as base64
 *  Parameters:
 *      output: the output buffer for the encoding, stores the encoded string
 *      input: the input buffer for the encoding, stores the binary to be encoded
 *      inputLen: the length of the input buffer, in bytes
 *  Return value:
 *      Returns the length of the encoded string
 *  Requirements:
 *      1. output must not be null or empty
 *      2. input must not be null
 *      3. inputLen must be greater than or equal to 0
 */
extern int16_t base64_encode(char *output, char *input, int16_t inputLen);

/* base64_decode:
 *  Description:
 *      Decode a base64 encoded string into bytes
 *  Parameters:
 *      output: the output buffer for the decoding,
 *       stores the decoded binary
 *      input: the input buffer for the decoding,
 *      stores the base64 string to be decoded
 *      inputLen: the length of the input buffer, in bytes
 *  Return value:
 *      Returns the length of the decoded string
 *  Requirements:
 *      1. output must not be null or empty
 *      2. input must not be null
 *      3. inputLen must be greater than or equal to 0
 */
extern int16_t base64_decode(char * output, char * input, int16_t inputLen);

/* base64_enc_len:
 *  Description:
 *      Returns the length of a base64 encoded string whose decoded
 *      form is inputLen bytes long
 *  Parameters:
 *      inputLen: the length of the decoded string
 *  Return value:
 *      The length of a base64 encoded string whose decoded form
 *      is inputLen bytes long
 *  Requirements:
 *      None
 */
extern int16_t base64_enc_len(int16_t plainLen);

/* base64_dec_len:
 *  Description:
 *      Returns the length of the decoded form of a
 *      base64 encoded string
 *  Parameters:
 *      input: the base64 encoded string to be measured
 *      inputLen: the length of the base64 encoded string
 *  Return value:
 *      Returns the length of the decoded form of a
 *      base64 encoded string
 *  Requirements:
 *      1. input must not be null
 *      2. input must be greater than or equal to zero
 */
extern int16_t base64_dec_len(char * input, int16_t inputLen);





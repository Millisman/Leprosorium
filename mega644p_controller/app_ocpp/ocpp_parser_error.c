#include "ocpp_parser.h"
#include "ocpp_strings.h"
#include "Base64.h"
#include "sha1.h"
#include "uuid.h"

#define LOG_ON

#ifdef LOG_ON
#define LOGGING(x) x
#else
#define LOGGING(x)
#endif

// [4,"82c8d8fe-434d-4855-8ce2-b34717119854","FormationViolation","The payload for action could not be deserialized",
// {"errorMsg":"Unexpected character ('c' (code 99)): was expecting a colon to separate field name and value\n at [So..."}]


// [<MessageTypeId>, "<UniqueId>", "<errorCode>", "<errorDescription>", {<errorDetails>}]
//                 const char * format = "[%d, %Q, %Q, %Q, %s]\n";
//                 const char * errorCode_key = "$[2]";
//                 const char * errorDescription_key = "$[3]";
//                 const char * errorDetails_key = "$[4]";


void OCPP_Parser_processing_error(ocpp_structure_t *ocpp) {
    LOGGING(
        printf_P(PSTR("Message 4 error - not implemented\n%s\n"), ocpp->json_payload );
    );
}







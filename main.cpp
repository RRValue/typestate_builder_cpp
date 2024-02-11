#include "http_response.h"

int main()
{
    HttpResponseBuilder<Start>::create().print()
        .status("bla").print()
        .add_header("header 1").print()
        .add_header("header 2").print()
        .header_done().print()
        .body("bodey").print()
        .send();
}
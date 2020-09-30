#include "WhdOlmInterface.h"

extern "C" MBED_WEAK const struct ol_desc *cycfg_get_default_ol_list()
{
    return NULL;
}

extern "C" MBED_WEAK const struct ol_desc *get_default_ol_list()
{
    return cycfg_get_default_ol_list();
}

WhdSTAInterface::OlmInterface&WhdSTAInterface::OlmInterface::get_default_instance()
{
    static WhdOlmInterface *olm;
    if (olm == NULL)
    {
        olm = new WhdOlmInterface(get_default_ol_list() );
    }
    return *olm;
}

